CXX = gcc
CXXFLAGS = -Wall -Wextra


SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = cson

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(TARGET)
