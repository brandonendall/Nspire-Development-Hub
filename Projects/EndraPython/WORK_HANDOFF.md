# Run029 handheld check

Run028 is the confirmed fallback. Run `Run029_Py3D_Benchmark.py.tns` in Run028 and Run029 for three passes per build. Each pass runs 24 frames for one cube and 24 frames for four cubes with no intentional pause. Record the prepare, raster and update totals and free heap for each scene. The last line should say `py3d benchmark passed`. Esc interrupts the script.
