# Run006 handheld checks

Transfer the standalone executable. Run005 installation was unconfirmed. Run004 is the owner-confirmed rollback.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run006.tns /upload
```

1. From Files select New script and give it a fresh name. Check the blank buffer. Tab indents to the next two-space stop. Enter repeats the current line's leading spaces. Opening an older script preserves its existing indentation.
2. In the editor or shell open Menu > Command catalog. Choose input or print. Use Left/Right for builtins or compiled modules. Type a prefix and press Enter to insert. Check both editor and shell insertion.
3. In the shell run input("Name? "), enter a reply and press Enter. Repeat with an empty reply and then try Esc. Confirm a subsequent command evaluates. In a saved script, exercise assignment from input and printing the result with Ctrl+R.
4. The shipped executable and ZIP omit sample programs. Scripts already saved on-device remain in their folders. Roll back by launching the retained Run004.
