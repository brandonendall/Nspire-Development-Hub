# EndraDiagnostics Run014 — expanded hardware inspection (14 pages)

Run013 has been physically photographed showing the LCD color check, keypad counters and valid 100% battery summary. The owner confirmed it exited properly and expressly instructed: **do not repeatedly ask for confirmation when exit works; they will report any failure**. Preserve Run013 as verified hardware baseline and avoid unnecessary USB transfers.

## Multiple changes batched for one physical test
- Run013's **11 existing pages**, same battery resolver, processor identification, cache information, MMU, optional 4 KiB heap integrity check, OS signature details, trend, safety display, LCD RGB check, keypad event counts and battery summary.
- Page 12: **16-step grayscale LCD visual test**. Colors are drawn into the already working software framebuffer. No display timings, clocks or hardware display modes changed.
- Page 13: **CPU status flag interpretation** (MMU, instruction/data cache, alignment fault-check and write-buffer flags), decoded only from the same already-read CP15 control snapshot. No new CP15 instructions, no additional MMIO reads.
- Page 14: **keypad activity bar chart** derived from existing app-level directional/Menu counts. No additional hardware input scanning; key behavior/navigation unchanged.
- Existing battery trend screen and battery summary now **redraw while sampling**, correcting stale on-screen data during long sessions without increasing the read frequency.

## Physical hardware safety
Original Run013 platform.c copied BYTE FOR BYTE, preserving known-fast manual Esc/Enter exit, original timer/LCD restoration, and no auto exit. Existing Run004 OS metadata reader and 4 KiB optional heap test are unchanged. New code only draws pixels and computes statistics from already available data; no new battery/ADC/charger reads or writes, no CPU stress, large RAM tests, flash/OS data writes, clock, USB or hook modifications. The preexisting battery resolver is still an experimental OS memory reader, so native freezes cannot be excluded.

## One-connection test plan
Transfer the single Run014 .tns once, disconnect USB, navigate through all 14 pages, compare known readings to Run013, inspect grayscale, CPU status flags and keypad bars, then exit normally when finished. **No need to report normal exit unless it fails.** Keep Run013 available as rollback.
