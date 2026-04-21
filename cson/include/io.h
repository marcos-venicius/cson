#ifndef CSON_IO_H_
#define CSON_IO_H_

unsigned long read_file_content(const char* filename, char** output);
unsigned long read_from_stdin(char **content);

#endif // CSON_IO_H_
