# Neurónová sieť pre logické funkcie

Vlastné dopredné šírenie a spätná propagácia v NumPy. Učenie XOR, AND a OR; porovnanie jednej/dvoch skrytých vrstiev, rýchlosti učenia a momenta.

## Technológie

Python, NumPy, Matplotlib.

## Spustenie

Príkazy spúšťaj z priečinka `logic-gate-neural-network/`.

```sh
python -m pip install -r requirements.txt
python main.py --epochs 500 --seed 42
```

## Rozsah a obmedzenia

Každá funkcia a konfigurácia dostane nový model, aby porovnanie nepokračovalo v tréningu predchádzajúceho modelu.

[Späť na prehľad](../README.md).

Bez interaktívnych grafov použi prepínač `--no-plots`.
