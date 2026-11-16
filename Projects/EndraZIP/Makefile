# EndraZIP 007 - tested recursive directory ZIP packaging.
GCC = nspire-gcc
LD = nspire-ld
GENZEHN = genzehn
CFLAGS = -Wall -Wextra -Os -marm -std=c99 -Ivendor/miniz
LDFLAGS = -Wl,--nspireio,--gc-sections
MINIZ = vendor/miniz/miniz.o vendor/miniz/miniz_zip.o vendor/miniz/miniz_tinfl.o vendor/miniz/miniz_tdef.o
OBJS = src/endrazip.o src/ndless_compat.o $(MINIZ)

all: EndraZIP_007.tns

src/ndless_compat.o: src/ndless_compat.c
	$(GCC) $(CFLAGS) -c $< -o $@

src/endrazip.o: src/endrazip.c
	$(GCC) $(CFLAGS) -c $< -o $@

vendor/miniz/%.o: vendor/miniz/%.c
	$(GCC) $(CFLAGS) -c $< -o $@

EndraZIP_007.elf: $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

EndraZIP_007.tns: EndraZIP_007.elf
	$(GENZEHN) --input $< --output $@ --name "EndraZIP 007"

clean:
	rm -f $(OBJS) EndraZIP_007.elf EndraZIP_007.tns

test:
	python3 tests/test_host.py

.PHONY: all clean test

