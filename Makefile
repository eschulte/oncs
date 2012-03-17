CC=gcc
TESTS = \
	test/open_space	\
	test/queue

all: vm

vm: oncs.c oncs.h vm.c
	$(CC) -o vm vm.c oncs.c

test/%.test: oncs.c oncs.h test/test.c test/test.h test/%.c
	$(CC) -Ltest/ -o test/$*.test oncs.c test/test.c test/$*.c

check: $(TESTS:=.test)
	for test in test/*.test;do \
		./$$test; \
		if [[ "$$?" -eq "0" ]];then result=PASS;else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

clean:
	rm -f vm test/*.test
