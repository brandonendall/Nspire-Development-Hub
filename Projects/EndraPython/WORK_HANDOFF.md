# Run013 handheld check

Run011 froze the calculator after displaying its initial graphics. Run009 is the confirmed hardware rollback.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run013.tns /upload
push /storage/emulated/0/Download/Run013_Numerics_Test.py.tns /upload
push /storage/emulated/0/Download/Run012_Frame_Probe.py.tns /upload
push /storage/emulated/0/Download/Run012_Graphics_Test.py.tns /upload
```

1. Start Run013 and run Run013_Numerics_Test.py.tns. Check for “Decimal checks passed” and “Vector and matrix checks passed”.
2. To test graphics, select Run012_Frame_Probe.py.tns, and press Enter. It draws once, pauses briefly, and returns to output with “Frame returned” and “Delay returned”. This isolates display and timing from held-key scanning.
3. If the probe completes, run Run012_Graphics_Test.py.tns. Try an arrow and release it. Press Esc to finish early or let the script return at 120 frames.
4. Report the last visible screen if either test stalls. Both probes leave your documents alone.
