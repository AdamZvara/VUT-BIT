/**/

/* **********************SOUBOR SE ST.TESTEM  ********************** */
/* **********************-----------------   *********************** */
/*  vytvoril: Martin Tucek
    predmet: Algoritmy (IAL) - FIT (Fakulta Informacnich Technologii)
    test pro soubor: c401.c
	datum: rijen 2005
    kompletni upravy: Roman Lukas, 2006
    upravil: Karel Masarik, rijen 2013
													 */
/* ***************************************************************** */


#include "../btree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bst_node_t* TempTree;			// deklarace testovaci promenne

int solved;

int Content_of_Search;
int Content_of_Insert;
char K;

void Print_tree2(bst_node_t* TempTree, char* sufix, char fromdir)
//vykresli sktrukturu binarniho stromu

{
     if (TempTree != NULL)
     {
	char* suf2 = (char*) malloc(strlen(sufix) + 4);
	strcpy(suf2, sufix);
        if (fromdir == 'L')
	{
	   suf2 = strcat(suf2, "  |");
	   printf("%s\n", suf2);
	}
	else
	   suf2 = strcat(suf2, "   ");
	Print_tree2(TempTree->right, suf2, 'R');
        printf("%s  +-[%c,%d]\n", sufix, TempTree->key, TempTree->value);
	strcpy(suf2, sufix);
        if (fromdir == 'R')
	   suf2 = strcat(suf2, "  |");
	else
	   suf2 = strcat(suf2, "   ");
	Print_tree2(TempTree->left, suf2, 'L');
	if (fromdir == 'R') printf("%s\n", suf2);
	free(suf2);
    }
}

void Print_tree(bst_node_t* TempTree)
{
  printf("Struktura binarniho stromu:\n");
  printf("\n");
  if (TempTree != NULL)
     Print_tree2(TempTree, "", 'X');
  else
     printf("strom je prazdny\n");
  printf("\n");
  printf("=================================================\n");
}

int test_BSTInit(bst_node_t* *TempTree)			{
	solved=TRUE;
	bst_init(TempTree);
	if (!solved)	{
		printf("Operace InitList() nebyla implementovana \n");
		return(FALSE);
	}
	else
	{
	        Print_tree(*TempTree);
		return(TRUE);
	}
}

int test_BSTSearch(bst_node_t* TempTree, char K, int *Content)	{
	solved=TRUE;
	int FOUND=FALSE;
	FOUND=bst_search(TempTree,K,Content);
	if (!solved)	{
		printf("Operace BSTSearch() nebyla implementovana \n");
		return(FALSE);
	}
	else	{
		if(!FOUND)
			printf("Polozka nebyla nalezena !\n");
		else {
			printf("Polozka byla nalezena !\n");
			printf("Polozka obsahuje hodnotu %d \n", *Content);
		}
		return(TRUE);
	}
}

int test_BSTInsert (bst_node_t** TempTree, char K, int Content)		{
	solved=TRUE;
	bst_insert(TempTree, K, Content);
	if (!solved)	{
		printf("Operace bst_insert() nebyla implementovana \n");
		return(FALSE);
	}
	else	{
		Print_tree(*TempTree);
		return(TRUE);
	}
}

int test_BSTDelete (bst_node_t** TempTree, char K)		{
	solved=TRUE;
	bst_delete(TempTree,K);
	if (!solved)	{
		printf("Operace BSTDelete() nebyla implementovana \n");
		return(FALSE);
	}
	else	{
		Print_tree(*TempTree);
		return(TRUE);
	}
}

int test_BSTDispose(bst_node_t** TempTree)		{
	solved=TRUE;
	bst_dispose(TempTree);
	if (!solved)	{
		printf("Operace BSTDispose() nebyla implementovana \n");
		return(FALSE);
	}
	else	{
		Print_tree(*TempTree);
		return(TRUE);
	}
}

/* .......................... sekce volani jednotlivych testu .............................*/

