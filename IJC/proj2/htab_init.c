/**
 * @brief Constructor of hash table, task B) IJC - DU2
 * @file htab_init.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include <stdlib.h>
#include "htab_private.h"

htab_t *htab_init(size_t n)
{
    if (n <= 0) return NULL;
    htab_t *hash_table = malloc(sizeof(htab_t) + n * sizeof(htab_item_t*));
    if (hash_table == NULL) return NULL;
    
    hash_table->size = 0;
    hash_table->arr_size = n;
 
    for (size_t i = 0; i < n; i++)
    {
        hash_table->arr[i] = NULL;
    }

    return hash_table;
}
