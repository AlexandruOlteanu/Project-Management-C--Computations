# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall

# Source files
SRC = solution.cpp

# Output executable
TARGET = o_solution

build: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
