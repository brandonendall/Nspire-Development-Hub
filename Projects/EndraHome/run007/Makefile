APP=EndraHome_Run007_Universal
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../gba-ui -I../run005
OBJS=build/main.o build/render.o build/overlay.o build/registry.o build/ui.o build/logic7.o build/platform.o
.PHONY: all test preview clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c ui.h registry.h render.h overlay.h ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/render.o: render.c registry.h render.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/overlay.o: overlay.c overlay.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/registry.o: registry.c registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/ui.o: ui.c ui.h registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic7.o: ../run005/logic7.c ../run005/logic7.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: ../gba-ui/platform.c ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome Universal 007" --input $< --output $@
build/test: registry.c registry.h ui.c ../run005/logic7.c test.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../gba-ui -I../run005 registry.c ui.c ../run005/logic7.c test.c -o $@
test: build/test
	./build/test
build/preview: render.c overlay.c registry.c ui.c ../run005/logic7.c preview.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../../UltimateCommander/vendor -I../gba-ui -I../run005 render.c overlay.c registry.c ui.c ../run005/logic7.c preview.c -o $@
preview: build/preview
	./build/preview
clean:
	rm -rf build $(APP).tns
