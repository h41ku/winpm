all: rebuild

memory.o:
	gcc -c -Wall -o memory.o memory.c

service.o:
	gcc -c -Wall -o service.o service.c

string.o:
	gcc -c -Wall -o string.o string.c

winpm.o:
	gcc -c -Wall -o winpm.o winpm.c

main.o:
	gcc -c -Wall -o main.o main.c

winpm: memory.o service.o string.o winpm.o main.o
	ld -static --subsystem windows -o winpm.exe main.o winpm.o string.o service.o memory.o -luser32 -ladvapi32 -lkernel32 -lshell32
	strip winpm.exe

clean:
	rm -f winpm.exe main.o winpm.o string.o service.o memory.o

rebuild: clean winpm
