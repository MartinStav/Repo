# Prenos správ a súborov cez UDP

Vlastná 8-bajtová hlavička, CRC, fragmentácia, ACK/NACK, handshake a heartbeat. Obsahuje aj Lua dissector pre Wireshark.

## Technológie

Python 3, Wireshark pre dissector.

## Spustenie

Príkazy spúšťaj z priečinka `udp-file-transfer/`.

```sh
python peer.py
```

## Rozsah a obmedzenia

Experimentálny prototyp. V dvoch termináloch zvoľ vzájomne prepojené localhost porty. Handshake začína jeden peer; potrebné načasovanie a obnova spojenia nie sú pokryté automatickou kontrolou. Kontrolované sú formát hlavičky a CRC, nie spoľahlivosť kompletného prenosu.

[Späť na prehľad](../README.md).
