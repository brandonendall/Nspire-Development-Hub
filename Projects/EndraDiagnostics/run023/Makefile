APP=EndraDiagnostics_Run023
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../run002 -I../run004
OBJS=build/main.o build/platform.o build/battery_native.o build/battery_adapter.o
.PHONY: all clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c ../run004/vendor/battery_native.h ../run002/platform.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: platform.c ../run002/platform.h ../run002/logic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/battery_native.o: ../run004/vendor/battery_native.c ../run004/vendor/battery_native.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/battery_adapter.o: ../run004/battery_adapter.c ../run004/vendor/arm926_ram_span.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraDiagnostics" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "Endra System Monitor 023" --input $< --output $@
clean:
	rm -rf build $(APP).tns
