
all: oncs

oncs: oncs.c oncs.h
	gcc -o oncs oncs.c

clean:
	rm -f oncs
