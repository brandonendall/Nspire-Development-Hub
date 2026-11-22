# Run002 physical check and rollback

1. Keep the existing EndraPython scripts folder and Run001 executable. Download EndraPython_Run002.tns to the phone and transfer the new executable.
   ```sh
   clear
   push /storage/emulated/0/Download/EndraPython_Run002.tns /upload
   ```
2. Open Run002. Select the existing Hello_World.py.tns in Files. If it is absent, choose + New script and confirm a blank name once to create the sample.
3. Set the second line to print(sum(range(5))). Press Ctrl+R. Expect Hello from EndraPython, followed by 10. Press Esc, make an edit, and press Ctrl+S. Exit and reopen Run002 to confirm the saved edit.
4. If saving fails, keep the edited buffer open and record the exact status. A pre-existing .saving folder can hold the only copy of a recovery file. Leave it in place for inspection.
5. To roll back the executable, close Run002 and launch the retained Run001. Run001 has the reported save defect. Keep the existing scripts directory in either case.
