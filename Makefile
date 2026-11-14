CXX = g++
CXXFLAGS = -Wall -Wextra -I include
LDFLAGS = -lSDL2

all: chip8

chip8: src/*.cpp
	$(CXX) $(CXXFLAGS) src/*.cpp -o chip8 $(LDFLAGS)

clean:
	rm -f chip8
