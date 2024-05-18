/**
 * @brief Counting number of words in a text
 * @file wordcount.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 7.4.2021
 * @details Program can take 1 argument (filename) or reads from stdin
 */

#include <stdio.h>
#include <stdlib.h>
#include "htab.h"
#include "io.h"

//calculated by the number of words in vocabulary of an average person 
//(lower value close to 20000)
//data taken from site https://wordcounter.io/blog/how-many-words-does-the-average-person-know/
//added to a prime number 
#define HTAB_SIZE 20011
#define MAX_WORD_LEN 127


/**
 * @brief Function to write out each word and value from hash table
 *
 * @param data Pointer to htab pair structure from HT
 */
void write_pair(htab_pair_t *data)
{
    printf("%s %d\n", data->key, data->value);
}

int main(int argc, char *argv[])
{
    FILE *fr = stdin;

    if (argc == 2)
    {
        fr = fopen(argv[1], "r");
        if (fr == NULL) 
        {
            fprintf(stderr, "Chyba pri otvarani suboru\n");
            return 1;
        }
    }

    bool limit_reached = false;
    int length;
    
    htab_t *table = htab_init(HTAB_SIZE);
    if (table == NULL)
    {
        fprintf(stderr, "Alokacia tabulky neprebehla uspesne\n");
        fclose(fr);
        return 1;
    }

    char *word = malloc(MAX_WORD_LEN);
    if (word == NULL)
    {
        htab_free(table);
        fprintf(stderr, "Alokacia slova neprebehla uspesne\n");
        fclose(fr);
        return 1;
    }

    htab_pair_t *pair;
    while ((length = read_word(word, MAX_WORD_LEN, fr)) != EOF)
    {
        if (length == 0 && !limit_reached)
        {
            limit_reached = true;
            fprintf(stderr, "Text obsahuje prilis dlhe slova ktore budu skratene\n");
        }
        
        pair = htab_lookup_add(table, word);
        if (pair == NULL) 
        {
            htab_free(table);
            free(word);
            fprintf(stderr, "Alokacia prvku tabulky neprebehla uspesne\n");
            fclose(fr);
            return -1;
        }

        pair->value++;
    }

    htab_for_each(table, (*write_pair));

    free(word);
    htab_free(table);
    fclose(fr);
    return 0;
}
