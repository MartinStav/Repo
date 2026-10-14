# Rozpoznávanie rukou písaných číslic

Plne prepojená sieť 784 → 256 → 128 → 10 v PyTorch; tréning SGD, SGD s momentom a Adam, vyhodnotenie presnosti.

## Technológie

Python, PyTorch, Torchvision, Matplotlib.

## Spustenie

Príkazy spúšťaj z priečinka `mnist-digit-recognition/`.

```sh
python -m pip install -r requirements.txt
python main.py --epochs 1 --seed 42
```

## Rozsah a obmedzenia

Dataset MNIST sa sťahuje pri spustení a nepatrí do repozitára. Každý optimalizátor začína novým modelom s rovnakou inicializáciou. Úplný tréning nie je súčasťou rýchlej kontroly.

[Späť na prehľad](../README.md).

Bez interaktívnych grafov použi prepínač `--no-plots`.
