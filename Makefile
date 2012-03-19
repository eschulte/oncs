CC=gcc
LIB = src/oncs.c src/oncs.h
TESTS = \
	test/open_space	\
	test/queue	\
	test/duplicate	\
	test/app	\
	test/expr	\
	test/paren	\
	test/parse

all: vm

vm: $(LIB) src/vm.c
	$(CC) -o vm -Isrc/ src/vm.c src/oncs.c

test/%.test: $(LIB) test/test.c test/test.h test/%.c
	$(CC) -Itest/ -Isrc/ -o test/$*.test src/oncs.c test/test.c test/$*.c

check: $(TESTS:=.test)
	for test in test/*.test;do \
		./$$test; \
		if [[ "$$?" -eq "0" ]];then result=PASS; \
		else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

etags: $(LIB) $(TESTS:=.c) test/test.c test/test.h
	etags $^

clean:
	rm -f vm test/*.test
