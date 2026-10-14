# Hashovacia tabuľka

Tabuľka so zreťazením kolízií pre textové kľúče a zostatky. Vloženie, vyhľadanie, zmena a odstránenie.

## Technológie

C11.

## Spustenie

Príkazy spúšťaj z priečinka `hash-table/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target hash_table
../build/hash_table < examples/input.txt
```

## Rozsah a obmedzenia

Kľúč obsahuje tri položky oddelené medzerou. Desatinné čísla používajú čiarku. Príkaz u pridáva rozdiel k zostatku; neprepisuje ho.

[Späť na prehľad](../README.md).
