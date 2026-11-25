# Run021 handheld check

Run020 is the confirmed rollback. Run021 adds Gint drawing and removes the redundant Catalog hint from the shell startup screen.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run021.tns /upload
push /storage/emulated/0/Download/Run021_Gint_Drawing.py.tns /upload
```

Place the Python test in your EndraPython scripts folder or browse to it in EndraPython. Run the test. A colored line and shapes should appear for about 4.5 seconds, then the terminal should print `Gint drawing checks passed`. The magenta ellipse should be hollow. The far-right green ellipse is intentionally clipped. In the shell, the physical Catalog key and the menu entry still open the browser. The startup hint is removed.

Gint key waits and events are planned separately.
