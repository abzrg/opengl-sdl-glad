BUILDDIR = build
SRCDIR = src
INCDIR = include

CC = clang
CXX = clang++

ASANFLAGS = -fsanitize=address # buffer overflows, use-after-free, use-after-scope, and memory leaks
ASANFLAGS += -fsanitize=undefined # integer overflows, invalid pointer arithmetic, ...
# ASANFLAGS += -fsanitize=memory # uninitialized memory reads (only supported on Linux).
# ASANFLAGS += -fsanitize=thread # Detects data races in multi-threaded programs
ASANFLAGS += -fsanitize=leak # Specifically detects memory leaks (included in Address Sanitizer by
                            # default
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g -O0 -I$(INCDIR) $(ASANFLAGS)
CXXFLAGS = -Wall -Wextra -std=c++20 -pedantic -g -O0 -I$(INCDIR) $(ASANFLAGS)

LDLIBS = -lSDL2 -ldl

CXX_SOURCES = $(wildcard $(SRCDIR)/*.cpp)
C_SOURCES = $(wildcard $(SRCDIR)/*.c)
CXX_OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(CXX_SOURCES))
C_OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(C_SOURCES))

TARGET = $(BUILDDIR)/prog
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS)


define compile
	echo '[Deps] Generating dependency files...'; \
    echo '[CtoO] $@'; \
	$(1) $(2) -MMD -c $< -o $@
endef

define link
	echo '[Link] $^ -> $@'; \
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	echo [DONE]
endef


all: $(TARGET)


$(TARGET): $(OBJECTS)
	@$(call link)


$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@$(call compile,$(CXX),$(CXXFLAGS))


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@$(call compile,$(CC),$(CFLAGS))


# Include dependency files if they exist
-include $(OBJECTS:.o=.d)


clean:
	$(RM) -v $(BUILDDIR)/*


.PHONY: all clean
