
/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
 * uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/ 

#include "c206.h"

int error_flag;
int solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error() {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = TRUE;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
    list->activeElement = NULL;
    list->firstElement = NULL;
    list->lastElement = NULL;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
    DLLElementPtr iterator = list->firstElement;

    // V pripade ze DLL obsahuje aspon 1 prvok 
    if (iterator != NULL) {

        // Sekvencny priechod cez DLL. Nemaze sa aktualny prvok ale predchadzajuci
        // pretoze by doslo ku strate ukazatela na dalsi prvok.  
        while (iterator->nextElement != NULL) {
            iterator = iterator->nextElement;
            free(iterator->previousElement);
        }
        
        free(iterator);
    }

    list->activeElement = NULL;
    list->firstElement = NULL;
    list->lastElement = NULL;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, int data ) {
    // Vytvorenie noveho prvku
    DLLElementPtr newItem = malloc(sizeof(*newItem));

    if (newItem == NULL) {
        DLL_Error();
        return;
    }

    newItem->data = data;
    newItem->previousElement = NULL;    //previousElement bude NULL v oboch pripadoch

    // Vkladanie do prazdneho DLL
    if (list->firstElement == NULL) {
        newItem->nextElement = NULL;

        list->firstElement = list->lastElement = newItem;
    } else { // Vkladanie do neprazdneho DLL
        newItem->nextElement = list->firstElement;

        list->firstElement->previousElement = newItem;
        list->firstElement = newItem;
    }
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, int data ) {
     // Vytvorenie noveho prvku
    DLLElementPtr newItem = malloc(sizeof(*newItem));
    
    if (newItem == NULL) {
        DLL_Error();
        return;
    }

    newItem->data = data;
    newItem->nextElement = NULL;    //nextElement bude NULL v oboch pripadoch

    // Vkladanie do prazdneho DLL
    if (list->lastElement == NULL) {
        newItem->previousElement = NULL;

        list->firstElement = list->lastElement = newItem;
    } else { // Vkladanie do neprazdneho DLL
        newItem->previousElement = list->lastElement;

        list->lastElement->nextElement = newItem;
        list->lastElement = newItem;
    }
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz (nepočítáme-li return),
 * aniž byste testovali, zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
    list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz (nepočítáme-li return),
 * aniž byste testovali, zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
    list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, int *dataPtr ) {
    if (list->firstElement == NULL) {
        DLL_Error();
    } else {
        *dataPtr = list->firstElement->data;
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, int *dataPtr ) {
    if (list->lastElement == NULL) {
        DLL_Error();
    } else {
        *dataPtr = list->lastElement->data;
    }
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
    if (list->firstElement == NULL)
        return;

    // Pripad, ze v DLL je len jeden prvok
    if (list->firstElement->nextElement == NULL) {
        free(list->firstElement);
        
        list->activeElement = NULL;
        list->firstElement = NULL;
        list->lastElement = NULL;
    } else { // V DLL je viac nez 1 prvok
        // Kontrola aktivity prveho prvku
        if (list->activeElement == list->firstElement) {
            list->activeElement = NULL;
        }

        list->firstElement = list->firstElement->nextElement;
        free(list->firstElement->previousElement);
        list->firstElement->previousElement = NULL;
    }
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
    if (list->lastElement == NULL)
        return;

    // V DLL je len 1 prvok
    if (list->lastElement->previousElement == NULL) {
        free(list->lastElement);
        
        list->activeElement = NULL;
        list->firstElement = NULL;
        list->lastElement = NULL;
    } else { // V DLL je viac nez 1 prvok
        // Kontrola aktivity poslendeho prvku
        if (list->activeElement == list->lastElement) {
            list->activeElement = NULL;
        }

        list->lastElement = list->lastElement->previousElement;
        free(list->lastElement->nextElement);
        list->lastElement->nextElement = NULL;
    } 
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
    if (list->activeElement == NULL || 
        list->activeElement == list->lastElement)
        return;

    // Nastav novy next element aktivneho prvku (preskoc vyhodeny prvok)
    DLLElementPtr afterActive = list->activeElement->nextElement;

    // Mazeme posledny prvok DLL
    if (afterActive == list->lastElement) {
        free(afterActive);  // Vymaz prvok po aktivnom prvku

        list->lastElement = list->activeElement;    // Aktivny prvok sa stava poslednym prvkom DLL
        list->activeElement->nextElement = NULL;    // Aktivny prvok nema za sebou dalsi prvok
    } else {
        afterActive = afterActive->nextElement;     // Preskoc prvok ktory chcem vyhodit
        free(afterActive->previousElement);         // Vymaz prvok, ktory mam vymazat

        afterActive->previousElement = list->activeElement;     // Prepoj vedlajsie prvky okolo vymazaneho prvku
        list->activeElement->nextElement = afterActive;
    }
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
    if (list->activeElement == NULL || 
        list->activeElement == list->firstElement)
        return;

    DLLElementPtr beforeActive = list->activeElement->previousElement;
    
    // Mazem prvy element DLL
    if (beforeActive == list->firstElement) {
        free(beforeActive);
        list->activeElement->previousElement = NULL;
        list->firstElement = list->activeElement;
    } else {
        beforeActive = beforeActive->previousElement;
        free(beforeActive->nextElement);
        beforeActive->nextElement = list->activeElement;
        list->activeElement->previousElement = beforeActive;
    }
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, int data ) {
    if (list->activeElement == NULL)
        return;

    DLLElementPtr newItem = malloc(sizeof(*newItem));
    
    if (newItem == NULL) {
        DLL_Error();
        return;
    }

    newItem->data = data;

    // Vkladam prvok za posledny prvok zoznamu
    if (list->activeElement == list->lastElement) {
        newItem->nextElement = NULL;
        list->lastElement = newItem;
    } else {
        newItem->nextElement = list->activeElement->nextElement;
        newItem->nextElement->previousElement = newItem;   
    }

    list->activeElement->nextElement = newItem;
    newItem->previousElement = list->activeElement;
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, int data ) {
    if (list->activeElement == NULL)
        return;

    DLLElementPtr newItem = malloc(sizeof(*newItem));
    
    if (newItem == NULL) {
        DLL_Error();
        return;
    }
    
    newItem->data = data;

    // Vkladanie pred prvy prvok zoznamu
    if (list->activeElement == list->firstElement) {
        newItem->previousElement = NULL;
        newItem->nextElement = list->activeElement;
        list->activeElement->previousElement = newItem;
        list->firstElement = newItem;
    } else {
        newItem->nextElement = list->activeElement;
        newItem->previousElement = list->activeElement->previousElement;
        list->activeElement->previousElement = newItem;
        newItem->previousElement->nextElement = newItem;
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, int *dataPtr ) {
    if (list->activeElement == NULL) {
        DLL_Error();
    } else {
        *dataPtr = list->activeElement->data;
    }
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, int data ) {
    if (list->activeElement == NULL)
        return;
    
    list->activeElement->data = data;
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
    if (list->activeElement == NULL)
        return;

    list->activeElement = list->activeElement->nextElement;
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
    if (list->activeElement == NULL)
        return;

    list->activeElement = list->activeElement->previousElement;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive( DLList *list ) {
    return list->activeElement != NULL;
}

/* Konec c206.c */
