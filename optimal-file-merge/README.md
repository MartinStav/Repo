# Optimálne spájanie súborov

Chamtivý algoritmus s prioritnou frontou: opakovane spojí dva najmenšie súbory a vypíše minimálne náklady spájania.

## Technológie

C++17.

## Spustenie

Príkazy spúšťaj z priečinka `optimal-file-merge/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target optimal_file_merge
../build/optimal_file_merge < examples/input.txt
```

## Rozsah a obmedzenia

Simulácia nákladov zo zadaných veľkostí. Nespája skutočné súbory. Hodnota 0 spustí dve vstavané ukážky.

[Späť na prehľad](../README.md).
