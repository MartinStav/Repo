# Martinove projekty

Výber programov a experimentov, usporiadaný podľa toho, čo riešia.
Každý priečinok predstavuje jednu konkrétnu implementáciu a má vlastný návod.
CatShow zostáva vo svojom samostatnom repozitári.

## Optimalizácia a učenie

- [Genetické hľadanie trasy](genetic-route-search/) — obchodný cestujúci; turnajový a ruletový výber, kríženie a mutácia.
- [Hľadanie trasy simulovaným žíhaním](simulated-annealing-route-search/) — ochladzovanie a prijímanie horších kandidátov.
- [Hierarchické zhlukovanie](hierarchical-clustering/) — aglomerácia s centroidom alebo reprezentatívnym bodom.
- [Neurónová sieť pre logické funkcie](logic-gate-neural-network/) — vlastná spätná propagácia v NumPy, XOR/AND/OR a momentum.
- [Rozpoznávanie číslic MNIST](mnist-digit-recognition/) — PyTorch sieť; SGD, momentum a Adam.

## Dátové štruktúry a grafy

- [AVL strom](avl-tree/) — samovyvažovanie, vloženie, odstránenie a intervalové vyhľadávanie.
- [Hashovacia tabuľka](hash-table/) — zreťazené kolízie a aktualizácie záznamov.
- [Najkratšia cesta](shortest-path/) — Dijkstrov algoritmus na váženom grafe.
- [Minimálna kostra](minimum-spanning-tree/) — Primov algoritmus.
- [Plánovanie úloh](job-scheduling/) — chamtivé plánovanie s voľnými slotmi cez disjunktné množiny.
- [Optimálne spájanie súborov](optimal-file-merge/) — prioritná fronta a minimálne náklady.

## Nástroje a sieťové programy

- [MiniDrive](minidrive/) — C++ klient/server, prenos súborov a synchronizácia.
- [Prenos cez UDP](udp-file-transfer/) — vlastný protokol a Wireshark dissector; experimentálny prototyp.
- [Organizátor fotografií](photo-organizer/) — priečinky podľa dátumu úpravy a spracovanie duplicít.
- [Správa skladu](inventory-manager/) — konzolová simulácia zásob a skladových nákladov.

## Aplikácie

- [BALLRUN](ballrun/) — hra v prehliadači.
- [Simulátor hlasovania](voting-simulator/) — JavaFX aplikácia s generovanými voličmi.

## Spoločné zostavenie C a C++ ukážok

```sh
cmake -S . -B build
cmake --build build
```

MiniDrive sa zostavuje samostatne, pretože potrebuje vlastné externé závislosti.
Python projekty majú vlastné `requirements.txt`; odporúčané je virtuálne prostredie.

## Kontroly a pôvod

[Výber súborov a úpravy](docs/selection.md) vysvetľuje konkrétne zahrnuté implementácie,
opravy a vynechané varianty. [Overenie](docs/verification.md) rozlišuje spustené kontroly
od častí, ktoré vyžadujú samostatné vyskúšanie.

Zachované sú pôvodné algoritmické implementácie a existujúce aplikácie. Pre ukážky boli
doplnené zrozumiteľné názvy, návody, malé vymyslené vstupy a opravy nájdených chýb.
Niektoré pôvodné komentáre uvádzajú pomoc ChatGPT alebo externý zdroj; tieto atribúcie zostávajú v kóde.
Tento výber nepridáva novú licenciu na pôvodné obrázky, zvuk ani cudzie časti.
