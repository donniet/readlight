
#include <iostream>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <boost/program_options.hpp>
#include <unistd.h>

using namespace std;
namespace po = boost::program_options;

// channel is the wiringPi name for the chip select (or chip enable) pin.
// Set this to 0 or 1, depending on how it's connected.
static const int CHANNEL = 1;

#define LIGHT_PIN 21

int main(int ac, char * av[])
{
  int fd, result;
  unsigned char buffer;
  int lightPin = LIGHT_PIN;
  bool lit = false;
  int delay = 1;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("lightPin", po::value<int>(&lightPin)->default_value(LIGHT_PIN), "light pin (BCM)")
    ("delay", po::value<int>(&delay)->default_value(1), "delay between readings");

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);

  if (vm.count("help") > 0) {
    cout << desc << endl;
    return 1;
  }

  cout << "Initializing" << endl ;

  // Configure the interface.
  // CHANNEL insicates chip select,
  // 500000 indicates bus speed.
  wiringPiSetupGpio();
  digitalWrite(lightPin, LOW);
  lit = false;
  fd = wiringPiSPISetup(CHANNEL, 500000);

  cout << "Init result: " << fd << endl;

  // clear display
  while(true) {
   wiringPiSPIDataRW(CHANNEL, &buffer, 1);

   cout << "value: " << (int)buffer << endl;

   if (buffer < 128 && !lit) {
     cout << "light on" << endl;
     digitalWrite(lightPin, HIGH);
     lit = true;
   } else if (buffer >= 128 && lit) {
     cout << "light off" << endl;
     digitalWrite(lightPin, LOW);
     lit = false;
   }

   sleep(1);


  }
}
