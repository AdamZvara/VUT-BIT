/**
 * @brief Create private structure (hash table) for library
 * @file htab_private.h
 * @author Adam Zvara
 * @date 4.4.2021
 */

#include "htab.h"

/* HTAB structure where size = 3, arr_size = 2
  +----------+
  |   size   | number of records [key,data),next]
  +----------+
  | arr_size | size of the following array 
  +----------+
  |ptr|-->[(key,data),next]-->[(key,data),next]-->[(key,data),next]--|
  +---+
  |ptr|--|
  +---+
*/

typedef struct htab_item htab_item_t;

/**
 * @struct htab_item
 * @brief Private declaration of hash table item (linked list)
*/
struct htab_item {
    htab_pair_t pair;           ///<Item pair containing word and its counter
    htab_item_t *next;     ///<Pointer to next table item
};


/**
 * @struct htab
 * @brief Private declaration of hash table structure for library
*/
struct htab {
    size_t size;                 ///<Number of records from hash table
    size_t arr_size;             ///<Number of pointers in arr
    struct htab_item *arr[];     ///<Array of pointers
};

