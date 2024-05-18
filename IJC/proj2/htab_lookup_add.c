/*
 * @brief Modified find function, task B) IJC - DU2
 * @file htab_lookup_add.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include <stdlib.h>
#include "htab_private.h"

/**
 * @brief Create new item to insert to HT
 * @details New item uses key passed from argument, value is set to 0
 * and pointer *next is NULL
 *
 * @param key String - word to insert into item
 *
 * @return Allocated item, if mallocs were successful
 * @return NULL, if any of the mallocs failed
 */
htab_item_t *item_create(htab_key_t key)
{
    htab_item_t *item = malloc(sizeof(htab_item_t));
    if (item == NULL) return NULL;

    size_t length = strlen(key)+1;
    item->pair.key = malloc(length);
    if (item->pair.key == NULL) 
    {
        free(item);
        return NULL;
    }
    memcpy((char *)item->pair.key, key, length);
    item->pair.value = 0; //or 1 depends on who's asking
    item->next = NULL;

    return item;
}

htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key)
{
    size_t index = htab_hash_function(key);
    index %= t->arr_size;
    
    htab_pair_t *pair = htab_find(t, key);
    if (pair != NULL) 
    {
        //pair->value++;
        return pair;
    }

    htab_item_t *new_item = item_create(key);
    if (new_item == NULL) return NULL;

    if (t->arr[index] == NULL) 
    {
        t->arr[index] = new_item;
        t->size++;
    }
    else
    {
        for (htab_item_t *tmp = t->arr[index]; tmp != NULL; tmp = tmp->next)
        {
            if (tmp->next == NULL)
            {
                tmp->next = new_item;
                t->size++;
                break;
            }
        }
    }
    return &(new_item->pair);
}
