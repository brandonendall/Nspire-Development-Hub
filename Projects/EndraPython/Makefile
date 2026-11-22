MP := micropython_embed
MP_SRC := $(filter-out $(MP)/port/mphalport.c,$(sort $(wildcard $(MP)/py/*.c $(MP)/port/*.c $(MP)/shared/runtime/*.c)))
COMMON := -I. -I$(MP) -I$(MP)/port -Inative -std=gnu99 -Os -DNDEBUG -fno-common
APP := runtime.c editor.c storage.c ui.c
NAT := $(APP) native/platform.c native/text_input.c native/main.c
NOBJ := $(addprefix build/arm/,$(patsubst %.c,%.o,$(NAT) $(MP_SRC)))
HOBJ := $(addprefix build/host/,$(patsubst %.c,%.o,$(APP) $(MP_SRC)))
all: EndraPython_Run002.tns
build/arm/%.o: %.c
	@mkdir -p $(dir $@)
	nspire-gcc $(COMMON) -marm -MMD -MP -c $< -o $@
build/host/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON) -MMD -MP -c $< -o $@
-include $(NOBJ:.o=.d) $(HOBJ:.o=.d)
build/endrapython.elf: $(NOBJ)
	nspire-ld -marm $^ -lm -o $@
EndraPython_Run002.tns: build/endrapython.elf
	genzehn --author "Endra" --name "EndraPython 002" --clickpad-support=false --color-support=true --240x320-support=true --compress --ndless-min 31 --ndless-rev-min 2001 --input $< --output $@
build/check: $(HOBJ) tests/check.c tests/save_folder.c
	$(CC) -I. -std=gnu99 -O2 tests/check.c tests/save_folder.c -Wl,--wrap=mkdir $(HOBJ) -lm -o $@
check: build/check
	./build/check
