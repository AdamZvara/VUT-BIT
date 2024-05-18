/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
  *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  bst_node_t *tmp = tree;
  while (tmp != NULL) {
    if (key == tmp->key) {
      *value = tmp->value;
      return true;
    }
    
    if (key < tmp->key) {
      tmp = tmp->left;
      continue;
    }
    
    if (key > tmp->key) {
      tmp = tmp->right;
    }
  }

  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  bst_node_t *tmp = *tree;
  if (tmp == NULL) {
    bst_node_t *new = malloc(sizeof(*new));
      if (new == NULL) 
        return;
      new->key = key;
      new->value = value;
      new->left = new->right = NULL;
      *tree = new;
  }

  while (tmp != NULL) {
    if (key == tmp->key) {  
      tmp->value = value;
      return;
    }
    
    if (key < tmp->key) {
      if (tmp->left != NULL) {
        tmp = tmp->left;
        continue;
      }
      bst_node_t *new = malloc(sizeof(*new));
      if (new == NULL) 
        return;
      new->key = key;
      new->value = value;
      new->left = new->right = NULL;
      tmp->left = new;
    }
    
    if (key > tmp->key) {
      if (tmp->right != NULL) {
        tmp = tmp->right;
        continue;
      }
      bst_node_t *new = malloc(sizeof(*new));
      if (new == NULL) 
          return;
        new->key = key;
        new->value = value;
        new->left = new->right = NULL;
        tmp->right = new;
    }
  } // end while
} // end function

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  bst_node_t *tmp = *tree;
  bst_node_t **tmp2 = &(target->left);
  // iteracia na rightmost prvok
  while (tmp->right != NULL) {
    tmp2 = &(tmp->right);
    tmp = tmp->right;
  } 
  target->key = tmp->key;
  target->value = tmp->value;
  if (tmp->left == NULL) {
    free(tmp);
    *tmp2 = NULL;
    return;
  }
  bst_node_t *delete = tmp;
  tmp = tmp->left;
  *tmp2 = tmp;
  free(delete);
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  bst_node_t *tmp = *tree;
  bst_node_t **test;
  while (tmp != NULL) {
    if (key < tmp->key) {
      test = &(tmp->left);
      tmp = tmp->left;
      continue;
    }
    if (key > tmp->key) {
      test = &(tmp->right);
      tmp = tmp->right;
      continue;
    }
    // v tomto bode som nasiel uzol ktory chcem vymazat
    if (tmp->right == NULL && tmp->left == NULL) {
      free(tmp);
      *test = NULL;
      return;
    }

    if (tmp->right == NULL && tmp->left != NULL) {
      bst_node_t *delete = tmp;
      tmp = tmp->left;
      free(delete);
      *test = tmp;
      return;
    }

    if (tmp->right != NULL && tmp->left == NULL) {
      bst_node_t *delete = tmp;
      tmp = tmp->right;
      free(delete);
      *test = tmp;
      return;
    }

    bst_replace_by_rightmost(tmp, &(tmp->left));
  }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
  if (*tree == NULL)
    return;

  stack_bst_t s;
  stack_bst_init(&s);
  do {
    if (*tree == NULL) {
      if (!stack_bst_empty(&s)) {
        *tree = stack_bst_top(&s);
        stack_bst_pop(&s);
      }
      continue;
    } 

    if ((*tree)->right != NULL) {
      stack_bst_push(&s, (*tree)->right);
    }
    bst_node_t *delete = *tree;
    *tree = (*tree)->left;
    free(delete);
  } while ((*tree != NULL) || (!stack_bst_empty(&s)));
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    bst_print_node(tree);
    tree = tree->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  stack_bst_t s;
  stack_bst_init(&s);
  bst_leftmost_preorder(tree, &s);
  while (!stack_bst_empty(&s)) {
    tree = stack_bst_top(&s);
    stack_bst_pop(&s);
    bst_leftmost_preorder(tree->right, &s);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  stack_bst_t s;
  stack_bst_init(&s);
  bst_leftmost_inorder(tree, &s);
  while (!stack_bst_empty(&s)) {
    tree = stack_bst_top(&s);
    stack_bst_pop(&s);
    bst_print_node(tree);
    bst_leftmost_inorder(tree->right, &s);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, TRUE);
    tree = tree->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  bool fromLeft;
  stack_bst_t s;
  stack_bool_t b;
  stack_bst_init(&s);
  stack_bool_init(&b);
  bst_leftmost_postorder(tree, &s, &b);
  while (!stack_bst_empty(&s)) {
    tree = stack_bst_top(&s);
    fromLeft = stack_bool_top(&b);
    stack_bool_pop(&b);
    if (fromLeft) {
      stack_bool_push(&b, FALSE);
      bst_leftmost_postorder(tree->right, &s, &b);
    } else {
      stack_bst_pop(&s);
      bst_print_node(tree);
    }
  }
}
