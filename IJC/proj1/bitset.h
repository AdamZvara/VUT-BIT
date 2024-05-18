/**
 * @brief Implementation of bitset, task A) - IJC-DU1
 * @file bitset.h
 * @author Adam Zvara - xzvara01, FIT
 * @date 9.3.2021
 * @details Bits are indexed from 0 to size of the bitset -1. Compiled with gcc 9.3.0, tested on    Ubuntu 20.04.1
 */
    
#ifndef BITSET_H
#define BITSET_H

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "error.h"

typedef unsigned long* bitset_t;
typedef unsigned long bitset_index_t;

#define ITEM_SIZE (sizeof(unsigned long)*CHAR_BIT) //</Size of a single item in bitset
#define MAX_LEN 200000000 //</Maximum range of dynamically allocated bitset

/**
 * @brief Initialize bitset, first value is set to the size of bitset in bits
 * @param name Name of a new bitset
 * @param size Size of bitset in bits
 * @details Only works with whole numbers (int, long, unsigned long ..)
 */
#define bitset_create(name, size)\
    unsigned long name[\
        (((size)%(ITEM_SIZE)) ? ((size)/(ITEM_SIZE)+2) : ((size)/(ITEM_SIZE)+1))] = {size};\
    _Static_assert(((size) > 0), "Velkost pola musi byt vacsia nez 0")

/**
* @brief Initialize bitset dynamically, first value is set to the size of bitset in bits
* @param name Name of a new bitset
* @param size Size of bitset in bits
* @details Only works with whole numbers (int, long, unsigned long ..)
*/
#define bitset_alloc(name, size)\
    assert((size) > 0);\
    assert((size) <= MAX_LEN);\
    unsigned long *name = calloc(\
        (((size)%(ITEM_SIZE)) ? ((size)/(ITEM_SIZE)+2) : ((size)/(ITEM_SIZE+1))), sizeof(unsigned long));\
    ((name == NULL) ? \
        (error_exit("bitset_alloc: Chyba alokace pameti\n"),0) : \
        (name[0] = size));

/**
 * @brief Set a single bit in bitset to a given value
 * @param name Bitset name
 * @param index Index of the bit to change
 * @param value Binary value to insert
 */ 
#define setbit(name, index, value)\
    ((value != 0) ? (name[(index)/(ITEM_SIZE)+1] |= 1L << ((index)%(ITEM_SIZE)))\
                  : (name[(index)/(ITEM_SIZE)+1] &= ~(1L << ((index)%(ITEM_SIZE)))))

/**
 * @brief Get a single bit value from bitset at given index
 * @param name Bitset name
 * @param index Index of the source bit
 */ 
#define getbit(name, index)\
    ((name[(index)/(ITEM_SIZE)+1] >> ((index)%(ITEM_SIZE))) & 1L)



//Inline functions are similiar to macros defined underneath them
#ifdef USE_INLINE
    inline void bitset_free(bitset_t name)
    {
        free(name);
    }

    inline unsigned long bitset_size(bitset_t name)
    {
        return name[0];
    } 

    inline void bitset_setbit(bitset_t name, bitset_index_t index, int value)
    { 
        if (index >= bitset_size(name))
        {
            error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n", \
             (unsigned long)index, (unsigned long)bitset_size(name)-1);
        } 
        else 
        {
            setbit(name, index, value); 
        }
    }

    
    inline unsigned long bitset_getbit(bitset_t name, bitset_index_t index)
    {
        if (index >= bitset_size(name))
        {
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", \
             (unsigned long)index, (unsigned long)bitset_size(name)-1);
            return -1;
        } 
        else 
        {
            return (getbit(name, index)); 
        }
    }

#else

    /**
     * @brief Frees previously allocated bitset
     * @param name Name of the bitset
     */
    #define bitset_free(name) free(name)
    
    /**
     * @brief Returns size of bitset in bits
     * @param name Name of bitset
     */
    #define bitset_size(name) name[0]
    
    /**
     * @brief Improved setbit macro with boundary values detection
     * @param name Name of the bitset
     * @param index Index of the bit
     * @param value Binary value to insert
     * @details Index is converted to unsigned long
     */ 
    #define bitset_setbit(name, index, value)\
        (((unsigned long)(index) >= bitset_size(name)) ? \
         (error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n", \
             (unsigned long)(index), (unsigned long)bitset_size(name)-1), 1) : \
         (setbit(name, index, value)))
    
    /**
     * @brief Improved getbit macro with boundary values detection
     * @param name Name of the bitset
     * @param index Index of source bit
     * @details Index is converted to unsigned long
     */
    #define bitset_getbit(name, index)\
        (((unsigned long)(index) >= bitset_size(name)) ? \
         (error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", \
             (unsigned long)(index), (unsigned long)bitset_size(name)-1), 1) : \
         (getbit(name, index)))
#endif

#endif
