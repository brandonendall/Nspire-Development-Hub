APP=EndraHome_Run005_PreviewMatch
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../gba-ui
OBJS=build/main.o build/render.o build/logic7.o build/platform.o
.PHONY: all test preview clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c render.h logic7.h ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/render.o: render.c render.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic7.o: logic7.c logic7.h ../gba-ui/logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: ../gba-ui/platform.c ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome PreviewMatch 005" --input $< --output $@
build/test_logic7: logic7.c logic7.h test_logic7.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../gba-ui logic7.c test_logic7.c -o $@
test: build/test_logic7
	./build/test_logic7
preview: | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../../UltimateCommander/vendor -I../gba-ui render.c logic7.c preview.c -o build/preview
	./build/preview
clean:
	rm -rf build $(APP).tns
