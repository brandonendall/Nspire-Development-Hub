NKITS ?= ../../../nkits
BUILD := build
TARGET := EndraTask_Run004
NATIVE_CFLAGS := -std=gnu99 -Wall -Wextra -Os -marm -ffunction-sections -fdata-sections
NATIVE_CPPFLAGS := -I$(NKITS)
OBJECTS := $(addprefix $(BUILD)/,main.o ui.o draw.o)
HOST_FLAGS := -std=c99 -Wall -Wextra -Werror -pedantic -Itests

.PHONY: all host-test clean
all: $(TARGET).tns

$(BUILD):
	mkdir -p $@

$(BUILD)/main.o: main.c $(NKITS)/common/ui.h $(NKITS)/common/draw.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/ui.o: $(NKITS)/common/ui.c $(NKITS)/common/ui.h $(NKITS)/common/draw.h $(NKITS)/version.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/draw.o: $(NKITS)/common/draw.c $(NKITS)/common/draw.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJECTS)
	nspire-ld -Wl,--gc-sections $^ -o $@

$(TARGET).tns: $(BUILD)/$(TARGET).elf
	genzehn --input "$<" --output "$(BUILD)/$(TARGET).zehn" --name EndraTask --240x320-support true --uses-lcd-blit true
	make-prg "$(BUILD)/$(TARGET).zehn" "$@"

$(BUILD)/test_main.o: main.c tests/libndls.h tests/common/ui.h | $(BUILD)
	$(CC) $(HOST_FLAGS) -Dmain=endratask_main -c $< -o $@

$(BUILD)/test_lifecycle: tests/test_lifecycle.c $(BUILD)/test_main.o tests/libndls.h tests/common/ui.h
	$(CC) $(HOST_FLAGS) $< $(BUILD)/test_main.o -o $@

host-test: $(BUILD)/test_lifecycle
	$(BUILD)/test_lifecycle

clean:
	rm -rf $(BUILD) $(TARGET).tns
