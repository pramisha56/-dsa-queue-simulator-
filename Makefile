all: main

main: main.c
	gcc -I src/include -L src/lib -o main main.c -lmingw32 -lSDL3main -lSDL3

clean:
	rm -f main

