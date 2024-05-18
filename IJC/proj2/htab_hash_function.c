/*
 * @brief Implementation of has function for htab, task B) IJC - DU2
 * @file htab_hash_function.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 4.4.2021
 */

#include <stdint.h>
#include "htab.h"

size_t htab_hash_function(htab_key_t str)
{
    uint32_t h=0;
    const unsigned char *p;
        for(p=(const unsigned char*)str; *p!='\0'; p++)
        {
            h = 65599*h + *p;
        }
    return h;
}
