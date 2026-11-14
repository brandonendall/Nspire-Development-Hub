/* EndraDiagnostics Run001: conservative Ndless diagnostic. No direct MMIO. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

static volatile uint32_t sink;
static void cpu_reference(void) {
    puts("CPU REFERENCE (previous physical CPUID result)");
    puts("ARM implementer 0x41; ARMv5TEJ / ARM926");
    puts("Part 0x926, variant 0, revision 5");
    puts("L1 Data 8 KiB; L1 Instruction 16 KiB");
    puts("4-way associative, 32-byte cache lines");
    puts("These are NOT live CPU measurements.");
}
static void memory_check(void) {
    const size_t n = 256u * 1024u;
    unsigned char *buf = (unsigned char *)malloc(n);
    size_t i;
    if (!buf) { puts("Allocation failed (256 KiB)."); return; }
    for (i = 0; i < n; ++i) buf[i] = (unsigned char)(i ^ (i >> 8));
    for (i = 0; i < n; ++i) {
        if (buf[i] != (unsigned char)(i ^ (i >> 8))) {
            printf("Memory mismatch at %lu\n", (unsigned long)i);
            free(buf); return;
        }
    }
    free(buf);
    puts("256 KiB allocation/readback: PASS");
    puts("Not a full RAM test/capacity measurement.");
}
static void cpu_benchmark(void) {
    uint32_t v = 0x12345678u;
    unsigned long i;
    clock_t start = clock();
    for (i = 0; i < 2000000ul; ++i) {
        v = (v * 1664525u) + 1013904223u;
        v ^= v >> 13;
    }
    sink = v;
    if (start != (clock_t)-1) {
        clock_t end = clock();
        if (end != (clock_t)-1 && end >= start && CLOCKS_PER_SEC > 0) {
            printf("2M loop iterations: %lu ms (clock)\n",
                (unsigned long)(((double)(end - start) * 1000.0) / CLOCKS_PER_SEC));
            puts("Not a CPU MHz measurement.");
            return;
        }
    }
    puts("Loop executed; clock timer unavailable.");
}
int main(void) {
    int ch;
    puts("==============================");
    puts("ENDRA HARDWARE DIAGNOSTICS 001");
    puts("Experimental safe-mode build");
    puts("==============================");
    puts("Target: TI-Nspire CX II CAS");
    puts("OS 6.4.0.74: target, NOT detected");
    for (;;) {
        puts("\n1 CPU reference   2 RAM check");
        puts("3 CPU loop test   0 Exit");
        puts("Enter selection then Enter:");
        ch = getchar();
        if (ch == EOF || ch == '0') break;
        if (ch == '1') cpu_reference();
        else if (ch == '2') memory_check();
        else if (ch == '3') cpu_benchmark();
        while (ch != '\n' && ch != EOF) ch = getchar();
    }
    puts("EndraDiagnostics exiting.");
    return 0;
}
