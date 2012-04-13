CC=gcc -g
LIB = src/oncs.c src/oncs.h
TESTS = \
	test/open_space	\
	test/queue	\
	test/int-msg	\
	test/duplicate	\
	test/app-1	\
	test/app-2	\
	test/app-3	\
	test/app	\
	test/expr	\
	test/paren	\
	test/parse	\
	test/cond	\
	test/prim-opt	\
	test/prim-cond	\
	test/fact-0	\
	test/y-comb

all: vm

vm: $(LIB) src/vm.c
	$(CC) -o vm -Isrc/ src/vm.c src/oncs.c

test/%.test: $(LIB) test/test.c test/test.h test/%.c
	$(CC) -Itest/ -Isrc/ -o test/$*.test $^

check: $(TESTS:=.test)
	for test in $(TESTS:=.test);do \
		./$$test 2>/dev/null; \
		if [[ "$$?" -eq "0" ]];then result=PASS; \
		else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

etags: $(LIB) $(TESTS:=.c) test/test.c test/test.h
	etags $^

clean:
	rm -f vm test/*.test
