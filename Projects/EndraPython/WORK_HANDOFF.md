# Physical acceptance and rollback

1. With the verified Run001 binary downloaded to the phone, transfer it using the owner's wrapper.
   ```sh
   clear
   push /storage/emulated/0/Download/EndraPython_Run001.tns /upload
   ```
2. Launch once, then press Esc from Files. Check that TI Documents returns promptly and the LCD restores correctly. Stop on a freeze or damaged display.
3. Relaunch, choose + New script, leave its name blank and press Enter. Run with Ctrl+R or Menu > Save and run. Look for Hello from EndraPython and 45. Esc should restore the caret.
4. Edit, save and reopen the example. Make a second named script with an invalid second line (if :). Run it and check the traceback reports line 2. Esc returns to the editor. Keep all test writes in the dedicated scripts folder.
5. In Python shell try x = 7 followed by sum(range(x)). Check 21. Try a script with a simple infinite Python loop and hold Esc. Measure launch/exit and repeated runs on hardware only after the basic tests pass.
6. First release rollback: exit the application and remove only EndraPython_Run001.tns through Documents. Keep /documents/EndraPython/ for all user scripts. No changes to hooks, configuration or firmware are required.

Next development milestone is editor undo/redo, selection and clipboard after physical input, file replacement, LCD restoration and runtime memory behavior are confirmed. Record device results separately from rendered previews.
