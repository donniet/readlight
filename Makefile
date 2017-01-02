CXX=g++
RM=rm -f
LDFLAGS=-lwiringPi -lboost_program_options
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

all: readLight

readLight: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

include .depend