int main(int argc, char *argv[])			{

	printf("Binarni vyhledavaci strom\n");
	printf("=========================\n");

	printf("[TEST01]\n");
	printf("Test inicializace....\n");
	test_BSTInit(&TempTree);

	printf("[TEST02]\n");
	printf("Zkusime zrusit strom\n");
	printf("~~~~~~~~~~~~~~~~~~~~\n");
	test_BSTDispose(&TempTree);

	printf("[TEST03]\n");
	printf("Pokusime se vyhledat polozku s klicem 'A' -- nenalezne se\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'A';
	test_BSTSearch(TempTree,K,&Content_of_Search);

	printf("[TEST04]\n");
	printf("Vlozime prvni prvek (H,1)\n");
	K = 'H';
	Content_of_Insert=1;
	test_BSTInsert(&TempTree,K,Content_of_Insert);

	printf("[TEST05]\n");
	printf("Pokusime se vyhledat polozku s klicem H\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_BSTSearch(TempTree,K,&Content_of_Search);

	printf("[TEST06]\n");
	printf("Vlozime prvek (H,8) - pouze zmena hodnoty\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	Content_of_Insert=8;
	test_BSTInsert(&TempTree,K,Content_of_Insert);

	printf("[TEST07]\n");
	printf("Vlozime dalsi prvky a vytvorime tak symetricky binarni strom \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

	bst_insert(&TempTree,'D',4);
	bst_insert(&TempTree,'L',12);
	bst_insert(&TempTree,'B',2);
	bst_insert(&TempTree,'F',6);
	bst_insert(&TempTree,'J',10);
	bst_insert(&TempTree,'N',14);
	bst_insert(&TempTree,'A',1);
	bst_insert(&TempTree,'C',3);
	bst_insert(&TempTree,'E',5);
	bst_insert(&TempTree,'G',7);
	bst_insert(&TempTree,'I',9);
	bst_insert(&TempTree,'K',11);
	bst_insert(&TempTree,'M',13);
	bst_insert(&TempTree,'O',15);

	Print_tree(TempTree);

	printf("[TEST08]\n");
	printf("Pokusime se vyhledat polozky s klici 'A', 'B'\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K='A';
	test_BSTSearch(TempTree,K,&Content_of_Search);
	K='B';
	test_BSTSearch(TempTree,K,&Content_of_Search);

	printf("[TEST09]\n");
	printf("Pokusime se vyhledat polozky s klici 'X', 'Y'\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K='X';
	test_BSTSearch(TempTree,K,&Content_of_Search);
	K='Y';
	test_BSTSearch(TempTree,K,&Content_of_Search);

	printf("[TEST10]\n");
	printf("Pridame vhodne jeste dalsi prvky \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

	bst_insert(&TempTree,'S',10);
	bst_insert(&TempTree,'R',10);
	bst_insert(&TempTree,'Q',10);
	bst_insert(&TempTree,'P',10);
	bst_insert(&TempTree,'X',10);
	bst_insert(&TempTree,'Y',10);
	bst_insert(&TempTree,'Z',10);

	Print_tree(TempTree);

	printf("[TEST11]\n");
	printf("Zrusime listovy uzel (A,1)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'A';
	test_BSTDelete(&TempTree,K);

	printf("[TEST12]\n");
	printf("Zrusime uzel, ktery ma jen levy podstrom (R,10)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'R';
	test_BSTDelete(&TempTree, K);

	printf("[TEST13]\n");
	printf("Zrusime uzel, ktery ma jen pravy podstrom (X,10)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	K = 'X';
	test_BSTDelete(&TempTree, K);

	printf("[TEST14]\n");
	printf("Zrusime uzel, ktery ma oba podstromy (L,12)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'L';
	test_BSTDelete(&TempTree, K);

	printf("[TEST15]\n");
	printf("Pokusime se zrusit uzel, ktery neexistuje (U,?)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'U';
	test_BSTDelete(&TempTree, K);

	printf("[TEST16]\n");
	printf("Zrusime korenovy uzel (H,8)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'H';
	test_BSTDelete(&TempTree, K);

	printf("[TEST17]\n");
	printf("Zrusime dalsi uzel (K,11)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'K';
	test_BSTDelete(&TempTree, K);

	printf("[TEST18]\n");
	printf("Zrusime dalsi  uzel (D,4)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'D';
	test_BSTDelete(&TempTree, K);

	printf("[TEST19]\n");
	printf("Zrusime dalsi uzel (S,10)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'S';
	test_BSTDelete(&TempTree, K);

	printf("[TEST20]\n");
	printf("Nakonec zrusime cely strom\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_BSTDispose(&TempTree);

	printf("[TEST21]\n");
	printf("Vlozime dalsi prvky a vytvorime tak degradovany binarni strom \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

	bst_insert(&TempTree,'C',3);
	bst_insert(&TempTree,'D',4);
	bst_insert(&TempTree,'E',5);
	bst_insert(&TempTree,'F',6);
	bst_insert(&TempTree,'G',7);
	bst_insert(&TempTree,'H',8);
	bst_insert(&TempTree,'I',9);
	bst_insert(&TempTree,'J',10);

	Print_tree(TempTree);

	printf("[TEST22]\n");
	printf("Pokusime se vyhledat polozky s klici 'E', 'F'\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K='E';
	test_BSTSearch(TempTree,K,&Content_of_Search);
	K='F';
	test_BSTSearch(TempTree,K,&Content_of_Search);

	printf("[TEST23]\n");
	printf("Pokusime se vyhledat polozky s klici 'A', 'B'\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K='A';
	test_BSTSearch(TempTree,K,&Content_of_Search);
	K='B';
	test_BSTSearch(TempTree,K,&Content_of_Search);

	printf("[TEST24]\n");
	printf("Pridame vhodne jeste dalsi prvky \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");

	bst_insert(&TempTree,'A',1);
	bst_insert(&TempTree,'B',2);

	Print_tree(TempTree);

	printf("[TEST25]\n");
	printf("Zrusime uzel (A,1)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'A';
	test_BSTDelete(&TempTree,K);

	printf("[TEST26]\n");
	printf("Zrusime uzel (B,2)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'B';
	test_BSTDelete(&TempTree, K);

	printf("[TEST27]\n");
	printf("Zrusime uzel (J,10)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
	K = 'J';
	test_BSTDelete(&TempTree, K);

	printf("[TEST28]\n");
	printf("Pokusime se zrusit uzel, ktery neexistuje (U,?)\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	K = 'U';
	test_BSTDelete(&TempTree, K);

	printf("[TEST29]\n");
	printf("Nakonec zrusime cely strom\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_BSTDispose(&TempTree);

	printf("[TEST30]\n");
	printf("Nakonec zrusime cely strom\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	test_BSTDispose(&TempTree);

	printf("------------------------------ konec -------------------------------------\n");
	return(0);
}
/**/