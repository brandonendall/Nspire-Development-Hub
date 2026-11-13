NDLESS_SDK ?= $(shell nspire-tools path 2>/dev/null)
ARM_CC ?= arm-none-eabi-gcc
HOST_CC ?= gcc
APP = EndraSDL_Run002
SRC = src/main.c src/safe_paths.c src/preview.c
OBJECTS = $(patsubst %.c,build/%.o,$(SRC))
INCLUDES = -Isrc -I$(NDLESS_SDK)/include
ARM_FLAGS = -std=c11 -Wall -Wextra -Wno-misleading-indentation -Os -marm -mcpu=arm926ej-s -D_TINSPIRE -D__TINSPIRE__ -fPIE -ffunction-sections -fdata-sections $(INCLUDES)
.PHONY: all clean test check-sdk
all: dist/$(APP).tns
check-sdk:
	@test -n "$(NDLESS_SDK)" || (echo 'Provide NDLESS_SDK'; exit 1)
	@test -f "$(NDLESS_SDK)/lib/libSDL.a"
	@test -f "$(NDLESS_SDK)/lib/libndls.a"
build/%.o: %.c src/safe_paths.h src/preview.h | check-sdk
	@mkdir -p $(dir $@)
	$(ARM_CC) $(ARM_FLAGS) -c $< -o $@
build/$(APP).elf: $(OBJECTS)
	$(ARM_CC) -mcpu=arm926ej-s -marm -nostartfiles -nostdlib -Wl,--pic-veneer,--emit-relocs,--gc-sections -Wl,-T,$(NDLESS_SDK)/system/ldscript $(NDLESS_SDK)/system/crt0.o $(NDLESS_SDK)/system/crti.o $(OBJECTS) -L$(NDLESS_SDK)/lib -Wl,--start-group -lSDL -lndls -lsyscalls -lm -lc -Wl,--end-group -lgcc $(NDLESS_SDK)/system/crtn.o -o $@
dist/$(APP).tns: build/$(APP).elf
	@mkdir -p dist
	$(NDLESS_SDK)/bin/genzehn --input $< --output build/$(APP).zehn --name "Endra SDL 002" --ndless-rev-min 2004 --240x320-support true
	cat $(NDLESS_SDK)/tools/zehn_loader/zehn_loader.tns build/$(APP).zehn > $@
build/test_safe_paths: tests/test_safe_paths.c src/safe_paths.c src/safe_paths.h
	@mkdir -p build
	$(HOST_CC) -std=c11 -g -O1 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc tests/test_safe_paths.c src/safe_paths.c -o $@
build/test_preview: tests/test_preview.c src/preview.c src/preview.h src/safe_paths.c src/safe_paths.h
	@mkdir -p build
	$(HOST_CC) -std=c11 -g -O1 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc tests/test_preview.c src/preview.c src/safe_paths.c -o $@
test: build/test_safe_paths build/test_preview
	ASAN_OPTIONS=detect_leaks=0 ./build/test_safe_paths
	ASAN_OPTIONS=detect_leaks=0 ./build/test_preview
clean:
	rm -rf build dist
