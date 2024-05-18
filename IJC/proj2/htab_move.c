/*
 * @brief Move contructor, task B) IJC - DU2
 * @file htab_move.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include "htab_private.h"

htab_t *htab_move(size_t n, htab_t *from)
{
    htab_t *table = htab_init(n);
    if (table == NULL) return NULL;

    table->size = from->size;

    for (size_t i = 0; i < from->arr_size; i++)
    {
        htab_item_t *tmp;

        while ((tmp = from->arr[i]) != NULL)
        {
            from->arr[i] = tmp->next;
            tmp->next = NULL;

            size_t new_index = htab_hash_function(tmp->pair.key);
            new_index %= table->arr_size;

            if (table->arr[new_index] == NULL) 
            {
                table->arr[new_index] = tmp;
            }
            else
            {
                for (htab_item_t *tmp2 = table->arr[new_index]; tmp2 != NULL; tmp2 = tmp2->next)
                {
                    if (tmp2->next == NULL)
                    {
                        tmp2->next = tmp;
                        break;
                    }
                }
            }
        }
    }

    return table;
}
