# Konkrétne zahrnuté implementácie

Každý projekt je pomenovaný podľa svojej funkcie. Názvy pôvodných súborov nižšie
slúžia na identifikáciu vybraných implementácií, nie ako štruktúra repozitára.

## Optimalizácia a učenie

1. **`genetic-route-search/main.py`** — z `1cA_120997_Stavrovsky.py`.
   Problém obchodného cestujúceho; turnajový a ruletový výber, kríženie,
   inverzná mutácia a elitizmus. Doplnené reprodukovateľné parametre.
   Opravené poradie indexov mutácie a veľkosť populácie po elitizme.
2. **`simulated-annealing-route-search/main.py`** — z `1cC_120997_Stavrovsky.py`.
   Simulované žíhanie pre rovnaký typ trasy. Opravené prijatie lepšieho riešenia,
   pravdepodobnosť prijatia horšieho riešenia, poradie indexov mutácie a limit
   celkového počtu iterácií. Graf teraz zobrazuje skutočnú dĺžku trasy.
3. **`hierarchical-clustering/main.py`** — z `2c_120997_Stavrovsky.py`.
   Aglomeratívne zhlukovanie s centroidom a bodom najbližším k centroidu.
   Zachovaný pôvodný variant reprezentanta; nejde o presný medoid.
   Opravené zlúčenie bodov so vzdialenosťou nula. Ukážka začína so 100 bodmi;
   pôvodné veľké zadanie by malo vysoké nároky na pamäť a čas.
4. **`logic-gate-neural-network/main.py`** — z `3ab_120997.py`.
   Vlastné lineárne vrstvy a spätná propagácia, Tanh/ReLU/Sigmoid a MSE.
   Experimenty XOR/AND/OR, jedna alebo dve skryté vrstvy, dve rýchlosti učenia
   a tréning s momentom alebo bez neho. Každý experiment teraz začína novým
   modelom s rovnakým seedom. Pôvodná pomoc ChatGPT pri vizualizácii ostáva označená.
5. **`mnist-digit-recognition/main.py`** — z `3aa_120997.py`.
   PyTorch klasifikátor číslic 0–9, s porovnaním SGD, SGD s momentom a Adam.
   Každý optimalizátor teraz dostane nový model a rovnaké poradie tréningových
   vzoriek. MNIST sa sťahuje pri spustení. Zachované označenie pomoci ChatGPT
   vo vizualizácii.

Notebook `new.ipynb` sa nepridáva ako ďalšia verzia toho istého MNIST príkladu.
Prázdny `MINST.ipynb`, dočasné súbory, kópie skriptov a stiahnutý dataset sú vynechané.

## Dátové štruktúry a grafy

6. **`avl-tree/main.c`** — existujúci `C programs/binary tree struct.c`,
   zhodný s archívnym `cv5final.c`. Zachovaný strom a doplnený samostatný návod.
   Obmedzené načítanie textu na veľkosť vstupného poľa.
7. **`hash-table/main.c`** — existujúci `C programs/hashtable.c`,
   zhodný s archívnym `cv6.c`. Zachované zreťazenie kolízií a operácie so zostatkom.
   Doplnený presný formát trojzložkového kľúča a obmedzené načítanie vstupu.
8. **`shortest-path/main.c`** — z `cv7.c`.
   Dijkstrov algoritmus, neorientovaný vážený graf a operácie s hranami.
9. **`minimum-spanning-tree/main.c`** — z `cv8.c`.
   Primov algoritmus. Opravená inicializácia rodičov pri nesúvislom grafe,
   typ výberovej váhy a kontrola začiatočného vrcholu.
10. **`job-scheduling/main.cpp`** — z `assigment2.cpp`.
    Plánovanie jednotkových úloh s termínom a ziskom pomocou disjunktných množín.
    Opravené odovzdávanie štruktúry odkazom pre kompresiu ciest a spracovanie
    neplatných/neprofitabilných úloh. Počet slotov je najviac počet úloh.
