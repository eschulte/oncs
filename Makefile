CC:=gcc
CFLAGS:=-Wall -pedantic -Werror -Wundef -Winline -Wextra
SIZE:=32
QLENGTH:=1024
DEBUG_P=0
BUILD=$(CC) $(CFLAGS) -D SIZE=$(SIZE) \
	-D QLENGTH=$(QLENGTH) -D DEBUG_P=$(DEBUG_P)
READLINE_LIB:=-lreadline
LIB=src/oncs.c src/oncs.h
TEST_LIB=test/test.c test/test.h
IXM_BASEDIR:=/usr/local/src/ixm/sfb/src/template/../..
USB_DEVICE:=/dev/ttyUSB0
TEST =
TESTS = \
	test/open_space	\
	test/queue	\
	test/coords	\
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
	test/y-comb-2	\
	test/fact-o-4

all: vm repl

vm: $(LIB) $(TEST_LIB) src/vm.c
	$(BUILD) -Isrc/ -Itest/ -o vm $^

repl: $(LIB) $(TEST_LIB) src/repl.c
	$(BUILD) -Isrc/ -Itest/ $(READLINE_LIB) -o repl $^

test/%.test: $(LIB) $(TEST_LIB) test/%.c
	$(BUILD) -Itest/ -Isrc/ -o test/$*.test $^

test/%.prof: $(LIB) $(TEST_LIB) test/%.c
	$(BUILD) -pg -Itest/ -Isrc/ -o test/$*.test $^ && \
	./test/$*.test && \
	gprof ./test/$*.test > test/$*.prof

check: $(TESTS:=.test)
	@for test in $(TESTS:=.test);do \
		./$$test 1>/dev/null; \
		if [[ "$$?" -eq "0" ]];then result=PASS; \
		else result=FAIL;fi; \
		echo "$$result $$test"; \
	done

play: $(TEST:=.test)
	@if [[ -z "$(TEST)" ]]; then \
		echo "must specify a test executable"; \
	else if [[ -z "$$WINDOW" ]];then \
		echo "must be run from within a screen session"; \
	else if ps aux|grep more|grep -v grep >/dev/null;then \
		echo "something matching 'ps aux|grep more' is already running"; \
	else \
		PID=""; \
		(while [[ -z "$$PID" ]];do \
			PID=$$(ps aux|grep more|grep -v grep|awk '{print $$2}'); \
		done; \
		while kill -0 $$PID 2>/dev/null;do \
			screen -X -p "$$WINDOW" stuff " "; sleep 0.0005; \
		done) & \
		./$(TEST:=.test) -v|more -c; \
	fi; fi; fi

ixm/Makefile:
	SKETCH_DIR="$$(pwd)/ixm"; \
	$(MAKE) -C $(IXM_BASEDIR) SKETCH_DIR=$$SKETCH_DIR sketchinit;

ixm/sketch.hex: ixm/Makefile ixm/sketch.pde ixm/test.o
	$(MAKE) -C ixm/ sketch.hex SKETCH_EXTRA_CPPFLAGS="-I../src/ -I../test/"

burn: ixm/sketch.hex
	SKETCH_DIR="$$(pwd)/ixm"; \
	pushd $(IXM_BASEDIR); \
	./targets/host/bin/sfbdl -f $$SKETCH_DIR/sketch.hex -d $(USB_DEVICE)

etags: $(LIB) $(TESTS:=.c) $(TEST_LIB)
	etags $^

clean:
	rm -f vm repl gmon.out test/*.test \
	ixm/sketch-wrapper.cpp ixm/sketch-wrapper.o ixm/sketch ixm/sketch.hex ixm/Makefile
