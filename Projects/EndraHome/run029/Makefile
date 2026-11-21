APP := EndraHome_Run041_Desktop
CC := nspire-gcc
LD := nspire-ld
GENZEHN := genzehn
HOST_CC ?= cc
INCLUDES := -I../../UltimateCommander/vendor -I../gba-ui -I../run005
NATIVE_FLAGS := -std=gnu99 -Wall -Wextra -Os -marm $(INCLUDES)
HOST_FLAGS := -std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-function $(INCLUDES)
DESKTOP := ui.c registry.c preferences.c app_target.c desktop.c toast.c ../run005/logic7.c
BROWSER := files.c file_ops.c image_viewer.c picker.c registry.c preferences.c app_target.c toast.c
DRAW := art.c art_data.S render.c display.c caption.c overlay.c manager.c picker_render.c files_render.c file_icons.c
NATIVE := main platform app_target art art_data render display caption overlay registry ui desktop toast preferences \
          session picker picker_render config_save edit manager text_input files file_ops file_icons \
          files_render image_viewer image_keys
OBJECTS := $(addprefix build/,$(addsuffix .o,$(NATIVE))) build/logic7.o
TESTS := caption registry return picker files image ops preferences display
.PHONY: all test preview clean
all: $(APP).tns

build:
	mkdir -p $@
build/%.o: %.c | build
	$(CC) $(NATIVE_FLAGS) -MMD -MP -c $< -o $@
build/art_data.o: art_data.S run038_art.bin | build
	$(CC) -marm -c $< -o $@
build/logic7.o: ../run005/logic7.c | build
	$(CC) $(NATIVE_FLAGS) -MMD -MP -c $< -o $@
-include $(OBJECTS:.o=.d)
build/$(APP).elf: $(OBJECTS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 31 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome Desktop 041" --input $< --output $@

HEADERS := $(wildcard *.h) ../run005/logic7.h ../gba-ui/logic.h
build/test_caption: test_caption.c caption.c
build/test_registry: test.c $(DESKTOP)
build/test_return: test_return.c session.c $(DESKTOP)
build/test_picker: test_picker.c picker.c config_save.c edit.c session.c $(DESKTOP)
build/test_files: test_files.c $(BROWSER)
build/test_image: test_image.c image_viewer.c
build/test_ops: test_ops.c file_ops.c
build/test_preferences: test_preferences.c config_save.c registry.c preferences.c app_target.c
build/test_display: test_display.c $(DRAW) $(BROWSER) ui.c desktop.c ../run005/logic7.c
build/preview: preview.c $(DRAW) $(BROWSER) ui.c desktop.c ../run005/logic7.c
build/test_files build/test_image build/test_display build/preview: ../../UltimateCommander/vendor/stb_image.h
build/test_display build/preview: ../../UltimateCommander/vendor/font8x8_basic.h run038_art.bin
$(addprefix build/test_,$(TESTS)) build/preview: $(HEADERS) | build
	$(HOST_CC) $(HOST_FLAGS) -D_POSIX_C_SOURCE=200809L $(filter %.c %.S,$^) -lm -o $@

test: $(addprefix build/test_,$(TESTS))
	@set -e; for name in $(TESTS); do ./build/test_$$name; done
preview: build/preview
	./build/preview
clean:
	rm -rf build $(APP).tns
