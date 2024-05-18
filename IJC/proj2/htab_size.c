/*
 * @brief Get number of records in table, task B) IJC - DU2
 * @file htab_size.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include "htab_private.h"

size_t htab_size(const htab_t *t)
{
    return t->size;
}
