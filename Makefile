CC=gcc
CFLAGS=
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
	test/app-4	\
	test/expr	\
	test/paren	\
	test/parse	\
	test/cond	\
	test/bool	\
	test/prim-opt	\
	test/prim-cond	\
	test/cbv	\
	test/fact-0	\
	test/examp-1	\
	test/examp-2	\
	test/l-promote	\
	test/fact-1	\
	test/de-nest	\
	test/fact-2	\
	test/fact-3	\
	test/y-comb-1	\
	test/examp-3	\
	test/examp-4	\
	test/y-comb-2	\
	test/fact-o-1	\
	test/fact-o-2
# too slow
#	test/fact-o-3
#	test/fact-4
#	test/fact
#	test/y-comb

all: vm repl

vm: $(LIB) $(TEST_LIB) src/vm.c
	$(CC) $(CFLAGS) -Isrc/ -Itest/ -o vm $^

repl: $(LIB) $(TEST_LIB) src/repl.c
	$(CC) $(CFLAGS) -Isrc/ -Itest/ $(READLINE_LIB) -o repl $^

test/%.test: $(LIB) $(TEST_LIB) test/%.c
	$(CC) $(CFLAGS) -Itest/ -Isrc/ -o test/$*.test $^

test/%.prof: $(LIB) $(TEST_LIB) test/%.c
	$(CC) $(CFLAGS) -pg -Itest/ -Isrc/ -o test/$*.test $^ && \
	./test/$*.test && \
	gprof ./test/$*.test > test/$*.prof

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
	rm -f vm repl gmon.out test/*.test
