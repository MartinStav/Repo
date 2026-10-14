# Organizátor fotografií

Triedenie fotografií do rok/mesiac podľa času poslednej úpravy, detekcia duplicít podľa veľkosti a FNV-1a hashov.

## Technológie

C++17.

## Spustenie

Príkazy spúšťaj z priečinka `photo-organizer/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target photo_organizer
../build/photo_organizer /cesta/ku/kopii-fotiek /cesta/ku/vystupu --keep-duplicates
```

## Rozsah a obmedzenia

Pracuj s kópiou vstupu: program presúva súbory a bez --keep-duplicates maže duplicity. Čas je čas súboru, nie EXIF. Hash nie je kryptografický a zhoda hashov sa nepotvrdzuje porovnaním bajtov. Výstup musí byť mimo vstupného stromu.

[Späť na prehľad](../README.md).
