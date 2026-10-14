# MiniDrive Documentation

## Build and Run

Projekt sa kompiluje pomocou CMake:
```
cmake -S . -B build
cmake --build build
```

Server sa spustí:
```
./build/server/server --port 9000 --root ./data/server_root
```

Klient sa pripojí:
```
./build/client/client 127.0.0.1:9000
```

## Design Decisions

### Thread-per-Client Model

Server používa thread-per-client model - každý klient dostane vlastný thread. Každý thread má vlastný file descriptor a spracováva príkazy nezávisle.

### File-based User Database

Používateľská databáza je uložená v textovom súbore `users.db`. Každý riadok obsahuje username a hash hesla:
```
username $argon2id$v=19$m=65536,t=2,p=1$salt$hash
```

Hash obsahuje salt a hash v jednom stringu.

### Path Traversal Protection

Používateľ nesmie mať prístup mimo svojho root adresára. Implementácia je cez funkciu `is_path_safe()`, ktorá normalizuje cestu pomocou `std::filesystem::canonical()` a kontroluje, či normalizovaná cesta začína user_root.

### Password Hashing

Používam Argon2id z libsodium. Salt je automaticky súčasťou hash stringu.

### Resume Transfers

Resume funguje len v private mode. Server ukladá `.part` súbory pri uploadoch, klient ukladá `_upload_*` a `_download_*` marker súbory s metadátami. Staré `.part` súbory (staršie ako 1 hodina) sa mažú pri štarte servera.

### Multiple Sessions

Server podporuje viacero sessions pre toho istého používateľa súčasne. Každý thread má vlastný kontext.

### SYNC Algorithm

SYNC funguje jednosmerne (local -> remote):
1. Získa zoznam súborov z servera s hashmi cez SYNC_LIST
2. Prejde lokálny adresár a vypočíta hash pre každý súbor
3. Porovná hashe - ak sa líšia alebo súbor neexistuje na serveri, uploaduje ho
4. Ak súbor existuje na serveri ale nie lokálne, vymaže ho na serveri

Používa sa hash comparison namiesto timestampov.

## Protocol Description

### Message Framing

Všetky správy sú JSON dokumenty kódované v UTF-8. Každá správa má prefix - 4 bajty (uint32_t) s dĺžkou správy v network byte order.

```
+----------------+------------------------+
| 4 bytes length | JSON payload           |
+----------------+------------------------+
```

Funkcie `read_message()` a `write_message()` v `protocol.cpp` toto riešia - najprv prečítajú 4 bajty, potom JSON podľa dĺžky.

### Authentication

Autentifikácia sa deje hneď po pripojení, pred akýmkoľvek iným príkazom.

#### Public Mode

Ak klient neposkytne username, pošle:
```json
{"type": "public"}
```

Server odpovie:
```json
{"status": "OK", "code": 0, "message": "", "data": {"message": "Public mode"}}
```

#### Private Mode - Login

Klient pošle:
```json
{"type": "auth", "username": "user1", "password": "password123"}
```

Ak je prihlásenie úspešné:
```json
{"status": "OK", "code": 0, "message": "", "data": {"message": "Logged as user1"}}
```

Ak je heslo nesprávne:
```json
{"status": "ERROR", "code": 101, "message": "Invalid password", "data": {}}
```

#### Registration

Ak používateľ neexistuje, server odpovie s error code 102. Klient sa opýta používateľa, či sa chce registrovať. Ak áno, pošle:
```json
{"type": "register", "username": "user1", "password": "password123"}
```

Server odpovie:
```json
{"status": "OK", "code": 0, "message": "", "data": {"message": "User registered successfully"}}
```

Alebo ak používateľ už existuje:
```json
{"status": "ERROR", "code": 103, "message": "User already exists", "data": {}}
```

### Command Format

Po úspešnej autentifikácii (alebo v public mode) klient posiela príkazy.

#### Request

