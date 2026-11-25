# Run015 handheld check

Run014 is the confirmed hardware baseline. Run015 adds compact Pygame image loading and built-in fonts.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run015.tns /upload
push /storage/emulated/0/Download/Run015_Assets_Test.py.tns /upload
push /storage/emulated/0/Download/Run015_Badge.bmp.tns /upload
```

Launch Run015. Browse to upload, highlight Run015_Assets_Test.py.tns, and press Enter. The badge should appear as a diamond without a magenta backdrop, with bitmap text above and below. The demo shows the scene for about two seconds. The output should then read “BMP loaded: (47, 47)” and “Bitmap fonts rendered”.
