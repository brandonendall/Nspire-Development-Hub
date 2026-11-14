APP=EndraHome_Run007_ZIPLauncher
TESTAPP=EndraHome_Test006
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../gba-ui -I../run005
OBJS=build/main.o build/render.o build/overlay.o build/ui.o build/logic7.o build/platform.o
.PHONY: all test preview clean
all: $(APP).tns $(TESTAPP).tns
build:
	mkdir -p build
build/main.o: main.c ui.h overlay.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/render.o: ../run006/render.c ../run005/render.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/overlay.o: overlay.c overlay.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/ui.o: ui.c ui.h ../run005/logic7.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic7.o: ../run005/logic7.c ../run005/logic7.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: ../gba-ui/platform.c ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome ZIP 007" --input $< --output $@
build/target.o: ../run006/target.c | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(TESTAPP).elf: build/target.o
	$(LD) -marm $^ -o $@ -lm
$(TESTAPP).tns: build/$(TESTAPP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome Test 006" --input $< --output $@
build/test: ui.c ../run005/logic7.c test.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../gba-ui -I../run005 ui.c ../run005/logic7.c test.c -o $@
test: build/test
	./build/test
build/preview: ../run006/render.c ui.c overlay.c preview.c ../run005/logic7.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../../UltimateCommander/vendor -I../gba-ui -I../run005 ../run006/render.c overlay.c ui.c ../run005/logic7.c preview.c -o $@
preview: build/preview
	./build/preview
clean:
	rm -rf build $(APP).tns $(TESTAPP).tns
