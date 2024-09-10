CC = gcc
CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++20 -pedantic
CXXFLAGS += -g -O0
CXXFLAGS += `pkg-config --cflags sdl2` `pkg-config --cflags glm`
LDLIBS = `pkg-config --libs sdl2` `pkg-config --libs glm` -ldl

BUILDDIR = build
SRCS = $(wildcard *.cpp)
TARGETS = $(addprefix $(BUILDDIR)/,$(patsubst %.cpp,%,$(SRCS)))


all: $(TARGETS)


build:
	mkdir build


$(BUILDDIR)/%: %.cpp | build
	$(CXX) $(CXXFLAGS) $< $(LDLIBS) -o $@


clean:
	@$(RM) -v $(TARGETS)


.PHONY: all clean
