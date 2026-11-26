# Run027 handheld check

Keep Run025 as the confirmed fallback. Run026 is the original 3D build. Run027 optimizes triangle preparation while keeping the same scene and controls.

Run `Run027_Py3D_Profile.py.tns` in both Run026 and Run027. Allow all 72 frames to finish without holding keys for a comparable measurement. Record `Render and present`, total elapsed milliseconds, and free heap. The test should print `py3d cube checks passed` in both builds.
