/**
 * @brief Function for parsing arguments in main file [proj2.c]
 * @file parse_args.c
 * @author Adam Zvara, xzvara01
 * @date 22.4.2021
 */

#include "parse_args.h"
#include "proj2.h"

int parse_arguments(int argc, char *argv[], args_t *args)
{
    if (argc != 5)
    {
        return 1;
    }

    long int num;
    char *endPtr = "";

    for(int i = 1; i < 5; i++)
    {
        num = strtol(argv[i], &endPtr, 10);

        if (strncmp(endPtr, "", 1L)) return 1;
        
        switch (i){
            case 1:
                if (num > 0  && num < MAX_ELVES)
                    args->NE = num;
                else return 1;
                break;
            case 2:
                if (num > 0  && num < MAX_RAIND)
                    args->NR = num;
                else return 1;
                break;
            case 3:
                if (num >= 0  && num <= MAX_ELF_T)
                    args->TE = num;
                else return 1;
                break;
            case 4:
                if (num >= 0  && num <= MAX_REIND_T)
                    args->TR = num;
                else return 1;
                break;
        }
    }
    return 0;
}

void usage(void)
{
    printf("usage:\n");
    printf("./proj2 NE NR TE TR\n"); 
    printf("\tNE - number of elves\n\t\t0 < NE < 1000\n"); 
    printf("\tNR - number of reindeers\n\t\t0 < NR < 20\n");  
    printf("\tTE - max time after in which elf is working alone\n"); 
    printf("\t\t0 <= TR <= 1000\n");
    printf("\tTR - max time after which reindeers return home\n"); 
    printf("\t\t0 <= TR <= 1000\n");
}

