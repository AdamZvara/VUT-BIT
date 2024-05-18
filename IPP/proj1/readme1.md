Implementačná dokumentácia k 1. úlohe do IPP 2021/2022
Meno a priezvisko: **Adam Zvara**
Login: **xzvara01**

### Implementácia hlavnej časti

Spracovanie vstupného súboru začína hľadaním hlavičky, ktoré prebieha v cykle, aby bolo možné preskočenie komentárov na začiatku súboru. Program je spracovávaný po riadkoch, pričom je každý zbavený komentárov a bielych znakov. Po úspešnom nájdní hlavičky sú zvyšné inštrukcie spracované v ďalšom cykle.

Každý načítaný riadok je po odstránení komentárov rozdelený na inštrukciu a prípadné argumenty. Na spracovanie inštrukcií je použitá trieda *Instruction*, ktorá obsahuje základné informácie o načítnej inštrukcii (operačný kód, poradie, počet argumentov). Poradie inštrukcie, je statická premenná a zvyšuje sa pri každom vytvorením nového objektu. Na zápis do výstupného XML formátu slúžia metódy *start_element/end_element*, ktorých zápis je realizovaný pomocou triedy *XMLWriter*. Zámenu špeciálnych znakov v názve premenných alebo v literáloch zapezpečuje knižnica XMLWriter.

Podľa typu inštrukcie, ktorý je daný počtom a typom argumentov zo zadania, sa kontrolujé platnosť jednotlivých argumenty. Na kontrolu povolených znakov v jednotlivých argumentoch sa používajú regulárne výrazy.

## Implementácia rozšírenia
#### Spracovanie argumentov
Argumenty sú postupne spracované v triede *Bonus_arg_handler*. Dôležitou časťou tejto triedy je zoznam *stats_array*, v ktorom sú udržiavané objekty triedy *Stat_file*, ktoré sú vytvorené po načítaní možnosti *--stats=file*. Tieto objekty obsaujú zoznam *stats_used*, ktorý je naplnený skupinou štatistík zadanou za možnosťou *--stats*. Takýmto spôsobom sa uchováva to, aké súbory majú byť vytvorené a aké skupiny štatistík majú obsahovať.

#### Výpočet štatistík:
Pre výpočet štatistík sa používa trieda *Statistics*. Jednoduché štatistiky ako loc, comments, jumps, labels majú počítadlá, ktoré sú modifikované priamo v priebehu analýzy programu. Štatistiky dopredných a spätných skokov sú vypočítané po celkovej analýze programu. Pre každú inštrukciu skoku sú uchovávané informácie o cieľovom návestí a pozícia inštrukcie skoku v kóde. Rovnaké informácie sú uložené aj pre návestia. Pri dopočítavaní dopredných a spätných skokov sa iteruje cez všetky uložené ciele skoku a hľadajú sa príslušné návestia s rovnakým menom. Následne sa pre všetky výskyty v kóde zisťuje (podľa pozície inštrukcie v kóde), či došlo ku skoku späť alebo vopred. Počet skokov na nedefinované návestie je vypočítaný ako rozdiel množín uložených cieľov skoku a uložených návestí.

#### Zápis do súboru:
Po zistení vypočítaní všetkých štatistík je pre každý objekt v zoznam *stats_file* vytvorený súbor, do ktorého sú zapísané požadované štatistiky.