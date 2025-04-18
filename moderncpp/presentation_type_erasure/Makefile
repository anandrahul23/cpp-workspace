# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

# Source files
SRCS = $(wildcard *.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = main

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean