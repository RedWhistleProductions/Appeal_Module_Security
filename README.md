# Appeal Security

Encryption/decryption behind a stable module API, with room for future permissions and security services. See the [complete Basic/Advanced API and globals](Docs/Security.html).

Projects: `Appeal_Module_Security` (bridge), `Test` (redacted echo/template), and `Appeal_Sodium_Security` (portable working libsodium backend).

## Use

```text
Run Setup/Config
Set_Var Key (Security Generate_Key)
Set_Var Cipher (Security Encrypt "A private message" Var Key)
Set_Var Plaintext (Security Decrypt Var Cipher Var Key)
Print_Global Security.Success
```

Keep the generated key securely if the data must be decrypted later. Keys and plaintext are returned directly, never automatically logged, published as globals, or saved by Security. Caller-owned script variables remain ordinary strings. Examples use demonstration data; applications obtain real credentials through their own user/configuration flow.

Password mode:

```text
Set_Var Cipher (Security Encrypt_With_Password "Message" "user supplied password")
Set_Var Plaintext (Security Decrypt_With_Password Var Cipher "user supplied password")
```

Wrong credentials, changed ciphertext, and malformed data fail authentication. Empty plaintext can be valid, so distinguish it from failure using `Security.Success` (1/0) and `Security Last_Error`. Success clears the error; status getters do not. `Security Backend_Version` reports the actual backend/library version.

## Files

```text
Security Encrypt_File Storage/Input.bin Security/Input.enc Var Key
Security Decrypt_File Security/Input.enc Security/Restored.bin Var Key
```

Relative paths are under Resources; absolute paths are accepted. Destination folders must exist, and existing destinations are never overwritten. The original source stays intact. `Encrypt_File_With_Password` / `Decrypt_File_With_Password` are equivalent password variants.

Files use bounded 64 KiB buffers and authenticated secretstream records. A mandatory final marker and exact EOF reject truncation, reordered records, and appended data. Output is staged privately and published only after success; failure removes the temporary output. Windows uses a protected owner-only DACL and no-replace rename. POSIX uses mode 0600 and atomic hard-link publication, requiring hard-link support on the destination filesystem.

## Build

Windows x64:

```text
python Resources/Modules/Appeal_Module_Security/Packaging/Install_Libsodium.py
make -C Resources/Modules/Appeal_Module_Security -f Makefile
make -C Resources/Modules/Appeal_Module_Security/Test -f Makefile
make -C Resources/Modules/Appeal_Module_Security/Appeal_Sodium_Security -f Makefile
```

The installer pins libsodium 1.0.22, verifies its upstream Minisign signature with a pinned published Ed25519 key, checks GitHub's asset digest when supplied, and preserves matching source, signatures, hashes, and licenses. A local Python cryptography package verifies downloads only. Windows links libsodium statically; no additional libsodium DLL is needed at runtime.

On Linux/macOS, install a maintained libsodium development package and pkg-config, then use the same Makefiles. Minimum supported library: 1.0.18. To use the signed current source retained in `.deps/source/libsodium-1.0.22`, configure with `--disable-shared --enable-static --with-pic --prefix=<absolute SDK prefix>`, then run `make -j4`, `make check -j4`, and `make install`. Build the plugin with `make -B -f Makefile SODIUM_ROOT=<SDK prefix>`.

Windows and Ubuntu have been built and runtime-tested, including bidirectional ciphertext/file exchange. The local Ubuntu plugin uses the matching libsodium 1.0.22 source, whose upstream tests also passed. Makefiles include a macOS dylib target and portable POSIX file operations, but macOS has not been executed here. Mobile hosts still need normal Appeal/NDK/iOS integration; this change does not port the entire interpreter to those systems.

## Test plugin and plugin authors

Run `Appeal.exe Test/Security`. Test needs neither libsodium nor real credentials. It echoes every backend export, redacts sensitive arguments, returns clearly marked TEST data, and never changes files. It is not encryption.

Copy Test into a sibling directory and rename it. Its Makefile derives the DLL/SO/dylib name from the folder. Implement each function in `src/main.cpp` using the [SDK](src/Security_Plugin_API.h), keeping initialization, cleanup, and status behavior. Compile host/plugin with matching C++ compiler/STL ABIs, following existing Appeal conventions. Successful empty plaintext must leave Last_Error empty. Use a maintained cryptography library rather than replacing algorithms with custom code.

## Version-1 interoperable formats

Text is `APSEC1:` plus URL-safe Base64 without padding. Decoded bytes:

| Field | Bytes |
|---|---:|
| Magic `APSTXT01` | 8 |
| Mode `K` (key) or `P` (password) | 1 |
| Salt (zero for key mode, random for password mode) | 16 |
| Random XChaCha20 nonce | 24 |
| Ciphertext and authentication tag | plaintext length + 16 |

The 25-byte outer header is authenticated associated data. Password-mode version 1 fixes Argon2id v1.3 to 2 passes, 64 MiB, and a 32-byte output, so neither attacker-controlled parameters nor changed library defaults alter decryption. Generated keys are 32 random bytes encoded as 43 URL-safe Base64 characters. Text is limited to 16 MiB; use file APIs for larger data.

Files start with `APSFILE1`, mode, 16-byte salt, and libsodium's 24-byte secretstream header. Each record contains a four-byte **little-endian** ciphertext length followed by the authenticated message, at most 65536 + 17 bytes. Data records use MESSAGE; an empty FINAL record and exact EOF terminate the file. All records authenticate the outer 25-byte header.

Derived keys, stream state, internal plaintext buffers, and owned secret-argument copies are wiped when released. Original files, caller-owned interpreter strings, application permissions, key storage, asymmetric encryption, and DRM are outside this initial implementation.

## Validation

```text
python Tests/Security_Tests.py
python Tests/Security_Tests.py --wsl
```

Tests exercise the module bridge, empty/binary text, randomization, wrong keys/passwords, tampering, truncated/reordered/appended files, empty/chunk-boundary files, no-clobber outputs, temporary-file cleanup, repeated loads, and Windows/Linux interoperability.

Upstream references: [XChaCha20-Poly1305](https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305/xchacha20-poly1305_construction), [secretstream](https://doc.libsodium.org/secret-key_cryptography/secretstream), [password key derivation](https://doc.libsodium.org/password_hashing/default_phf).
