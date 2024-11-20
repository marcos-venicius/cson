CXX = gcc
CXXFLAGS = -Wall -Wextra
DEBUG_FLAGS = -g -DDEBUG

SRC = $(wildcard cson/*.c)
OBJ = $(SRC:.c=.o)

BASIC_OBJ=basic_example.o $(OBJ)
BASIC_TARGET = basic

ifeq ($(DEBUG), 1)
	CXXFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(SANITIZE), 1)
	CXXFLAGS += -fsanitize=address
endif

$(BASIC_TARGET): $(BASIC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BASIC_OBJ) $(OBJ) $(BASIC_TARGET)
