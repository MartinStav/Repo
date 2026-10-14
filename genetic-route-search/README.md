# Genetické hľadanie trasy

Genetický algoritmus pre problém obchodného cestujúceho. Porovnáva turnajový a ruletový výber, používa kríženie, mutáciu a elitizmus.

## Technológie

Python, NumPy, Matplotlib.

## Spustenie

Príkazy spúšťaj z priečinka `genetic-route-search/`.

```sh
python -m pip install -r requirements.txt
python main.py --seed 42 --cities 25 --generations 200
```

## Rozsah a obmedzenia

Syntetické body; optimalizácia uzavretej trasy, nie navigácia po reálnej cestnej sieti.

[Späť na prehľad](../README.md).

Bez interaktívnych grafov použi prepínač `--no-plots`.
