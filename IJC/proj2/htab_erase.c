/*
 * @brief Delete record from table, task B) IJC - DU2
 * @file htab_erase.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include <stdlib.h>
#include "htab_private.h"

bool htab_erase(htab_t *t, htab_key_t key)
{
    size_t index = htab_hash_function(key);
    index %= t->arr_size;

    if (htab_find(t, key) == NULL) return false;

    htab_item_t *tmp = t->arr[index];
    if (strlen(tmp->pair.key) == strlen(key) && 
        !strncmp(tmp->pair.key, key, strlen(tmp->pair.key)))
    {
        if (tmp->next == NULL)
        {
            free((char *)tmp->pair.key);
            free(tmp);
            t->arr[index] = NULL;
        } else
        {
            htab_item_t *tmp2 = tmp->next;
            free((char *)tmp->pair.key);
            free(tmp);
            t->arr[index] = tmp2;
        }

        t->size--;
        return true;
    }

    for (tmp = t->arr[index]; tmp->next != NULL; tmp = tmp->next)
    {
        if (strlen(tmp->next->pair.key) == strlen(key) && 
            !strncmp(tmp->next->pair.key, key, strlen(tmp->next->pair.key)))
        {
            htab_item_t *tmp2 = tmp->next->next;
            free((char *)tmp->next->pair.key);
            free(tmp->next);
            tmp->next = tmp2;
            
        }
    }

    t->size--;
    return true;
}
