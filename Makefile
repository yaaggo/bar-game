CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lfreeglut -lglu32 -lopengl32

SRC = main.cpp src/game.cpp src/render.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = bar_game.exe

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q main.o src\*.o $(EXEC)