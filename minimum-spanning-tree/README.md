# Minimálna kostra grafu

Primov algoritmus nad neorientovaným váženým grafom, s výpisom celkovej váhy a vybraných hrán.

## Technológie

C11.

## Spustenie

Príkazy spúšťaj z priečinka `minimum-spanning-tree/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target minimum_spanning_tree
../build/minimum_spanning_tree < examples/input.txt
```

## Rozsah a obmedzenia

Prvý riadok: počet vrcholov; potom hrany (u, v, váha). Príkaz s vrchol spustí výpočet. Pri nesúvislom grafe ide o kostru dostupnej komponenty.

[Späť na prehľad](../README.md).
