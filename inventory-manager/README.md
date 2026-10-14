# Správa skladu a skladových nákladov

Konzolová simulácia predaja, naskladnenia a účtovania skladových nákladov; načítanie a uloženie zásob.

## Technológie

C++17.

## Spustenie

Príkazy spúšťaj z priečinka `inventory-manager/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target inventory_manager
../build/inventory_manager examples/items.txt examples/inventory.txt ../build/inventory-out.txt 1000 < examples/commands.txt
```

## Rozsah a obmedzenia

Položky: ID predajná_cena skladový_náklad. Zásoby: ID množstvo. Príkazy: sell ID množstvo, store ID množstvo, none. Ide o výučbovú simuláciu, nie účtovný systém.

[Späť na prehľad](../README.md).
