# Define the compiler and the flags
CXX = clang++
CXXFLAGS = -Wall -g -Iinclude -O3 -std=c++20 
# -Wextra -Werror

# Define the target executable
TARGET = myprogram

# Define the directories
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Define the source files and the object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile the source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up the build files
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Phony targets
.PHONY: all clean

