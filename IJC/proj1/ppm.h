/**
 * @brief Header file for ppm.c, task B) - IJC-DU1
 * @file ppm.h
 * @author Adam Zvara - xzvara01, FIT
 * @date 7.3.2021
 * @details Compiled with gcc 9.3.0 - Ubuntu 20.04.1 
 */

#include "error.h"

#ifndef PPM_H
#define PPM_H

/**
 * @brief Structure to store data from ppm file
 * @struct ppm
 */
struct ppm 
{
    unsigned xsize; /**< Width of image in pixels*/
    unsigned ysize; /**< Height of image in pixels*/
    char data[];    /**< Flexible array used to store RGB byte values from image*/
};

int ppm_get_size(FILE *fr, int *xsize, int *ysize);

/**
 * @brief Extract data from file to structure ppm
 * @param filename Name of the file with PPM format
 * @return ppm* Pointer to a dynamically allocated structure
 */
struct ppm * ppm_read(const char* filename);

/**
 * @brief Frees previously allocated ppm structure
 * @param ppm Pointer to the structure to free
 */
void ppm_free(struct ppm *p);

#endif
