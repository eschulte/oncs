CC=gcc

all: vm

vm: oncs.c oncs.h vm.c
	$(CC) -o vm vm.c oncs.c

test/open_space.test: oncs.c oncs.h test/test.c test/test.h test/open_space.c
	$(CC) -Ltest/ -o test/open_space.test oncs.c test/test.c test/open_space.c

check: test/open_space.test
	for test in test/*.test;do \
		./$$test; \
		if [[ "$$?" -eq "0" ]];then result=PASS;else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

clean:
	rm -f vm test/*.test
