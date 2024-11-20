CXX = gcc
CXXFLAGS = -Wall -Wextra
DEBUG_FLAGS = -g -DDEBUG

SRC = $(wildcard cson/*.c)
OBJ = $(SRC:.c=.o)

BASIC_OBJ=basic_example.o $(OBJ)
BASIC_TARGET = basic

NESTED_OBJECTS_OBJ=nested_objects_example.o $(OBJ)
NESTED_OBJECTS_TARGET = nested_objects

NESTED_ARRAYS_OBJ=nested_arrays_example.o $(OBJ)
NESTED_ARRAYS_TARGET = nested_arrays

ifeq ($(DEBUG), 1)
	CXXFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(SANITIZE), 1)
	CXXFLAGS += -fsanitize=address
endif

$(BASIC_TARGET): $(BASIC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NESTED_OBJECTS_TARGET): $(NESTED_OBJECTS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NESTED_ARRAYS_TARGET): $(NESTED_ARRAYS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BASIC_OBJ)  $(BASIC_TARGET) $(NESTED_OBJECTS_OBJ) $(NESTED_OBJECTS_TARGET) $(NESTED_ARRAYS_OBJ) $(NESTED_ARRAYS_TARGET)
