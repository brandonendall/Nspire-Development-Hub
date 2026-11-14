EXE = EndraDiagnostics_Run001
GCC = nspire-gcc
LD = nspire-ld
GENZEHN = genzehn

all: $(EXE).tns

main.o: main.c
	$(GCC) -Wall -Wextra -Os -marm -c $< -o $@

$(EXE).elf: main.o
	$(LD) $^ -o $@

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $< --output $@ --name "EndraDiagnostics Run001"

clean:
	rm -f main.o $(EXE).elf $(EXE).tns

.PHONY: all clean
