#!/usr/bin/env python3
"""Host-only loading renderer checks and native-size previews, without a ROM.

Compile the actual patched loading C, native text/rectangle routines and font.
Stub only the hardware/key boundary. No calculator execution is simulated.
"""
from pathlib import Path
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
import zlib

root = Path("upstream-gpsp").resolve()
out = Path("validation/run012").resolve()
out.mkdir(parents=True, exist_ok=True)
ui = (root / "nspire_video_ui.c").read_text()

def function(name):
    match = re.search(r"(?m)^void " + name + r"\(", ui)
    assert match, name
    start = ui.index("{", match.start())
    depth = 1
    end = start + 1
    while depth:
        depth += (ui[end] == "{") - (ui[end] == "}")
        end += 1
    return ui[match.start():end]

prefix = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
static u16 frame[320 * 240];
static u32 resolution_width = 320, resolution_height = 240;
static unsigned flips;
#define get_screen_pixels() (frame)
#define get_screen_pitch() ((u32)320)
static u16 *ui_screen(void) { return frame; }
static void flip_screen(void) { flips++; }
static void keys_wait_release(void) {}
static void update_at_vblank(void) {}
static void keys_poll(void) {}
static int any_key_down(void) { return 1; }
void (*nspire_rom_progress)(u32 done, u32 total);
'''
loader = (root / "nspire_loading.c").read_text()
loader = re.sub(r'(?m)^#include "[^"\n]+"\n', '', loader)
test = r'''
static void snapshot(const char *name)
{
  char path[200];
  snprintf(path, sizeof(path), "%s.ppm", name);
  FILE *f = fopen(path, "wb");
  assert(f);
  fputs("P6\n320 240\n255\n", f);
  for (unsigned i = 0; i < 320 * 240; i++) {
    u16 p = frame[i];
    unsigned r = p & 31, g = (p >> 5) & 31, b = (p >> 10) & 31;
    assert(r == g && g == b && !(p & 0x8000));
    assert(p == COLOR_BACK || p == COLOR_TITLE || p == COLOR_TEXT ||
           p == COLOR_BAR || p == COLOR_EMPTY || p == COLOR_ERROR);
    unsigned x = i % 320, y = i / 320;
    if (x < 30 || x >= 318 || y < 80 || y >= 160) assert(p == 0);
    fputc(r * 255 / 31, f); fputc(g * 255 / 31, f); fputc(b * 255 / 31, f);
  }
  assert(fclose(f) == 0);
}
static void progress(unsigned done, unsigned total, unsigned width, const char *name)
{
  assert(nspire_rom_progress);
  unsigned previous_flips = flips;
  nspire_rom_progress(done, total);
  assert(flips == previous_flips + 1);
  for (unsigned y = 110; y < 118; y++)
    for (unsigned x = 0; x < 320; x++) {
      u16 expected = x < 30 || x >= 290 ? COLOR_BACK :
                     x < 30 + width ? COLOR_BAR : COLOR_EMPTY;
      assert(frame[y * 320 + x] == expected);
    }
  snapshot(name);
}
int main(void)
{
  nspire_loading_start("/documents/Games/ShiningSoul2.gbz.tns");
  assert(strcmp(game_name, "ShiningSoul2.gbz") == 0);
  snapshot("startup");
  progress(0, 8, 0, "loading_000");
  progress(2, 8, 65, "loading_025");
  progress(8, 8, 260, "loading_100");
  nspire_loading_note("Loading its state (slot 003)");
  snapshot("loading_state");
  nspire_loading_failed("Couldn't read this game. Is it a GBA ROM?");
  assert(nspire_rom_progress == NULL && bar_total == 0);
  snapshot("loading_failed");
  nspire_game_stopped("Game stopped.\nReturn to the library.");
  snapshot("game_stopped");
  nspire_loading_start("An_extra_long_game_filename_for_the_native_LCD_clipping_test.gbz.tns");
  assert(strlen(game_name) == 51);
  progress(1, 3, 86, "long_name");
  progress(0, 0, 0, "unknown_total");
  nspire_loading_end();
  assert(nspire_rom_progress == NULL);
  assert(flips == 10);
  puts("PASS: actual C loader/text/font; nine frames; bar widths 0/65/260/86/0;");
  puts("PASS: grayscale/bounds; callback start/end/failure; state/stopped/long-name/zero-total");
  return 0;
}
'''
unit = prefix + (root / "font.h").read_text() + "\n"
unit += "\n".join(function(n) for n in ("clear_screen", "fill_rect", "print_string_ext", "print_string"))
unit += "\n" + loader + "\n" + test
with tempfile.TemporaryDirectory(prefix="endragba-run012-") as tmp:
    source = Path(tmp) / "loading_preview.c"
    binary = Path(tmp) / "loading_preview"
    source.write_text(unit)
    subprocess.run([os.environ.get("CC", "cc"), "-std=c99", "-g", "-O1",
                    "-Wall", "-Wextra", "-fsanitize=address,undefined",
                    "-fno-omit-frame-pointer", "-fno-pie", "-no-pie",
                    str(source), "-o", str(binary)], check=True)
    result = subprocess.run([str(binary)], cwd=out, capture_output=True, text=True)
    print(result.stdout, end="")
    print(result.stderr, end="", file=sys.stderr)
    result.check_returncode()

def png(path, width, height, pixels):
    def chunk(kind, data):
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))
    rows = b"".join(b"\0" + pixels[y*width*3:(y+1)*width*3] for y in range(height))
    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)) +
                     chunk(b"IDAT", zlib.compress(rows)) + chunk(b"IEND", b""))

for ppm in sorted(out.glob("*.ppm")):
    data = ppm.read_bytes()
    assert data.startswith(b"P6\n320 240\n255\n")
    pixels = data[len(b"P6\n320 240\n255\n"):]
    assert len(pixels) == 320 * 240 * 3
    png(ppm.with_suffix(".png"), 320, 240, pixels)

report = {
    "kind": "Host render of actual loading C and native drawing/font; NOT calculator screenshots",
    "result": "PASS", "sanitizers": ["AddressSanitizer", "UndefinedBehaviorSanitizer"],
    "frames": sorted(p.name for p in out.glob("*.png")),
    "source_sha256": {n: hashlib.sha256((root / n).read_bytes()).hexdigest()
                      for n in ("nspire_loading.c", "nspire_video_ui.c", "font.h")},
    "physical_test": "Pending owner test on CX II CAS 6.4.0.74 / Ndless",
}
(out / "HOST_RESULTS.json").write_text(json.dumps(report, indent=2) + "\n")
(out / "HOST_RESULTS.txt").write_text(report["kind"] + "\n" + result.stdout + "Physical hardware: PENDING\n")
