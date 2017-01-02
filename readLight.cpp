
#include <iostream>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <boost/program_options.hpp>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <chrono>

using namespace std;
namespace po = boost::program_options;

// channel is the wiringPi name for the chip select (or chip enable) pin.
// Set this to 0 or 1, depending on how it's connected.
static const int CHANNEL = 1;

#define LIGHT_PIN 21

volatile bool interrupted = false;

void handleInt(int sig) {
  interrupted = true;
}

int main(int ac, char * av[])
{
  int fd, result;
  unsigned char buffer;
  int lightPin = LIGHT_PIN;
  int channel = CHANNEL;
  bool lit = false;
  int minValue = 128;
  double delay = 1000.0;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("lightPin,p", po::value<int>(&lightPin)->default_value(LIGHT_PIN), "light pin (BCM)")
    ("channel,c", po::value<int>(&channel)->default_value(CHANNEL), "chip select channel")
    ("minValue,m", po::value<int>(&minValue)->default_value(128), "minimum light value")
    ("delay,d", po::value<double>(&delay)->default_value(1000.0), "delay between readings (ms)");

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);

  if (vm.count("help") > 0) {
    cout << desc << endl;
    return 1;
  }

  cout << "Initializing" << endl ;

  signal(SIGINT, handleInt);

  std::chrono::duration<double, std::milli> d(delay);

  // Configure the interface.
  // CHANNEL insicates chip select,
  // 500000 indicates bus speed.
  wiringPiSetupGpio();
  digitalWrite(lightPin, LOW);
  lit = false;
  fd = wiringPiSPISetup(channel, 500000);

  cout << "Init result: " << fd << endl;

  // clear display
  while(!interrupted) {
   wiringPiSPIDataRW(channel, &buffer, 1);

   cout << "value: " << (int)buffer << endl;

   if (buffer <= minValue && !lit) {
     cout << "light on" << endl;
     digitalWrite(lightPin, HIGH);
     lit = true;
   } else if (buffer > minValue && lit) {
     cout << "light off" << endl;
     digitalWrite(lightPin, LOW);
     lit = false;
   }

   std::this_thread::sleep_for(d);


  }

  digitalWrite(lightPin, LOW);
}
