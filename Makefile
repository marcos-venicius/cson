CXX = gcc
CXX_FLAGS = -Wall -Wextra -pedantic
DEBUG_FLAGS = -g -DDEBUG
EXE_NAME = csonf

$(EXE_NAME): csonf.o cson/cson.o cson/libs/ll.o cson/io.o cson/utils.o cson/lexer.o cson/st_parser.o cson/common.o
	$(CXX) $(CXX_FLAGS) -o $(EXE_NAME) $^

csonf.o: csonf.c cson/include/cson.h
	$(CXX) $(CXX_FLAGS) -c csonf.c -o csonf.o

cson.o: cson/cson.c cson/include/cson.h cson/include/lexer.h cson/include/st_parser.h cson/include/utils.h cson/include/io.h cson/include/common.h cson/libs/assertf.h cson/libs/ll.h
	$(CXX) $(CXX_FLAGS) -o cson.o -c cson/cson.c

ll.o: cson/libs/ll.c cson/libs/ll.h cson/libs/assertf.h
	$(CXX) $(CXX_FLAGS) -o ll.o -c cson/libs/ll.c

io.o: cson/io.c cson/include/io.h
	$(CXX) $(CXX_FLAGS) -o io.o cson/io.c

utils.o: cson/utils.c cson/include/utils.h
	$(CXX) $(CXX_FLAGS) -o utils.o cson/utils.c

lexer.o: cson/lexer.c cson/include/lexer.h cson/include/common.h
	$(CXX) $(CXX_FLAGS) -o lexer.o cson/lexer.c

st_parser.o: cson/st_parser.c cson/include/st_parser.h cson/include/lexer.h cson/libs/ll.h
	$(CXX) $(CXX_FLAGS) -o st_parser.o cson/st_parser.c

common.o: cson/common.c cson/include/common.h cson/include/lexer.h cson/include/st_parser.h cson/libs/ll.h
	$(CXX) $(CXX_FLAGS) -o common.o cson/common.c

clean:
	rm -rf `find . -type f -name "*.o"`
