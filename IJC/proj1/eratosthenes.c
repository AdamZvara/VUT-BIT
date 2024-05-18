/**
 * @brief Implementation of eratosthenes function, task A) - IJC-DU1 
 * @file eratosthenes.c
 * @author Adam Zvara - xzvara01, FIT
 * @date 6.3.2021
 * @details Compiled with gcc 9.3.0 tested on Ubuntu 20.04.1 
 */

#include <math.h>
#include "eratosthenes.h"

void Eratosthenes(bitset_t bset)
{
    const unsigned long length = bitset_size(bset);

    bitset_setbit(bset, 0, 1);
    bitset_setbit(bset, 1, 1);
    
    const double max = sqrt(length);

    for (bitset_index_t i = 2; i < max; i++)
    {
        if (bitset_getbit(bset, i) == 0)
        {
            for (bitset_index_t j = 2*i; j < length; j+=i)
            {
                bitset_setbit(bset, j, 1);
            }
        }
    }
}
