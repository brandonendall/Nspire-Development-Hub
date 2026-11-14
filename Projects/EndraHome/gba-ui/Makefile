APP=EndraHome_Run003_GBAUI
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor
OBJS=build/main.o build/platform.o build/logic.o
.PHONY: all test clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c platform.h logic.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: platform.c platform.h logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic.o: logic.c logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome GBA UI" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome GBA UI 003" --input $< --output $@
build/test_logic: logic.c logic.h tests/test_logic.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I. logic.c tests/test_logic.c -o $@
test: build/test_logic
	./build/test_logic
clean:
	rm -rf build $(APP).tns
