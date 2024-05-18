# ipk-l4-scan

Program ipk-l4-scan slúži na skenovanie portov rôznych zariadení. Pri spustení je možné definovať rozhranie, z ktorého budú porty odoslané (interface), rozsah skenovaných portov a adresu alebo názov domény skenovaného zariadenia.

**Autor**: Adam Zvara - xzvara01(@vutbr.cz)

## Kompilácia

V hlavnom adresári projektu spustite príkaz `make`, ktorý by mal aplikáciu skompilovať.

## Použitie
Syntax spustenia programu je:

`sudo ./ipk-l4-scan interface ports domain {timeout}`

**Program využíva raw sokety, preto je nutné ho spustiť s rootovskými oprávneniami**

#### Nájdenie rozhraní
Pre nájdenie všetkých rozhraní vašeho zariadenia spustite aplikáciu s prepínačom -i (--interface).
```bash
$ sudo ./ipk-l4-scan -i
Available interfaces:
lo
enp0s3
```

#### Určovanie portov
Porty je možné definovať pomocou intervalu (*range_notation*) alebo výpisom jednotlivých portov (*list_notation*).<br>
Príklad definície pomocou intervalu:
```bash
$ sudo ./ipk-l4-scan -i lo -t 10-100 localhost
```
Začne skenovať porty v intervale `<10-100>` cez rozhranie `lo` na adrese `localhost`.

Príklad definície pomocou výpisu:
```bash
$ sudo ./ipk-l4-scan -i lo -t 10,20,30,40 localhost
```
Začne skenovať porty `{10,20,30,40}` cez rozhranie `lo` na adrese `localhost`.

#### Podpora IPv4/IPv6
Rozhodnutie o tom, či sa bude jednať o IPv4 alebo IPv6 skenovanie je na základe domény. V prípade, že sa jedná o doménové meno je preferovaná IPv4 adresa. Doménu je však možné aj definovať priamo IP adresou a vynútiť skenovanie IPv6.
`sudo ./ipk-l4-scan -i lo --pt 10-20 ::1`<br>
**! IPv6 skenovanie je limitované na localhost !**
Viac informácií je možné nájsť v manuále.

#### Zoznam odovzdaných súborov
```
|-- src
|   |-- common.c
|   |-- common.h
|   |-- parse_args.c
|   |-- parse_args.h
|   |-- tcp_scan.c
|   |-- tcp_scan.h
|   |-- udp_scan.c
|   `-- udp_scan.h
| 
|-- tests
|   |-- tests.sh
|
|-- ipk-l4-scan.c
|-- Makefile
|-- readme.md
|-- manual.pdf
```