/**
 * @brief Apply function to each record in hash table
 * @file htab_for_each.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 5.4.2021
 */

#include "htab_private.h"

void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data))
{
    for (size_t i = 0; i < t->arr_size; i++)
    {
        htab_item_t *tmp = t->arr[i];
        while (tmp != NULL)
        {
            (*f)(&tmp->pair);
            tmp = tmp->next;
        }
    }
}
