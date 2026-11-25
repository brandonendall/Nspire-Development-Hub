# Run020 handheld check

Brandon confirmed the Run019 menu catalog on the handheld. Run020 maps the unmodified physical Catalog key to that same built-in catalog action in the Python shell and Python script editor. Run019 is the rollback for this change.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run020.tns /upload
```

1. In Run020, open the Python shell and tap the physical Catalog key once. Expect the Python modules list.
2. Press Esc and confirm the shell returns to the same prompt. Verify the shortcut from a Python script editor after the shell check.
3. If the shortcut causes a freeze or reboot, return to Run019 and use Menu > Command catalog.

Gint input is a separate milestone. It includes `gint.getkey()`, polling, and key state and events.
