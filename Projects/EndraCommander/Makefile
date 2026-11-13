NDLESS_SDK ?= $(shell nspire-tools path 2>/dev/null)
ARM_CC ?= arm-none-eabi-gcc
HOST_CC ?= gcc
APP = EndraCommanderNX_Run005
MINIZ = vendor/miniz/miniz.c vendor/miniz/miniz_zip.c vendor/miniz/miniz_tdef.c vendor/miniz/miniz_tinfl.c
CORE = src/nx_fs.c src/nx_zip.c
UI = src/nx_ui.c src/nx_platform.c
INCLUDES = -Isrc -Ivendor -Ivendor/miniz
COMMON = -std=c11 -Wall -Wextra -Wno-misleading-indentation -D_DEFAULT_SOURCE -DMINIZ_NO_TIME $(INCLUDES)
ARM_FLAGS = $(COMMON) -Os -marm -mcpu=arm926ej-s -D_TINSPIRE -fPIE -ffunction-sections -fdata-sections -I$(NDLESS_SDK)/include
OBJECTS = $(patsubst %.c,build/%.o,$(CORE) $(UI) $(MINIZ))
.PHONY: all test preview clean check-sdk
all: dist/$(APP).tns
check-sdk:
	@test -n "$(NDLESS_SDK)" || (echo 'Set NDLESS_SDK to a built Ndless SDK.'; exit 1)
	@test -f "$(NDLESS_SDK)/lib/libndls.a"
build/%.o: %.c src/nx.h src/nx_platform.h | check-sdk
	@mkdir -p $(dir $@)
	$(ARM_CC) $(ARM_FLAGS) -c $< -o $@
build/$(APP).elf: $(OBJECTS)
	$(ARM_CC) -mcpu=arm926ej-s -marm -nostartfiles -nostdlib -Wl,--pic-veneer,--emit-relocs,--gc-sections -Wl,-T,$(NDLESS_SDK)/system/ldscript $(NDLESS_SDK)/system/crt0.o $(NDLESS_SDK)/system/crti.o $(OBJECTS) -L$(NDLESS_SDK)/lib -Wl,--start-group -lndls -lsyscalls -lm -lc -Wl,--end-group -lgcc $(NDLESS_SDK)/system/crtn.o -o $@
dist/$(APP).tns: build/$(APP).elf
	@mkdir -p dist
	$(NDLESS_SDK)/bin/genzehn --input $< --output build/$(APP).zehn --name "EndraCommander NX 003" --ndless-rev-min 2004 --240x320-support true
	$(NDLESS_SDK)/bin/genzehn --info --input build/$(APP).zehn
	cat $(NDLESS_SDK)/tools/zehn_loader/zehn_loader.tns build/$(APP).zehn > $@
build/test_core: tests/test_core.c $(CORE) $(MINIZ) src/nx.h
	@mkdir -p build
	$(HOST_CC) $(COMMON) -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_core.c $(CORE) $(MINIZ) -lm -o $@
test: build/test_core
	ASAN_OPTIONS=detect_leaks=0 ./build/test_core
build/preview: $(CORE) $(UI) $(MINIZ)
	@mkdir -p build
	$(HOST_CC) $(COMMON) -Os $(CORE) $(UI) $(MINIZ) -lm -o $@
preview: build/preview
clean:
	rm -rf build dist
