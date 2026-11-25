# Run017 handheld check

Transfer the standalone executable from Android.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run017.tns /upload
```

1. Open the Python shell. Press the physical Catalog key, type `math`, and press Enter. Select `sqrt` and check that the syntax says `math.sqrt(x)`. Back out, execute `import math`, then use the Catalog to insert `math.sqrt(`. Type `9)` and press Enter. Expect `3.0`.
2. Check that Ctrl+Catalog still opens TI symbols. Esc returns without inserting text.
3. Open a Python script. Press Menu and type `settings`. Change Indent guides or Line numbers and observe the editor. Return settings to On and check that Enter after a block header adds two spaces.
4. In the shell, `import casioplot` should report ImportError. `import display, gint, pygame` should succeed.

Run016 is the owner-confirmed fallback. Existing script files can be reused.
