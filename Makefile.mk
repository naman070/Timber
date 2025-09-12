# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I D:/cpp_libraries/SFML-3.0.0/include

# Linker flags (added -mwindows)
LDFLAGS = -L D:/cpp_libraries/SFML-3.0.0/lib -lsfml-graphics -lsfml-window -lsfml-system -mwindows -lopengl32 -lsfml-audio

# Source files (all .cpp in current dir)
SRC = $(wildcard *.cpp)

# Object files
OBJ = $(SRC:.cpp=.o)

# Output executable
OUT = game.exe

# Default target
all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o *.exe 2>nul || true