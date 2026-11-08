# Security Test plugin

Build with `make -f Makefile`, then load `Add_Dynamic_Module Security Test`. Run `Appeal.exe Test/Security` from the Appeal root. Every export echoes its call, but plaintext, keys, and passwords are redacted. Dummy results are clearly marked TEST; files are never changed.

Copy this folder to a sibling, rename it, and replace each function in `src/main.cpp`. The Makefile derives its output name from the folder. Keep the parent module's `src/Security_Plugin_API.h` available. Match Appeal's compiler/STL ABI. Constructor/Destructor initialize and release library state; Last_Error must clear on success and contain a non-secret diagnostic on failure. Empty decrypted text is legitimate. Use a maintained cryptography library: this template itself is not encryption.
