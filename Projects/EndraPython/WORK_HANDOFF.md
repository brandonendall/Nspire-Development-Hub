# Run026–028 handheld checks

Keep Run025 as the confirmed fallback. Run026, Run027 and Run028 are separate test builds.

- Run026: run `Run026_Py3D_Cube.py.tns` for baseline shaded cube rendering and steering.
- Run027: run `Run027_Py3D_Profile.py.tns`. Run the same profile in Run026 without holding keys to compare render-and-present milliseconds and free heap.
- Run028: run `Run028_Py3D_Clipping.py.tns`. A yellow triangle should retain its visible portion as it crosses the near plane and finish with `py3d clipping checks passed`.
