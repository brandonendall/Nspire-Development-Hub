# EndraGBA — Run008 branding

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
