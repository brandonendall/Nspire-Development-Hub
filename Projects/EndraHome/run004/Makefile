APP=EndraHome_Run004_Visual
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../gba-ui
OBJS=build/main.o build/render.o build/platform.o build/logic.o
.PHONY: all test preview clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c render.h ../gba-ui/platform.h ../gba-ui/logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/render.o: render.c render.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: ../gba-ui/platform.c ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic.o: ../gba-ui/logic.c ../gba-ui/logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome Visual 004" --input $< --output $@
build/test_logic: ../gba-ui/logic.c ../gba-ui/logic.h ../gba-ui/tests/test_logic.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../gba-ui ../gba-ui/logic.c ../gba-ui/tests/test_logic.c -o $@
test: build/test_logic
	./build/test_logic
preview: | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../../UltimateCommander/vendor -I../gba-ui render.c ../gba-ui/logic.c preview.c -o build/preview
	cd . && ./build/preview
clean:
	rm -rf build $(APP).tns