11. **`optimal-file-merge/main.cpp`** — z `assigment3.cpp`.
    Optimálne spájanie veľkostí pomocou min-heapu. Doplnené chýbajúce hlavičky
    pre prioritnú frontu a porovnávač.

Varianty grafov s menami iných autorov sa nepridávajú.
Jednoduchší plánovač `assigment1.cpp` sa nepridáva ako duplikát.
`assigment4.cpp` obsahuje pokus o Huffmanove kódy, ktorý neodvodzuje dĺžky
z Huffmanovho stromu; vyžaduje samostatnú opravu pred prezentovaním ako hotový algoritmus.

## Nástroje a siete

12. **`minidrive/`** — jedna zdrojová verzia MiniDrive:
    koreňový CMake, `cmake/Dependencies.cmake`, `client/`, `server/`, `shared/`,
    test zdrojov/knižníc a `documentation.md`. Závislosti, používateľské dáta
    a zostavené programy sa nepridávajú. Druhé odovzdanie s odlišnými textami
    hlášok netvorí ďalší projekt.
13. **`photo-organizer/main.cpp`** — hlavná verzia organizátora fotografií.
    Čas poslednej úpravy, filtrovanie prípon, FNV-1a hash a presúvanie súborov.
    Zachovaný odkaz na externý zdroj hash funkcie. Ukážkové fotky sa nepridávajú.
14. **`inventory-manager/main.cpp`** — hlavná verzia skladovej simulácie,
    doplnená o malé číselné vstupy `items.txt`, `inventory.txt` a nový
    ukážkový príkazový vstup. Opravená kontrola chýbajúcich vstupov a nekladných množstiev.
15. **`udp-file-transfer/peer.py`** — z `FO_120997.py`;
    **`wireshark-dissector.lua`** — z `WS_120997.lua`.
    Vybraná dvojica z jedného odovzdania, nie kombinácia nesúvisiacich variantov.
    Opravený odkaz na globálny peer pri ukončení, podmienka odosielania heartbeatu
    a dekódovanie diagnostiky na hraniciach UTF-8 fragmentov.
    Prototyp naďalej vyžaduje samostatné overenie kompletných prenosov a obnovy spojenia.

`NaiveString` zostáva vynechaný, pretože copy/move operácie sú iba TODO kostra.
Stiahnutý Codespaces základ a pokusy `copy` sa nevydávajú za ďalšie vlastné projekty.

## Zachované aplikácie

16. **`ballrun/`** — existujúci `JavaScript Game/`, premenovaný podľa názvu hry.
    Herný kód a aktíva sú zachované, odstránené sú systémové súbory a nastavenia editora.
17. **`voting-simulator/`** — existujúci `Java Election System/`,
    premenovaný podľa účelu. Zachované Java/JavaFX zdroje, FXML, obrázky a Maven Wrapper.
    Pôvodný autentifikačný súbor nahradila vzorová konfigurácia s demo účtom.
    Pri štarte sa už nevypisuje heslo správcu. Maven konfigurácia teraz smeruje
    na existujúcu vstupnú triedu a používa stabilnú JavaFX 21.0.2 namiesto pôvodnej EA verzie.

## Materiály mimo tohto výberu

- CatShow má vlastný repozitár a nevkladá sa znova.
- Nexus, DeliGo a Gemini Coffee majú vlastné existujúce projekty a tímový kontext.
  Do tejto kolekcie sa nekopíruje ich celá pracovná história.
- Analýza zdravotných dát zostáva mimo výberu, kým sa nepreverí pôvod dát
  a osobné údaje v uložených výstupoch notebookov.
- Prednášky, cudzie zadania, opakované odovzdania, fotoarchív, osobné dáta,
  lokálne heslá a generované programy sa nepridávajú.

Prístup k pôvodnému archívu bol iba na čítanie. Existujúca Git história
tohto repozitára zostáva zachovaná; čistenie aktuálnych súborov neprepisuje staré commity.
