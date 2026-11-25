# Run011 handheld check

Run009 is the confirmed hardware rollback. Run010 and Run011 hardware checks can be done independently.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run011.tns /upload
push /storage/emulated/0/Download/Run011_Graphics_Test.py.tns /upload
```

1. Launch Run011, select Run011_Graphics_Test.py.tns in Files, and press Enter. A green square appears with an instruction banner.
2. Hold an arrow to move the square. Release it and check that movement stops.
3. Press Esc. EndraPython should show the number of rendered frames and elapsed milliseconds. Press Esc again to open the editor.
4. Press Ctrl+R to rerun and confirm the square starts at the center. The test script does not save data.
