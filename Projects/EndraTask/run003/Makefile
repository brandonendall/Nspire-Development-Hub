NKITS ?= ../../../nkits
BUILD := build
TARGET := EndraTask_Run003
NATIVE_CFLAGS := -std=gnu99 -Wall -Wextra -Os -marm -ffunction-sections -fdata-sections
NATIVE_CPPFLAGS := -I. -I$(NKITS)
OBJECTS := $(addprefix $(BUILD)/,main.o diagnostic.o ui.o draw.o)

.PHONY: all host-test clean
all: $(TARGET).tns

$(BUILD):
	mkdir -p $@

$(BUILD)/main.o: main.c diagnostic.h $(NKITS)/common/ui.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/diagnostic.o: diagnostic.c diagnostic.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/ui.o: $(NKITS)/common/ui.c $(NKITS)/common/ui.h $(NKITS)/version.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/draw.o: $(NKITS)/common/draw.c $(NKITS)/common/draw.h | $(BUILD)
	nspire-gcc $(NATIVE_CFLAGS) $(NATIVE_CPPFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJECTS)
	nspire-ld -Wl,--gc-sections $^ -o $@

$(TARGET).tns: $(BUILD)/$(TARGET).elf
	genzehn --input "$<" --output "$(BUILD)/$(TARGET).zehn" --name EndraTask --240x320-support true --uses-lcd-blit true
	make-prg "$(BUILD)/$(TARGET).zehn" "$@"

$(BUILD)/test_diagnostic: diagnostic.c diagnostic.h test_diagnostic.c | $(BUILD)
	$(CC) -std=c99 -Wall -Wextra -Werror -pedantic diagnostic.c test_diagnostic.c -o $@

host-test: $(BUILD)/test_diagnostic
	$(BUILD)/test_diagnostic

clean:
	rm -rf $(BUILD) $(TARGET).tns
