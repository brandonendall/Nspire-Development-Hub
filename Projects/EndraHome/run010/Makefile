APP=EndraHome_Run010_NoRefresh
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
CFLAGS=-std=gnu99 -Wall -Wextra -Os -marm -I../../UltimateCommander/vendor -I../gba-ui -I../run007 -I../run005
OBJS=build/main.o build/render.o build/caption.o build/overlay.o build/registry.o build/ui.o build/logic7.o build/platform.o
.PHONY: all test preview clean
all: $(APP).tns
build:
	mkdir -p build
build/main.o: main.c registry.h render.h ../run007/ui.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/render.o: render.c caption.h registry.h render.h ../../UltimateCommander/vendor/font8x8_basic.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/caption.o: caption.c caption.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/overlay.o: ../run007/overlay.c ../run007/overlay.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/registry.o: registry.c registry.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/ui.o: ../run007/ui.c ../run007/ui.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/logic7.o: ../run005/logic7.c ../run005/logic7.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/platform.o: platform.c ../gba-ui/platform.h | build
	$(CC) $(CFLAGS) -c $< -o $@
build/$(APP).elf: $(OBJS)
	$(LD) -marm $^ -o $@ -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --version 12 --author "EndraHome" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --name "EndraHome Fast 009" --input $< --output $@
build/test_caption: caption.c caption.h test_caption.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror caption.c test_caption.c -o $@
build/test_registry: registry.c ../run007/ui.c ../run005/logic7.c test.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../run007 -I../run005 -I../gba-ui registry.c ../run007/ui.c ../run005/logic7.c test.c -o $@
test: build/test_caption build/test_registry
	./build/test_caption
	./build/test_registry
build/preview: render.c caption.c ../run007/overlay.c registry.c ../run007/ui.c ../run005/logic7.c preview.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I../../UltimateCommander/vendor -I../gba-ui -I../run007 -I../run005 render.c caption.c ../run007/overlay.c registry.c ../run007/ui.c ../run005/logic7.c preview.c -o $@
preview: build/preview
	./build/preview
clean:
	rm -rf build $(APP).tns
