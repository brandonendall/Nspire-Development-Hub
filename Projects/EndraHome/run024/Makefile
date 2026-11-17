APP=EndraHome_Run024_FullScreen
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../gba-ui -I../run005
OBJS=build/main.o build/render.o build/caption.o build/overlay.o build/registry.o build/ui.o build/logic7.o build/platform.o build/session.o build/picker.o build/picker_render.o build/config_save.o build/edit.o build/manager.o build/text_input.o build/files.o build/files_render.o build/image_viewer.o build/image_keys.o
.PHONY: all test preview clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c registry.h render.h session.h ui.h overlay.h picker.h config_save.h edit.h manager.h text_input.h files.h image_viewer.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/render.o: render.c caption.h registry.h render.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/caption.o: caption.c caption.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/overlay.o: overlay.c overlay.h ui.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/registry.o: registry.c registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/ui.o: ui.c ui.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic7.o: ../run005/logic7.c ../run005/logic7.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: platform.c ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/session.o: session.c session.h ui.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/picker.o: picker.c picker.h registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/picker_render.o: picker_render.c picker.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/config_save.o: config_save.c config_save.h registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/edit.o: edit.c edit.h config_save.h registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/manager.o: manager.c manager.h ui.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/text_input.o: text_input.c text_input.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/files.o: files.c files.h picker.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/files_render.o: files_render.c files.h picker.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/image_viewer.o: image_viewer.c image_viewer.h ../../UltimateCommander/vendor/stb_image.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/image_keys.o: image_keys.c image_viewer.h | build
	$(CC) $(CFLAGS) -c $< -o $@

build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome FullScreen 024" --input $< --output $@
build/test_caption: caption.c caption.h test_caption.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror caption.c test_caption.c -o $@
build/test_registry: registry.c ui.c ../run005/logic7.c test.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../run005 -I../gba-ui registry.c ui.c ../run005/logic7.c test.c -o $@
build/test_return: session.c registry.c ui.c ../run005/logic7.c test_return.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../run005 -I../gba-ui session.c registry.c ui.c ../run005/logic7.c test_return.c -o $@
build/test_picker: picker.c config_save.c registry.c test_picker.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../gba-ui picker.c config_save.c edit.c registry.c test_picker.c -o $@
build/test_files: files.c image_viewer.c picker.c registry.c test_files.c | build
	$(HOST_CC) -std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-function -I../../UltimateCommander/vendor -I../gba-ui files.c image_viewer.c picker.c registry.c test_files.c -lm -o $@
build/test_image: image_viewer.c image_viewer.h test_image.c ../../UltimateCommander/vendor/stb_image.h | build
	$(HOST_CC) -std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-function -I../../UltimateCommander/vendor -I../gba-ui image_viewer.c test_image.c -lm -o $@
test: build/test_caption build/test_registry build/test_return build/test_picker build/test_files build/test_image
	./build/test_caption
	./build/test_registry
	./build/test_return
	./build/test_picker
	./build/test_files
	./build/test_image
build/preview: render.c caption.c overlay.c picker_render.c registry.c ui.c ../run005/logic7.c preview.c | build
	$(HOST_CC) -std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-function -I../../UltimateCommander/vendor -I../gba-ui -I../run005 render.c caption.c overlay.c picker_render.c manager.c files_render.c files.c image_viewer.c picker.c registry.c ui.c ../run005/logic7.c preview.c -lm -o $@
preview: build/preview
	./build/preview
clean:
	rm -rf build $(APP).tns
