MP := micropython_embed
MP_SRC := $(filter-out $(MP)/port/mphalport.c,$(sort $(wildcard $(MP)/py/*.c $(MP)/extmod/*.c $(MP)/port/*.c $(MP)/shared/runtime/*.c)))
COMMON := -I. -I$(MP) -I$(MP)/port -Inative -std=gnu99 -Os -DNDEBUG -fno-common
APP := catalog.c project_io.c utf.c runtime.c editor.c storage.c ui.c render.c
NAT := $(APP) native/platform.c native/text_input.c native/main.c native/ti_input.c
NOBJ := build/arm/font.o $(addprefix build/arm/,$(patsubst %.c,%.o,$(NAT) $(MP_SRC)))
HOBJ := build/host/font.o $(addprefix build/host/,$(patsubst %.c,%.o,$(APP) $(MP_SRC)))
all: EndraPython_Run008.tns
build/arm/%.o: %.c
	@mkdir -p $(dir $@)
	nspire-gcc $(COMMON) -marm -MMD -MP -c $< -o $@
build/host/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON) -MMD -MP -c $< -o $@
build/arm/font.o: assets/font.S assets/font.bin
	@mkdir -p $(dir $@)
	nspire-gcc -marm -c $< -o $@
build/host/font.o: assets/font.S assets/font.bin
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@
-include $(NOBJ:.o=.d) $(HOBJ:.o=.d)
build/endrapython.elf: $(NOBJ)
	nspire-ld -marm $^ -lm -o $@
EndraPython_Run008.tns: build/endrapython.elf
	genzehn --author "Endra" --name "EndraPython 008" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --input $< --output $@
build/check: $(HOBJ) tests/check.c tests/save_folder.c
	$(CC) -I. -std=gnu99 -O2 tests/check.c tests/save_folder.c -Wl,--wrap=mkdir $(HOBJ) -lm -o $@
check: build/check
	./build/check
	python3 tests/keyboard.py
