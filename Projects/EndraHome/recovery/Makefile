APP=EndraHome_Run002_Safe
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor
OBJS=build/main.o build/state.o
.PHONY: all clean test
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c state.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/state.o: state.c state.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --input $< --output build/$(APP).zehn --name "EndraHome Safe 002" --ndless-rev-min 2004
	make-prg build/$(APP).zehn $@
build/test_state: state.c state.h tests/test_state.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I. state.c tests/test_state.c -o $@
test: build/test_state
	./build/test_state
clean:
	rm -rf build $(APP).tns