Všetky príkazy majú rovnaký formát:
```json
{"cmd": "COMMAND_NAME", "args": {...}}
```

`args` je JSON objekt s parametrami príkazu. Napríklad pre LIST:
```json
{"cmd": "LIST", "args": {"path": "subdir"}}
```

#### Response

Úspešná odpoveď:
```json
{"status": "OK", "code": 0, "message": "", "data": {...}}
```

Chybová odpoveď:
```json
{"status": "ERROR", "code": 200, "message": "Error description", "data": {}}
```

`code` je vždy 0 pri OK, pri ERROR je to chybový kód.

### Commands

#### LIST

Zoznam súborov a adresárov v zadanom adresári.

Request:
```json
{"cmd": "LIST", "args": {"path": "subdir"}}
```

Ak `path` nie je zadaný, použije sa aktuálny adresár.

Response:
```json
{
  "status": "OK",
  "code": 0,
  "message": "",
  "data": {
    "entries": [
      {"name": "file.txt", "type": "file", "size": 1234},
      {"name": "dir", "type": "dir"}
    ]
  }
}
```

`type` môže byť `"file"` alebo `"dir"`. `size` je len pre súbory.

#### UPLOAD

Nahrávanie súboru na server.

Request:
```json
{
  "cmd": "UPLOAD",
  "args": {
    "remote_path": "path/to/file.txt",
    "file_size": 12345,
    "file_hash": "abc123def456..."
  }
}
```

`file_hash` je hash súboru (ako hex string) vypočítaný pomocou hash funkcie z libsodium.

Server odpovie:
```json
{"status": "OK", "code": 0, "message": "", "data": {}}
```

Potom klient posiela binárny obsah súboru po chunkoch (64KB). Server ukladá do `.part` súboru (v private mode), po prijmutí celého súboru overí hash. Ak je hash OK, premenuje `.part` na finálny súbor a odpovie OK. Ak hash nie je OK, vymaže `.part` a odpovie ERROR:

```json
{"status": "ERROR", "code": 206, "message": "Hash mismatch", "data": {}}
```

#### DOWNLOAD

Sťahovanie súboru zo servera.

Request:
```json
{"cmd": "DOWNLOAD", "args": {"remote_path": "path/to/file.txt", "start_offset": 0}}
```

`start_offset` sa používa pri resume - ak je > 0, server pošle súbor od tohto offsetu.

Response:
```json
{
  "status": "OK",
  "code": 0,
  "message": "",
  "data": {
    "file_size": 12345,
    "file_hash": "abc123def456..."
  }
}
```

Potom server posiela binárny obsah súboru. Klient ho uloží a overí hash.

#### SYNC_LIST

Interný príkaz pre SYNC - vráti zoznam všetkých súborov v adresári s hashmi.

Request:
```json
{"cmd": "SYNC_LIST", "args": {"path": "remote_dir"}}
```

Response:
```json
{
  "status": "OK",
  "code": 0,
  "message": "",
  "data": {
    "files": [
      {"path": "rel/path/file1.txt", "hash": "hash1..."},
      {"path": "rel/path/file2.txt", "hash": "hash2..."}
    ]
  }
}
```

`path` je relatívna cesta od root adresára používateľa. `hash` je hash súboru vypočítaný pomocou hash funkcie z libsodium.

### Error Codes

Definoval som vlastné chybové kódy rozdelené do kategórií:

**Autentifikačné chyby (100-104):**
- 100: User already logged in (nie je používané, keďže podporujeme multiple sessions)
- 101: Invalid password
- 102: User not found
- 103: User already exists
- 104: Registration failed

**File system chyby (200-208):**
- 200: Path traversal not allowed
- 201: Directory not found
- 202: Target already exists
- 203: File not found
- 204: File too large (>4GB)
- 205: Cannot create file
- 206: Hash mismatch
- 207: Transfer incomplete
- 208: Resume only available in private mode

**Všeobecné chyby (300-301):**
- 300: Unknown command
- 301: Internal error
