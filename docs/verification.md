# Overenie pripraveného výberu

Kontrola 14. 10. 2026.

## Lokálne úspešné kontroly

- Spoločné CMake zostavenie všetkých ôsmich C/C++ programov na macOS ARM64.
- 22 automatických testov: platnosť trás a veľkosť populácie, zlepšenie a monotónnosť
  najlepšej trasy pri žíhaní, duplicitné body pri zhlukovaní, numerická kontrola gradientu,
  nezávislé modely a učenie XOR, formát UDP hlavičky, referenčné CRC a pokračovanie heartbeatu.
- Príklady AVL stromu, hash tabuľky, Dijkstrovho a Primovho algoritmu vrátane
  nedostupného vrcholu/nesúvislej komponenty, plánovania, optimálneho spájania,
  skladových nákladov a organizovania syntetických testovacích súborov.
- Štyri Python demonštrácie spustené cez ich príkazové rozhranie bez grafov.
- Python syntaktická kontrola vrátane MNIST skriptu a syntaktická kontrola
  všetkých JavaScript súborov hry.
- MiniDrive: zostavenie klienta, servera a vstavaného testu pripojenia knižníc.
- MiniDrive: lokálna komunikácia s dočasnými dátami — verejný handshake, LIST a MKDIR.
- Existencia lokálnych odkazov v dokumentácii a odkazovaných skriptov/štýlov v HTML hry.

## Automatické kontroly na GitHube

Workflow `Project checks` zostavuje C/C++ programy a spúšťa 22 testov,
pripravuje Java 21 a zostavuje Maven aplikáciu. Samostatne zostavuje MiniDrive,
spúšťa test knižníc a lokálnu komunikáciu: verejný handshake, LIST a MKDIR.
Výsledok konkrétneho behu je dostupný na karte Actions.

## Čo tieto kontroly nepotvrdzujú

- Úplný tréning a dosiahnutú presnosť MNIST; PyTorch tréning so stiahnutím datasetu
  nebol lokálne spustený.
- Všetky hraničné vstupy historických C programov.
- Kompletný prenos, fragmentové straty, obnovu spojenia a timeouty UDP prototypu.
- Úplný upload/download/resume/synchronizačný protokol MiniDrive.
- Ručné prejdenie všetkých obrazoviek hry a JavaFX aplikácie. Lokálne nie je nainštalované JDK;
  zostavenie Java aplikácie preto preveruje GitHub workflow.
- Licenciu pôvodných herných obrázkov a zvukov.

## Zopakovanie kontrol

```sh
python -m pip install -r genetic-route-search/requirements.txt
cmake -S . -B build
cmake --build build
MPLBACKEND=Agg python -m unittest discover -s tests -v
cmake -S minidrive -B minidrive/build
cmake --build minidrive/build
minidrive/build/tests/integration_smoke
python3 tests/check_minidrive.py
```

Posledný krok spustí krátko lokálny server; používa iba dočasné vymyslené dáta a server po kontrole ukončí.
