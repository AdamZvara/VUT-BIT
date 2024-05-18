/**
 * @brief Function to read word from a file
 * @file io.h
 * @author Adam Zvara, xzvara01 - FIT
 * @date 6.4.2021
 */

#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

/**
 * @brief Function that reads a single word from a file
 *
 * @param s Buffer to load the word into
 * @param max Maximum length of word
 * @param f File to read the word from
 *
 * @return Number of letters, that have been read
 * @return EOF If EndOfFile was reached
 *
 * @return 0 If the word was longer than max length 
 *      - if this happens, then the length of the word is max-1 
 */
int read_word(char *s, int max, FILE *f);

#endif
