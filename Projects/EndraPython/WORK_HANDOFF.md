# Run019 handheld check

The regular Menu uses arrows and Enter. The separate Python Command Catalog keeps typing shortcuts. The physical Catalog shortcut remains disabled after the Run017 reboot. Run016 is the confirmed fallback.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run019.tns /upload
```

1. On Files, press Menu. Check that New Python script, New text file, Python shell, Browse Documents, Settings, and Help appear as a plain list. Typing `print` should leave the selection in place.
2. Use the arrows to reach Settings, then press Enter and check that arrow navigation and Enter still work. Press Esc to return.
3. From Python shell, use Menu > Command catalog. Type `math`, open it, and type `sqrt`. Verify the `math.sqrt(x)` hint and press Esc twice to return to the shell.
