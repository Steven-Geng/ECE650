CXX = g++
CXXFLAGS = -Wall -std=c++98

all: player ringmaster

player: player.cpp player.h potato.cpp potato.h helperFns.h
	$(CXX) $(CXXFLAGS) -o player player.cpp potato.cpp

ringmaster: ringmaster.cpp ringmaster.h potato.h helperFns.h
	$(CXX) $(CXXFLAGS) -o ringmaster ringmaster.cpp potato.cpp

clean:
	rm -f player ringmaster
