BUILDDIR = build

CC = clang
CXX = clang++

CFLAGS = -Wall -Wextra -std=c11 -pedantic -g -O0 -I./include
CXXFLAGS = -Wall -Wextra -std=c++20 -pedantic -g -O0 -I./include
LDLIBS = -lSDL2 -ldl


all: build/prog


build/prog: build/main.o build/glad.o build/App.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)


build/main.o: src/main.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@


build/App.o: include/App/App.h
build/App.o: src/App.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@


build/glad.o: include/glad/glad.h
build/glad.o: src/glad.c
	$(CC) -c $< $(CFLAGS) -o $@


clean:
	rm -fv build/prog build/*.o


.PHONY: all clean
