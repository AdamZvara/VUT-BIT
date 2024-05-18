/**
 * @brief Processing ppm image into structure
 * @file ppm.c
 * @author Adam Zvara - xzvara01, FIT
 * @date 7.3.2021
 * @details Compiled with gcc 9.3.0, tested on Ubuntu 20.04.1 
 */

#include <ctype.h>
#include "ppm.h"

#define MAX_SIZE 8000 //</Maximum witdh and height of picture

/**
 * @brief Reading from .ppm file image
 * @param filename Name of file to open
 * @return ppm* Pointer to a dynamically allocated structure
 */
struct ppm * ppm_read(const char* filename)
{
    FILE *fr = fopen(filename, "rb");

    if (!fr)
    {
        warning_msg("Subor sa nepodarilo otvorit\n");
        return NULL;
    }

    int xsize, ysize = 0;

    if ((fscanf(fr, "P6 %d %d 255", &xsize, &ysize) != 2) || !(isspace(fgetc(fr))))
    {
        warning_msg("Nepodporovany format\n");
        fclose(fr);
        return NULL;
    }
    
    if ((xsize <= 0) || (ysize <= 0) || (xsize > MAX_SIZE) || (ysize > MAX_SIZE))
    {
        warning_msg("Neplatna velkost obrazku\n");
        fclose(fr);
        return NULL;
    }
    
    struct ppm *image = malloc(sizeof(struct ppm)+xsize*ysize*3);
    if (image == NULL)
    {
        warning_msg("Alokacia struktury neprebehla spravne\n");
        fclose(fr);
        return NULL;
    }

    image->xsize = xsize;
    image->ysize = ysize;

    int c, i = 0;
    while (((c = fgetc(fr)) != EOF) && (i < xsize*ysize*3))
    {
        image->data[i++] = c;
    }

    fclose(fr);
    return image;
}

/**
 * @brief Free previosly allocated ppm structure
 * @param ppm* Dynamically allocated ppm structure
 */
void ppm_free(struct ppm *p)
{
    free(p);
}
