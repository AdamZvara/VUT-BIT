/*
 * @brief Get array size in table, task B) IJC - DU2
 * @file htab_bucket_count.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include "htab_private.h"

size_t htab_bucket_count(const htab_t *t)
{
    return t->arr_size;
}
