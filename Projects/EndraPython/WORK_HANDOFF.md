# Run007 handheld check

Run006 passed Stage 1 on Brandon’s TI-Nspire CX II CAS and is the rollback build.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run007.tns /upload
```

1. In Files choose Menu > Browse Documents and open a project folder. Create `helper` there. Enter `value = 6` and save with Ctrl+S.
2. Create `main` in the same folder. Enter `from helper import value` on line 1 and `print(value)` on line 2. Ctrl+R should print 6.
3. Save `value = 7` in the helper. Run main again and check for 7.
4. Add `1 // 0` on line 2 of the helper and save it. Run main. Check that the traceback names helper.py.tns and line 2. Press Enter to open that line. Remove the test error and save.
