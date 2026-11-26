# Run026 handheld check

Run025 passed on the CX II CAS and remains the rollback. Run026 adds py3d and Pygame submodule imports.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run026.tns /upload
push /storage/emulated/0/Download/Run026_Py3D_Cube.py.tns /upload
```

Run the script in EndraPython. A shaded cube should rotate full-screen. Try holding Left or Right to change its rotation. Let the demo finish all 72 frames and check for `py3d cube checks passed`, elapsed milliseconds, and free heap. Esc exits the demo or interrupts a running script.
