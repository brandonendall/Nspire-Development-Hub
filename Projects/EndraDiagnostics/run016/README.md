# EndraDiagnostics Run016 — storage-size verification and RAM status

The owner wants **actual, practical RAM/storage/CPU/battery diagnostics** and minimal TI-Nspire USB plugging. Run015 was photographed on physical TI-Nspire CX II CAS OS 6.4.0.74, including working CPU, battery and 4 KiB heap tests. Its *document scan* found 50 files and 41 folders, but yielded 0 total file bytes and was PARTIAL. This is **not proof that the documents are empty**.

## Fixes and improvements bundled
1. **Two-method file sizing:** Keep existing read-only `stat()`, but if it returns zero or nonpositive, optionally cross-check with `fopen(path,"rb"); fseek(...,SEEK_END); ftell(); fclose()`. At most 48 such fallback opens per explicitly triggered scan. Neither method writes file contents; neither is guaranteed supported on this virtual filesystem.
2. **Honest size confidence:** Report nonzero measurable file bytes separately from files for which size is unknown. A zero from two interfaces is *not* automatically a verified empty file; show `FILE SIZES: UNAVAILABLE` where appropriate.
3. **More useful bounded directory coverage:** Increase limits from 256 entries/16 folders/depth 3 to 384 entries/48 queued folders/depth 4, scanning only when the user presses Up on Storage. Continue showing `PARTIAL` whenever bounds cut coverage; no background storage scan.
4. **System overview correction:** Display `DOC SIZE: UNAVAILABLE` if none can be measured, rather than misleading `DOCS: 0 KB`. For positive measured sizes use `DOCS KNOWN` as a minimum for enumerated files, not actual free/used disk.
5. **RAM accuracy:** Maintain real ARM/MMU/cache readings and the proven voluntary malloc-owned 4 KiB two-pattern test. **Never infer free system RAM from malloc success.** Current TI OS 6.4.0.74 free/used RAM and storage capacity remain explicitly unverified and unreported.
6. **Safety continuity:** All screen, keypad, battery resolver, timer and **fast manual Esc/Enter exit** code in platform.c remains byte-for-byte identical to Run015. No auto-exit, no filesystem writes, no CPU stress, no unknown memory probes or charger/ADC operations.

## Known limitations
The documents path may be a virtual filesystem with limited `stat()`, `ftell()` and directory semantics. The size fallback is untested on real hardware and could return zero or fail. A correct `FILE SIZES: UNAVAILABLE` is a valid experimental finding. At worst the scan remains partial; don't claim actual total/free disk usage or a percentage. File size checks can still incur filesystem I/O and are **not guaranteed harmless** on undocumented OS paths: keep optional.

## One USB-connection acceptance
Transfer one Run016 `.tns`, unplug USB; verify CPU and battery. On Storage press Up once and photograph the updated storage screen. You can skip optional heap test. The latest user-confirmed stable binary stays available: Run015. User will proactively report any exit failures; no repetitive exit confirmation requests.
