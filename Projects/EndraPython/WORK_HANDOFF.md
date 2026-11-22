# Run004 handheld checks

Brandon reported Run003 could not access the calculator punctuation popup or open scripts stored elsewhere. Keep Run002 as the last owner-confirmed working rollback and preserve existing scripts.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run004.tns /upload
```

1. In the shell, type `print(`, press the physical **?▸** key, pick the double quote with Enter, type a word, insert another quote the same way, then type `)` and press Enter. Repeat in the editor and try square brackets.
2. For TI's own interface press the **Catalog (book)** key. In the TI text field press **?▸** for punctuation or **Ctrl+Catalog** for the wider symbol palette. Choose a symbol and confirm the text field. Check cancel, returning to the same caret, and the LCD after exiting. This OS-dialog interaction requires physical confirmation.
3. In Files, press Menu to browse Documents. Enter opens a folder or Python script, Esc moves up, and Tab cycles pages when a folder has more than 96 entries. Open a plain-text .py or .py.tns from another folder, edit a test copy, run it, exit and reopen the same file. Save writes to that original folder.
4. Roll back by closing Run004 and opening the retained Run002 executable.
