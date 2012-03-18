CC=gcc
TESTS = \
	test/open_space	\
	test/queue	\
	test/duplicate	\
	test/app

all: vm

vm: src/oncs.c src/oncs.h src/vm.c
	$(CC) -o vm -Isrc/ src/vm.c src/oncs.c

test/%.test: src/oncs.c src/oncs.h test/test.c test/test.h test/%.c
	$(CC) -Itest/ -Isrc/ -o test/$*.test src/oncs.c test/test.c test/$*.c

check: $(TESTS:=.test)
	for test in test/*.test;do \
		./$$test; \
		if [[ "$$?" -eq "0" ]];then result=PASS; \
		else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

clean:
	rm -f vm test/*.test
