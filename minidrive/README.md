# MiniDrive

C++20 klient/server pre prenos súborov, používateľské účty, obnovenie prenosu a jednosmernú synchronizáciu.

## Technológie

C++20, CMake, POSIX sockets, libsodium.

## Spustenie

Príkazy spúšťaj z priečinka `minidrive/`.

```sh
cmake -S . -B build
cmake --build build
./build/server/server --port 9000 --root ./data/server-root
# druhý terminál
./build/client/client 127.0.0.1:9000
```

## Rozsah a obmedzenia

Závislosti sa sťahujú pri konfigurácii. Implementácia používa POSIX API (Linux/macOS). Vstavaný smoke test overuje prevažne pripojenie knižníc; nie celý prenosový protokol. SYNC môže odstrániť vzdialené súbory chýbajúce lokálne; skúšaj iba ukážkové adresáre.

[Späť na prehľad](../README.md).
