# Run023 handheld check

Run022 passed the Gint key test on the CX II CAS and remains the rollback. Run023 adds Pygame drawing and transforms.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run023.tns /upload
push /storage/emulated/0/Download/Run023_Pygame_Draw.py.tns /upload
```

Open the test in EndraPython. It should show full-screen shapes and four versions of a sprite for about five seconds, then print `Pygame drawing checks passed`. The red circle is hollow, the far-right outline clips at the screen edge, and all four sprites have transparent backgrounds.
