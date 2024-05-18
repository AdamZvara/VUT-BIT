/**
 * @brief Decoding ppm image, task B) - IJC-DU1
 * @file steg-decode.c
 * @author Adam Zvara - xzvara01, FIT
 * @date 7.3.2021
 * @details Compiled with GCC 9.3.0 tested on Ubuntu 20.04.1
 */

#include <stdbool.h>
#include "ppm.h"
#include "eratosthenes.h"

#define START 23
#define MESSAGE_LENGTH 100

void free_all(struct ppm* image, bitset_t bset, char * message)
{
    bitset_free(bset);
    ppm_free(image);
    free(message);
}

void message_resize(char **message, int size, struct ppm *image, bitset_t bset)
{
    char *test = (char *)realloc(*message, size);
    if (test == NULL)
    {
        free_all(image, bset, *message);
        error_exit("Chyba pri realokacii spravy\n");
    }
    *message = test;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error_exit("Usage: ./steg-decode image.ppm\n");
    }
    
    struct ppm *image = ppm_read(argv[1]);
    if (image == NULL) return 1;

    unsigned int bit_counter = 0,
                 character = 0,
                 character_count = 0,
                 msg_length = MESSAGE_LENGTH;

    bool end_bit = false;
        
    //creating bitset with primes numbers
    const long size = (image->xsize)*(image->ysize)*3;
    bitset_alloc(bset, size);
    Eratosthenes(bset);
    
    //allocating message
    char *message = (char *)malloc(msg_length);
    if (message == NULL)
    {
        free_all(image, bset, message);
        error_exit("Chyba pri alokacii spravy");
    }
    
    for (bitset_index_t i = START; i < bitset_size(bset); i++)
    {
        if (bitset_getbit(bset, i) == 0)
        {
            if ((image->data[i] & 1) == 1)
            {
                character |= (1 << bit_counter);
            }
            
            if (bit_counter == (CHAR_BIT-1)) // character is full
            {
                if ((character_count != 0) && (character_count % 10 == 0))
                {
                    msg_length += MESSAGE_LENGTH;
                    message_resize(&message, msg_length, image, bset);
                }

                message[character_count++] = character;
                
                if (character == '\0') 
                {
                    end_bit = true;
                    break;   
                }
                else 
                {
                    bit_counter = 0;
                    character = 0;
                    continue;
                }
            }
            bit_counter++;
        }
    }

    if (end_bit == false)
    {
        free_all(image, bset, message);
        error_exit("Sprava nebola spravne ukoncena\n");
    }
    else
    {
        printf("%s\n", message);
    }
    
    free_all(image, bset, message);
    return 0;
}
