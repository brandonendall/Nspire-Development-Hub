# Run008 handheld check

Run007 passed Stage 2 on Brandon’s TI-Nspire CX II CAS, including the jump to the helper’s line 2. Keep Run007 for rollback.

Transfer the application and separate test script.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run008.tns /upload
push /storage/emulated/0/Download/Run008_Storage_Test.py.tns /upload
```

1. Launch Run008. Open Run008_Storage_Test.py.tns from the upload folder and press Ctrl+R. A fresh project prints “Saved runs: 1” and creates Run008_counter.json.tns next to the script.
2. Exit EndraPython completely, reopen it and run the same script. Check for “Saved runs: 2”.
3. To test portability, move the script and its counter together into another folder. Run the script there. The count should advance to 3.

The test writes only its named counter. Repeating it advances the stored count. The executable starts new scripts blank.
