# Run022 handheld check

Run021 passed the Gint drawing check on the CX II CAS and is the rollback. Run022 adds sampled Gint input while keeping Esc as the script interrupt.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run022.tns /upload
push /storage/emulated/0/Download/Run022_Gint_Keys.py.tns /upload
```

Open the test in EndraPython. The prompt uses the full screen with an edge-to-edge background. Tap Right and release it, then tap A when prompted. Leave the keypad untouched for the short timeout check. The console should display `Gint key checks passed`. On a second run, press Esc at the first wait to check that the script returns with a KeyboardInterrupt.
