# IPK Projekt 1
Projekt 1 pre predmet IPK - vytvorenie lightweight serveru v jazyku C++, ktorý prostredníctvom protokolu HTTP poskytuje základné informácie o systéme.

**autor**: Adam Zvara <br/>
**login:** xzvara01 <br/>
**email:** xzvara01@stud.fit.vutbr.cz <br/>

## Spustenie
Rozbalenie priečinku xzvara01.zip a preklad pomocou priloženého súboru Makefile vytvorí spustiteľný súbor **hinfosvc**. Syntax spustenia je `./hinfosvc port_number`. Následne sa vytvorí spojenie na danom čísle portu a so serverom je možné komunikovať.
```
$ unzip xzvara01.zip
$ make
$ ./hinfosvc 12345
```

## Použitie

Program je schopný odpovedať na základné požiadavky pomocou HTTP protokolu a poskytuje informácie ako:
- názov serveru (*hostname*)
- názov procesoru použitého na serveri (*cpu-name*)
- aktuálnu záťaž procesoru (*load*)

### Príklady
Po preklade je potrebné spustenie serveru
```
$ ./hinfosvc 8080 &
```

Následne sa môžeme dotazovať niekoľkými spôsobmi, buď priamo pomocou intenetového prehliadača, pomocou nástroju *wget* alebo *curl*.

**hostname**
```
$ curl http://localhost:8080/hostname
notebook
```
**cpu-name**
```
$ curl http://localhost:8080/cpu-name
Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz
```
**load**
```
$ curl http://localhost:8080/load
5%
```
