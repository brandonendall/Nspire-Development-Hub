NKITS ?= ../../../nkits
BUILD := build
TARGET := EndraTask_Run001
SOURCES := main.c probe.c $(NKITS)/TaskManager/src/tasks.c $(NKITS)/common/ui.c $(NKITS)/common/draw.c
NATIVE_CFLAGS := -std=gnu99 -Wall -Wextra -Os -marm -ffunction-sections -fdata-sections
NATIVE_CPPFLAGS := -I. -I$(NKITS) -I$(NKITS)/TaskManager/src

.PHONY: all host-test clean
all: $(TARGET).tns

$(BUILD):
	mkdir -p $@

$(BUILD)/native.c: $(NKITS)/TaskManager/src/native.c native-readonly.patch | $(BUILD)
	patch --batch --fuzz=0 -i "$(CURDIR)/native-readonly.patch" -o "$@" "$<"

$(BUILD)/$(TARGET).elf: $(SOURCES) $(BUILD)/native.c probe.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c main.c -o $(BUILD)/main.o
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c probe.c -o $(BUILD)/probe.o
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $(BUILD)/native.c -o $(BUILD)/native.o
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $(NKITS)/TaskManager/src/tasks.c -o $(BUILD)/tasks.o
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $(NKITS)/common/ui.c -o $(BUILD)/ui.o
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $(NKITS)/common/draw.c -o $(BUILD)/draw.o
	nspire-ld -Wl,--gc-sections $(addprefix $(BUILD)/,main.o probe.o native.o tasks.o ui.o draw.o) -o $@

$(TARGET).tns: $(BUILD)/$(TARGET).elf
	genzehn --input "$<" --output "$(BUILD)/$(TARGET).zehn" --name EndraTask --240x320-support true --uses-lcd-blit true
	make-prg "$(BUILD)/$(TARGET).zehn" "$@"

$(BUILD)/test_probe: probe.c probe.h test_probe.c | $(BUILD)
	$(CC) -std=c99 -Wall -Wextra -Werror -pedantic probe.c test_probe.c -o "$@"

host-test: $(BUILD)/test_probe $(BUILD)/native.c
	$(BUILD)/test_probe
	! grep -Eq 'discover_objects|native_action' $(BUILD)/native.c
	grep -F 'capacity = 65' $(BUILD)/native.c

clean:
	rm -rf $(BUILD) $(TARGET).tns
