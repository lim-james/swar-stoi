# SWAR (SIMD-Within-A-Register) STOI

> Initially belonged under low-latency-experiments, but I decided to make it a
> project of its own.

I was wondering if std::stoi could be faster. At a cost of being unsafe.

| Scenario | Method | Throughput (ints/ms) | Total Cycles | Total Instructions | IPC |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **LOWER**<br>*(4 chars)* | `std::stoi` | 28,922 | 8,953,057 | 36,751,432 | 4.10 |
| | `Predictable` | **49,013** | 5,554,968 | 20,788,170 | 3.74 |
| | `SWAR` | 46,724 | 5,581,259 | 21,392,540 | 3.83 |
| **MIDDLE**<br>*(7 chars)* | `std::stoi` | 22,665 | 11,148,809 | 47,169,692 | 4.23 |
| | `Predictable` | **35,216** | 7,228,822 | 27,007,266 | 3.74 |
| | `SWAR` | 34,386 | 7,504,151 | 27,619,716 | 3.68 |
| **UPPER**<br>*(>8 chars)* | `std::stoi` | 18,937 | 13,983,575 | 57,036,103 | 4.08 |
| | `Predictable` | 27,846 | 9,519,478 | 32,880,368 | 3.45 |
| | `SWAR` | **28,140** | **9,213,281** | **30,733,778** | 3.34 |
