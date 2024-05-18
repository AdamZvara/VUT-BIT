/* ******************** s016-advanced-test.c *********************** */
/*  předmět: Algoritmy (IAL) - FIT VUT Brno                          */
/*  úkol: c202 - Tabulka s rozptýlenými položkami                    */
/*  vytvořil: Václav Topinka                                         */
/*  datum: září 2005                                                 */
/*  modifikace: 2013 Karel Masarik                                   */
/*  kódování: iso-8859-2                                             */
/* ***************************************************************** */

/* Testy pro domácí úkol na téma zásobník znaků (c202.c).
Obsahuje procedury a funkce, které testují studentské verze jejich voláním.
Toto je rozšířená verze, obsahující komplexní testování.
Slouží pro lektory ke kontrole příslušné domácí úlohy. */

#include "hashtable.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


ht_item_t* UNDEFPTR;
ht_table_t* ptrht;

extern int solved;

/* tiskne float ze struktury */
void htPrintData ( float* ptrdata ) {
	if ( ptrdata )
		printf ("%.2f\n", *ptrdata );
	else
		printf ("NULL\n");
}

/* tiskne tItem ze struktury */
void htPrintItem ( ht_item_t* ptritem ) {
	if ( ptritem )
		printf ("%s - %.2f\n", ptritem->key, ptritem->value );
	else
		printf ("NULL\n");
}

/* tiskne celou tabulku */
void htPrintTable( ht_table_t* ptrht ) {
	int maxlen = 0;
	int sumcnt = 0;
	int sumcnt2 = 0;
	int n = 0;

	printf ("------------HASH TABLE--------------\n");
	for ( int i=0; i<HT_SIZE; i++ ) {
		printf ("%i:",i);
		int cnt = 0;
		ht_item_t* ptr = (*ptrht)[i];
		while ( ptr != NULL ) {
			printf (" (%s,%.2f)",ptr->key,ptr->value);
			if ( ptr != UNDEFPTR )
				cnt++;
			ptr = ptr->next;
		}
		printf ("\n");

		if (cnt > maxlen)
			maxlen = cnt;
		n += cnt;
		sumcnt += cnt;
		sumcnt2 += cnt*cnt;
	}

	printf ("------------------------------------\n");
	printf ("Items count %i   The longest list %i\n",sumcnt,maxlen);
	if ( n < 2 )
		n = 2;
	printf ("Smerodatna odchylka: %f\n",sqrt((sumcnt2-((float)(sumcnt*sumcnt)/n))/(n-1)));
	printf ("------------------------------------\n");
}

/* vola funkci htInit a v pripade ze neni resena tiskne chybu */
void use_init ( ht_table_t* ptrht ) {
	solved = 1;
	ht_init( ptrht );
	if ( ! solved )
		printf("[W] Funkce HTInit nebyla implementovana...\n");
}

/* vola funkci htSearch a tiskne vysledek,
 v pripade ze neni resena tiskne chybu */
void use_search( ht_table_t* ptrht, char* key ) {
	solved = 1;
	ht_item_t* ptritem = ht_search ( ptrht, key );
	if ( ! solved )
		printf("[W] Funkce HTSearch nebyla implementovana...\n");
	else
		htPrintItem ( ptritem );
}

/* vola funkci htInsert a v pripade ze neni resena tiskne chybu */
void use_insert ( ht_table_t* ptrht, char* key, float value ) {
	solved = 1;
	ht_insert( ptrht, key, value);
	if ( ! solved )
		printf("[W] Funkce HTInsert nebyla implementovana...\n");
}

/* vola funkci htRead a tiskne vysledek,
 v pripade ze neni resena tiskne chybu */
void use_read( ht_table_t* ptrht, char* key ) {
	solved = 1;
	float* ptrdata = ht_get ( ptrht, key );
	if ( ! solved )
		printf("[W] Funkce HTSearch nebyla implementovana...\n");
	else
		htPrintData ( ptrdata );
}

/* vola funkci htDelete a v pripade ze neni resena tiskne chybu */
void use_delete ( ht_table_t* ptrht, char* key ) {
	solved = 1;
	ht_delete( ptrht, key );
	if ( ! solved )
		printf("[W] Funkce HTDelete nebyla implementovana...\n");
}

/* vola funkci htClearAll a v pripade ze neni resena tiskne chybu */
void use_clear_all ( ht_table_t* ptrht  ) {
	solved = 1;
	ht_delete_all( ptrht );
	if ( ! solved )
		printf("[W] Funkce HTClearAll nebyla implementovana...\n");
}

