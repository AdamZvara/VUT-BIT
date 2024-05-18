/**
 * @brief Function for reading a single word from a file
 * @file io.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 6.4.2021
 */

#include "io.h"

int read_word(char *s, int max, FILE *f)
{
    if (f == NULL)
    {
        fprintf(stderr, "Subor sa nepodarilo precitat\n");
    }

    int c;
    int length = 0;
    bool limit_reached = false;

    //skip all whitespaces from the point of reading
    while ((c = fgetc(f)) != EOF && isspace(c)) ;
    
    if (c == EOF) return EOF;

    //the first char is already in c
    s[length++] = c;

    while ((c = fgetc(f)) != EOF && !isspace(c))
    {
        s[length] = c;
        length++;
        
        if (length == max-1)
        {
            limit_reached = true;
            break;
        }
    }
    s[length] = '\0';

    if (limit_reached && !isspace(fgetc(f)))
    {
        //skip the rest of the word
        while ((c = fgetc(f)) != EOF && !isspace(c)) ;
        return 0;
    }

    return length;
}
