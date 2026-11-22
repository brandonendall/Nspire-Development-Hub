# Run005 handheld checks

Brandon confirmed the Run004 shell and punctuation control working. His photo showed the Esc Files label clipped on the shell footer. Keep Run004 as the rollback.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run005.tns /upload
```

1. Open the shell. The bottom bar should show only Menu and Commands & help. Type part of a command, press Menu then Enter on Help, and browse the topics with Left and Right. Press Esc to return to the same input.
2. From Files choose Menu > Samples, select Dice Roller and confirm a new filename. Press Ctrl+R. Expect “Rolling a d20” and a number from 1 to 20. Press Esc to return to the same caret.
3. Open Menu in the editor or output to check its commands. In Files choose Menu > Browse Documents. In Shell choose Menu > List modules or enter help('modules').
4. Roll back by closing Run005 and launching Run004. Preserve the scripts directory.
