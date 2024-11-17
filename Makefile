CXX = gcc
CXXFLAGS = -Wall -Wextra
DEBUG_FLAGS = -g -DDEBUG


SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = cson

ifeq ($(DEBUG), 1)
	CXXFLAGS += $(DEBUG_FLAGS)
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(TARGET)
