
all: oncs

oncs: oncs.c oncs.h
	gcc -o oncs oncs.c

test/open_space: oncs.c oncs.h test/test.c test/test.h test/open_space.c
	gcc -L./ -Ltest/ -o test/open_space oncs.c test/test.c test/open_space.c

clean:
	rm -f oncs test/open_space
