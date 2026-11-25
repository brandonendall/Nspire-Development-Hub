# Run018 handheld check

Run017 rebooted Brandon's CX II CAS when he pressed Catalog. Keep Run016 as the confirmed rollback. Run018 disables the key inside EndraPython and uses native read-only data for the menu catalog. The immediate cause of the reboot has not been isolated.

After Ndless is working, transfer Run018 as a separate file.

```sh
clear
push /storage/emulated/0/Download/EndraPython_Run018.tns /upload
```

1. Open Run018 and choose Python shell from the Files menu.
2. Open **Menu > Command catalog**. Type `math` and press Enter, then find `sqrt`. The syntax line should show `math.sqrt(x)`.
3. Press Esc to go back to modules and again to return to the shell. First verify this menu path. Stop at any display, input or stability issue and use Run016.
