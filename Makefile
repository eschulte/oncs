CC=gcc -g
READLINE_LIB=-lreadline
LIB = src/oncs.c src/oncs.h
TEST_LIB= test/test.c test/test.h
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
	test/l-promote
#	test/fact-0	\
	test/y-comb

all: vm repl

vm: $(LIB) $(TEST_LIB) src/vm.c
	$(CC) -Isrc/ -Itest/ -o vm $^

repl: $(LIB) $(TEST_LIB) src/repl.c
	$(CC) -Isrc/ -Itest/ $(READLINE_LIB) -o repl $^

test/%.test: $(LIB) $(TEST_LIB) test/%.c
	$(CC) -Itest/ -Isrc/ -o test/$*.test $^

check: $(TESTS:=.test)
	for test in $(TESTS:=.test);do \
		./$$test 2>/dev/null; \
		if [[ "$$?" -eq "0" ]];then result=PASS; \
		else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

etags: $(LIB) $(TESTS:=.c) $(TEST_LIB)
	etags $^

clean:
	rm -f vm repl test/*.test
