APP=EndraDiagnostics_Run002
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor
OBJS=build/main.o build/platform.o
.PHONY: all clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c platform.h logic.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: platform.c platform.h logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraDiagnostics" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "Endra Diagnostics 002" --input $< --output $@
clean:
	rm -rf build $(APP).tns
