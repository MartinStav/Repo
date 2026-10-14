# Hierarchické zhlukovanie bodov

Aglomeratívne zhlukovanie syntetických 2D bodov: centroid a reprezentant vybraný ako bod najbližší k centroidu.

## Technológie

Python, NumPy, Matplotlib.

## Spustenie

Príkazy spúšťaj z priečinka `hierarchical-clustering/`.

```sh
python -m pip install -r requirements.txt
python main.py --points 100 --clusters 5 --seed 42
```

## Rozsah a obmedzenia

Variant označený v pôvodnom kóde medoid nie je presný medoid minimalizujúci súčet všetkých vzdialeností. Implementácia je zámerne malá a výpočtovo náročná; predvolená ukážka používa 100 bodov namiesto pôvodných približne 20 000.

[Späť na prehľad](../README.md).

Bez interaktívnych grafov použi prepínač `--no-plots`.
