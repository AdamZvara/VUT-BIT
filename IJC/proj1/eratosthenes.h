/**
 * @brief Eratosthenes header file, task A) - IJC-DU1
 * @file eratosthenes.h
 * @author Adam Zvara - xzvara01, FIT
 * @date 6.3.2021
 * @details Compiled with gcc 9.3.0 - Ubuntu 20.04.1 
 */

#include "bitset.h"

#ifndef ERATOSTHENES_H
#define ERATOSTHENES_H

/**
 * @brief Calculating prime numbers with bitset
 * @param name Name of the bitset
 * @details Using eratosthenes seive to calculate primes numbers in bitset. Indexes in bitset 
 * are representing numbers. For every bit which value is 0, we set all the bits which indexes 
 * are miltiples of the original bit to 1. We continue untill we reach sqrt(max_number). In the 
 * end, the indexes of bits that contain 0 are prime numbers
 */
void Eratosthenes(bitset_t bset);

#endif
