# EndraGBA — Run012 Loading NightMode

Current branch: `feature/endragba-run012-loading-nightmode`.
Output: **EndraGBA_Run012_LoadingNightMode.tns**.
Read [project memory](PROJECT_MEMORY.md) and [Run012 build/test notes](RUN012_NOTES.md)
for verified build identity and current hardware status.

Run012 extends the approved Run011 black/gray menus to the separate loading,
status and failure renderer. Progress, GBZ, saves, RTC, browser and FastExit
behavior are preserved. Build with **EndraGBA Run012 - Loading NightMode**;
keep Run011 alongside it as rollback. Native-size previews are host renders;
Run012 calculator testing remains pending.

Transfer from Android Downloads:
```sh
clear
push /storage/emulated/0/Download/EndraGBA_Run012_LoadingNightMode.tns /upload
```

## Historical Run008 branding and association notes

EndraGBA is the new name for UltraGBA. Run008 layers the new menu title,
exit label and executable metadata onto the preserved Run007 patch.

Output: `EndraGBA_Run008_Branding.tns`. Build with the **EndraGBA Run008**
GitHub Actions workflow, or copy `vendor/upstream-gpsp` to `upstream-gpsp`,
run `python3 Projects/EndraGBA/run008_branding.py`, then compile with Ndless.

GBZ loading, ROM-local saves, RTC persistence and display support retain the
Run007 implementation. Keep the executable in the same folder as the old
emulator so it finds `UltraGBA_Data` and `UltraGBA_Tests`; those compatibility
folder names remain unchanged. Historical patch scripts live in `../UltraGBA`.

## Opening games directly

The compatibility-mode warning and Documents-browser launching are separate.
The build declares `--240x320-support=true`, which is display metadata.
Run007 removed all automatic Ndless association writes after Run006 did not
fix native Documents launching. This build retains that behavior.

For a file named exactly `EndraGBA_Run008_Branding.tns`, Ndless's game
associations would need these values in the existing `ndless/ndless.cfg.tns`:

```ini
ext.gba=EndraGBA_Run008_Branding
ext.gbz=EndraGBA_Run008_Branding
```

These are two entries to add or update, not a replacement configuration.
Retain existing image-viewer and other mappings. A different executable
filename needs matching values without the final `.tns` extension.
An existing matching entry alone does not prove native dispatch works on the
device. The current calculator configuration/error needs inspection before
calling the launch issue fixed. Opening the emulator and selecting the ROM
inside it remains the working route reported by the owner.

This is a branding build, not a claim that native ROM dispatch is repaired.
