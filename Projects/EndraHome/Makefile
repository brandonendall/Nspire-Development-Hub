APP=EndraHome_Run001
CC=nspire-gcc
LD=nspire-ld
GENZEHN=genzehn
HOST_CC?=cc
.PHONY: all clean test
all: $(APP).tns
build:
	mkdir -p build
build/home.o: src/home.c src/shortcuts.h | build
	$(CC) -std=gnu99 -Wall -Wextra -Os -marm -D__TINSPIRE__ -c $< -o $@
build/shortcuts.o: src/shortcuts.c src/shortcuts.h | build
	$(CC) -std=gnu99 -Wall -Wextra -Os -marm -c $< -o $@
build/$(APP).elf: build/home.o build/shortcuts.o
	$(LD) -marm $^ -o $@ -lSDL -lm
$(APP).tns: build/$(APP).elf
	$(GENZEHN) --input $< --output build/$(APP).zehn --name "EndraHome 001" --ndless-rev-min 2004
	make-prg build/$(APP).zehn $@
build/test_shortcuts: src/shortcuts.c src/shortcuts.h tests/test_shortcuts.c | build
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -g -I src src/shortcuts.c tests/test_shortcuts.c -o $@
test: build/test_shortcuts
	./build/test_shortcuts
clean:
	rm -rf build $(APP).tns
