# Simulátor hlasovania

JavaFX aplikácia s registráciou, hlasovaním, správou kandidátov a simulovanými voličmi na pozadí.

## Technológie

JDK 21, Maven Wrapper, JavaFX.

## Spustenie

Príkazy spúšťaj z priečinka `voting-simulator/`.

```sh
cp src/main/resources/Authentication.example.csv src/main/resources/Authentication.csv
./mvnw javafx:run
```

## Rozsah a obmedzenia

Ukážkový účet demo-admin / demo-only. Heslá sa v tejto historickej simulácii ukladajú ako text; používaj iba vymyslené účty. Nie je to systém pre skutočné voľby. Spúšťaj z tohto priečinka kvôli relatívnym cestám. Java zostavenie vyžaduje JDK 21.

[Späť na prehľad](../README.md).
