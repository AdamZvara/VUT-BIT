/**
 * @brief Extern declaration of inline functions (CFLAGS=-O0), task A) - IJC-DU1
 * @file bitset.c
 * @author Adam Zvara - xzvara01, FIT
 * @date 6.3.2021
 * @details Compiled with gcc 9.3.0 tested on Ubuntu 20.04.1 
 */

#include "bitset.h"

extern inline void bitset_free(bitset_t name);
extern inline unsigned long bitset_size(bitset_t name);
extern inline void bitset_setbit(bitset_t name, bitset_index_t index, int value);
extern inline unsigned long bitset_getbit(bitset_t name, bitset_index_t index);

