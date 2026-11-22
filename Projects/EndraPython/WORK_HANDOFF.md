# Run003 handheld checks

Brandon confirmed Run002 successful. Keep that executable as the rollback and keep the existing scripts folder. Run003 uses the same directory and save format.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run003.tns /upload
```

1. Open Run003. Press Down to highlight a saved file. Open Hello_World, make a small edit and press Ctrl+S. Exit, reopen and confirm the edit.
2. Select a word with Shift+Right. Copy, paste, undo and redo using the editor Menu or the shortcuts in README.md. Use Ctrl+F twice on repeated text to check wraparound.
3. Run a script with Ctrl+R. To verify error navigation, use a new script containing a print on line one and `1 // 0` on line two. From the traceback press Enter and check the caret on line two.
4. In Files, press Right then Enter for Python shell. Press Menu to list modules. Try `import random` then `random.randint(1, 6)`. Check text clarity, selection visibility and keys on the actual LCD.
5. Exit and reopen Documents. If a save reports an error, keep the buffer open and record its exact status. Preserve existing scripts and any .saving recovery folder. To roll back, close Run003 and launch Run002.
