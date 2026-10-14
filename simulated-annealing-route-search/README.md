# Hľadanie trasy simulovaným žíhaním

Hľadanie kratšej uzavretej trasy pomocou ochladzovania a pravdepodobnostného prijatia horšieho riešenia.

## Technológie

Python, NumPy, Matplotlib.

## Spustenie

Príkazy spúšťaj z priečinka `simulated-annealing-route-search/`.

```sh
python -m pip install -r requirements.txt
python main.py --seed 42 --cities 25 --iterations 3000
```

## Rozsah a obmedzenia

Heuristika negarantuje globálne optimum. Opravené prijímanie nového riešenia a obrátenie úseku trasy.

[Späť na prehľad](../README.md).

Bez interaktívnych grafov použi prepínač `--no-plots`.
