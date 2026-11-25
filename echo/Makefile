CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

all: echo

echo: echo.cpp
	$(CXX) $(CXXFLAGS) -o echo echo.cpp

clean:
	rm -f echo *.o

.PHONY: all clean
