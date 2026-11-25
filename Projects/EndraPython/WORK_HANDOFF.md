# Run009 handheld check

Run008 passed the JSON storage check on Brandon’s TI-Nspire CX II CAS and is the rollback build.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run009.tns /upload
push /storage/emulated/0/Download/Run009_Workbench_Test.py.tns /upload
```

1. Highlight Run009_Workbench_Test.py.tns and press Enter. Expect `Run009 ready`. Esc opens the editor. Return to Files, highlight the script, and press Right to enter the editor directly.
2. In the editor, select Outline from the Menu. Choose the function and check the jump to line 1. Try Go to line with 4. Use Replace all to change `ready` to `edited`, then undo with Ctrl+Z.
3. Open the Python shell from the folder with the test script. Enter `:help`, `:ls`, then `:python Run009_Workbench_Test.py.tns`. The program prints `Run009 ready`.
4. In the catalog, switch to json, type `load`, and check the signature. Tab inserts `import json` at the cursor. Exit without saving if this was a test edit.
