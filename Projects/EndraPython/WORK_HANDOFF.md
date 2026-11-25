# Run024 handheld check

Run023 passed the Pygame drawing test on the CX II CAS and remains the rollback. Run024 expands Pygame keyboard input and the event queue.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run024.tns /upload
push /storage/emulated/0/Download/Run024_Pygame_Events.py.tns /upload
```

Open the test in EndraPython. Hold Right until the screen prompt changes, then release Right and tap A. The console should print `Pygame event checks passed`. Try Esc at the first prompt on a separate run to verify interruption.
