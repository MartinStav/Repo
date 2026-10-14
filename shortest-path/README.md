# Najkratšia cesta v grafe

Dijkstrov algoritmus nad neorientovaným váženým grafom; pridávanie, zmena a odstránenie hrán.

## Technológie

C11.

## Spustenie

Príkazy spúšťaj z priečinka `shortest-path/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target shortest_path
../build/shortest_path < examples/input.txt
```

## Rozsah a obmedzenia

Nezáporné váhy. Prvý riadok: počet vrcholov a hrán; potom hrany (u, v, váha). Príkazy s u v, i u v váha, d u v, u u v rozdiel.

[Späť na prehľad](../README.md).
