1) Vytvořte jednoduchý síťový TCP, UDP skener v C/C++/C#. Program oskenuje zadanou IP adresu (ať už IPv4 nebo IPv6) a porty. Na standardní výstup vypíše, v jakém stavu se porty nacházejí (otevřený, filtrovaný, uzavřený) (13 b)
2) Vytvořte relevantní manuál/dokumentaci k projektu (7b)

Aplikace oskenuje zvolené porty na daném síťovém zařízení. Pakety musí být odeslané pomocí BSD sockets. Odchytávat odpovědi můžete např. pomocí knihovny libpcap. Při implementaci lze využít knihovny netinet či arpa/inet.h.

TCP skenování:
Posílá pouze SYN pakety. Neprovádí tedy kompletní 3-way-handshake. Pokud přijde odpověď RST - port je označen jako uzavřený. Pokud po daný časový interval nepřijde ze skenovaného portu odpověď, je nutno ověřit dalším paketem a teprve potom port označit jako filtrovaný. Pokud na daném portu běží nějáká služba, je port označen jako otevřený. Více viz RFC 793.

UDP skenování:
U UDP skenování můžete uvažovat, že daný počítač při zavřeném portu odpoví ICMP zprávou typu 3, kódu 3 (port unreachable). Ostatní porty považujte za otevřené.

Volání programu:

```./ipk-l4-scan [-i interface | --interface interface] [--pu port-ranges | --pt port-ranges | -u port-ranges | -t port-ranges] {-w timeout} [domain-name | ip-address] ```

kde:

- -i eth0 (právě jedno rozhraní, na skrz nějžd bude skenování probíhat) či --interface. Nebude-li tento parametr uveden, či bude-li uvedené jen -i/--interface bez hodnoty, vypíše se seznam aktivních rozhraní (další informace nad rámec seznamu interface jsou vítané, ale ne nutné)
- -t nebo --pt, -u nebo --pu port-ranges - skenované tcp/udp porty, povolený zápis např. --pt 22 nebo --pu 1-65535 nebo --pt 22,23,24
argumenty --pu a --pt mohou být zadány zvláště, tj. nemusí se vyskytovat oba naráz, když chce uživatel jen TCP či jen UDP skenování
- -w 3000 či --wait 3000, je timeout čekání na odpověď v milisekundách pro jedno oskenování portu. Tento parametr je volitený, v případě jeho nepřítomnosti se použije hodnota 5000 (tj. pět vteřin)
domain-name | ip address - doménové jméno, nebo IP adresa skenovaného stroje
argumenty mohou být v libovolném pořadí

Příklad volání:

```
./ipk-l4-scan --interface eth0 -u 53,67 2001:67c:1220:809::93e5:917
./ipk-l4-scan -i eth0 -w 1000 -t 80,443,8080 www.vutbr.cz
```


Příklad chování:
```
./ipk-l4-scan -i eth0 --pt 21,22,143 --pu 53,67 localhost

Interesting ports on localhost (127.0.0.1):
PORT STATE
21/tcp closed
22/tcp open
143/tcp filtered
53/udp closed
67/udp open
```

V dobré dokumentaci se OČEKÁVÁ následující: titulní strana, obsah, logické strukturování textu, výcuc relevantních informací z nastudované literatury, popis zajímavějších pasáží implementace, sekce o testování (ve které kromě vlastního programu otestujete nějaký obecně známý open-source nástroj), bibliografie, popisy k řešení bonusových zadání.

DOPORUČENÍ/OMEZENÍ:
- Skenujte pouze počítače, které jsou ve Vašem vlastnictví (případně localhost).
- Všechny implementované programy by měly být použitelné (např. scanner, který skenuje zadaný rozsah přes hodinu moc použitelný není) a řádně komentované. Pokud už přejímáte zdrojové kódy z různých tutoriálů či příkladů z Internetu (ne mezi sebou pod hrozbou ortelu disciplinární komise), tak je POVINNÉ správně vyznačit tyto sekce a jejich autory dle licenčních podmínek, kterými se distribuce daných zdrojových kódů řídí. V případě nedodržení bude na projekt nahlíženo jako na plagiát!
- U syntaxe vstupních voleb jednotlivým programům složené závorky {} znamenají, že volba je nepovinná (pokud není přítomna, tak se použíje implicitní hodnota), oproti tomu [] znamená povinnou volbu. Přičemž pořadí jednotlivých voleb a jejich parametrů může být libovolné. Pro jejich snadné parsování se doporučuje použít funkci getopt().
- Výsledky vaší implementace by měly být co možná nejvíce multiplatformní mezi OS založenými na unixu, ovšem samotné přeložení projektu a funkčnost vaší aplikace budou testovány na referenčním Linux image (viz obecné pokyny k zadání) pro síťové předměty (přihlašovací údaje student / student).
- Program by se měl dát v kterémkoli okamžiku korektně ukončit pomocí Ctrl+C.