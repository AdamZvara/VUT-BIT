/*
 * @brief Table destructor, task B) IJC - DU2
 * @file htab_free.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include <stdlib.h>
#include "htab_private.h"

void htab_free(htab_t *t)
{
    htab_clear(t);
    free(t);
}
