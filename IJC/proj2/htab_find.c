/*
 * @brief Find record in a table, task B) IJC - DU2
 * @file htab_find.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include <stdlib.h>
#include "htab_private.h"

htab_pair_t *htab_find(htab_t *t, htab_key_t key)
{
    size_t index = htab_hash_function(key);
    index %= t->arr_size;

    if (t->arr[index] == NULL) return NULL;
 
    for (htab_item_t *tmp = t->arr[index]; tmp!= NULL; tmp = tmp->next)
    {
        if (strlen(tmp->pair.key) == strlen(key) && 
            !strncmp(tmp->pair.key, key, strlen(tmp->pair.key)))
        {
            return &tmp->pair;
        }
    }

    return NULL;   
}
