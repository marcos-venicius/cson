CXX = gcc
CXXFLAGS = -Wall -Wextra
DEBUG_FLAGS = -g -DDEBUG

SRC = $(wildcard cson/*.c)
SRC += $(wildcard cson/libs/*.c)
OBJ = $(SRC:.c=.o)

BASIC_OBJ = basic_example.o $(OBJ)
BASIC_TARGET = basic

ST_OBJ = st_example.o $(OBJ)
ST_TARGET = st

NESTED_OBJECTS_OBJ = nested_objects_example.o $(OBJ)
NESTED_OBJECTS_TARGET = nested_objects

NESTED_ARRAYS_OBJ = nested_arrays_example.o $(OBJ)
NESTED_ARRAYS_TARGET = nested_arrays

NESTED_STUFF_OBJ = nested_stuff_example.o $(OBJ)
NESTED_STUFF_TARGET = nested_stuff

REAL_JSON_OBJ = real_json_example.o $(OBJ)
REAL_JSON_TARGET = real_json

ifeq ($(DEBUG), 1)
	CXXFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(SANITIZE), 1)
	CXXFLAGS += -fsanitize=address
endif

$(ST_TARGET): $(ST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BASIC_TARGET): $(BASIC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NESTED_OBJECTS_TARGET): $(NESTED_OBJECTS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NESTED_ARRAYS_TARGET): $(NESTED_ARRAYS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NESTED_STUFF_TARGET): $(NESTED_STUFF_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(REAL_JSON_TARGET): $(REAL_JSON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(ST_OBJ)  $(ST_TARGET) $(BASIC_OBJ)  $(BASIC_TARGET) $(NESTED_OBJECTS_OBJ) $(NESTED_OBJECTS_TARGET) $(NESTED_ARRAYS_OBJ) $(NESTED_ARRAYS_TARGET) $(NESTED_STUFF_OBJ) $(NESTED_STUFF_TARGET) $(REAL_JSON_OBJ) $(REAL_JSON_TARGET)
