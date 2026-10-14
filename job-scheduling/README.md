# Plánovanie úloh podľa termínu a zisku

Chamtivý výber jednotkových úloh podľa zisku; voľné časové sloty sa hľadajú pomocou disjunktných množín.

## Technológie

C++17.

## Spustenie

Príkazy spúšťaj z priečinka `job-scheduling/`.

```sh
cmake -S .. -B ../build
cmake --build ../build --target job_scheduling
../build/job_scheduling < examples/input.txt
```

## Rozsah a obmedzenia

Vstup: počet úloh a dvojice termín/zisk. Hodnota 0 spustí vstavanú ukážku. Použitý je variant hľadajúci skorší voľný slot, nie jednoduchý pokus obsadiť iba presný termín.

[Späť na prehľad](../README.md).