/* Hlavni funkce pro testovani */
int main(int argc, char* argv[] ) {
	UNDEFPTR = (ht_item_t*) malloc ( sizeof(ht_table_t) );
	UNDEFPTR->key = "*UNDEF*";
	UNDEFPTR->value = -1;
	UNDEFPTR->next = NULL;
	ptrht = (ht_table_t*) malloc ( sizeof(ht_table_t) );
	for ( int i=0; i<MAX_HT_SIZE; (*ptrht)[i++] = UNDEFPTR );

/********************* SCRIPT START ******************************************/
	printf ("Hash Table - testing script\n");
	printf ("---------------------------\n");
	printf ("\nLet's set HTSIZE to 19 ( must be prvocislo )\n");
	HT_SIZE = 19;
	htPrintTable(ptrht);


	printf ("\n[TEST01] Table initialization\n");
	use_init ( ptrht );
	htPrintTable(ptrht);

	printf ("\n[TEST02] Search for non-existing item\n");
	use_search( ptrht, "rohlik" );

	printf ("\n[TEST03] Let's try htInsert()\n");
	use_insert ( ptrht, "rohlik", 1.50 );
	htPrintTable(ptrht);

	printf ("\n[TEST04] Search for existing item\n");
	use_search (ptrht,"rohlik");

	printf ("\n[TEST05] Let's insert next items\n");
	use_insert ( ptrht, "chleba", 20.00);
	use_insert ( ptrht, "pepsi", 30.00);
	use_insert ( ptrht, "kecup", 28.90);
	use_insert ( ptrht, "spagety", 14.60);
	use_insert ( ptrht, "debrecinka", 19.60);
	use_insert ( ptrht, "ryze", 26.10);
	use_insert ( ptrht, "vajicko", 2.50);
	use_insert ( ptrht, "olej", 32.50);
	use_insert ( ptrht, "", 14.60);
	use_insert ( ptrht, "jogurt", 8.50);
	use_insert ( ptrht, "fish", 99.90);
	use_insert ( ptrht, "korgon", 18.00 );
	use_insert ( ptrht, "zlaty bazant", 20.00);
	use_insert ( ptrht, "gambrinus", 18.50);
	use_insert ( ptrht, "starobrno", 16.50);
	use_insert ( ptrht, "plzen", 25.00);
	use_insert ( ptrht, "velvet", 26.00);
	use_insert ( ptrht, "kelt", 30.00);
	use_insert ( ptrht, "kofola", 15.00);
	use_insert ( ptrht, "graham", 4.50);
	use_insert ( ptrht, "fernet", 95.00);
	use_insert ( ptrht, "sul", 10.00);
	use_insert ( ptrht, "cukr", 25.50);
	htPrintTable(ptrht);

	printf ("\n[TEST06] htSearch for \"kecup\"\n");
	use_search ( ptrht, "kecup" );

	printf ("\n[TEST07] Let's change kecup price to 12.50\n");
	use_insert ( ptrht, "kecup", 12.50);
	use_search ( ptrht, "kecup" );

	printf ("\n[TEST08] Is \"kiwi\" in table?\n");
	use_search ( ptrht, "kiwi" );
	htPrintTable(ptrht);

	printf ("\n[TEST09] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST10] Change fish price to 105.50\n" );
	use_insert ( ptrht, "fish", 105.50 );
	use_search (ptrht, "fish" );

	printf ("\n[TEST11] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST12] How much is kiwi?\n");
	use_read ( ptrht, "kiwi" );

	printf ("\n[TEST13] Let's delete non-existing \"kiwi\"\n");
	use_delete ( ptrht, "kiwi" );
	htPrintTable ( ptrht );

	printf ("\n[TEST14] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST15] Lets delete fish twice\n");
	use_delete( ptrht, "fish" );
	use_delete( ptrht, "fish" );
	htPrintTable ( ptrht );

	printf ("\n[TEST16] How much is the fish?\n");
	use_read ( ptrht, "fish" );

	printf ("\n[TEST17] Lets delete next items\n");
	use_delete ( ptrht, "zlaty bazant");
	use_delete ( ptrht, "gambrinus");
	use_delete ( ptrht, "starobrno");
	use_delete ( ptrht, "plzen");
	use_delete ( ptrht, "velvet");
	use_delete ( ptrht, "kelt");
	use_delete ( ptrht, "");
	htPrintTable ( ptrht );

	printf ("\n[TEST18] Let's delete whole table\n");
	use_clear_all ( ptrht );
	htPrintTable ( ptrht );

	printf ("\n[TEST19] And try delete in empty table\n");
	use_delete ( ptrht, "zlaty bazant");
	use_delete ( ptrht, "gambrinus");
	use_delete ( ptrht, "starobrno");
	htPrintTable ( ptrht );

	printf ("\n[TEST20] htSearch for \"kecup\" in empty table\n");
	use_search ( ptrht, "kecup" );
	htPrintTable ( ptrht );

	free (UNDEFPTR);
	free (ptrht);
	return 0;
}
