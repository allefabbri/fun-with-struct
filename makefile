all: struct

struct: fun_with_struct.c
	gcc -o example.exe fun_with_struct.c
	./example.exe