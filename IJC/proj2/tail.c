/*
 * @brief Program for printing last lines from a file, task A) - IJC-DU2
 * @file tail.c
 * @author Adam Zvara, xzvara01 - FIT
 * @date 29.3.2021
 * @details Program can take parameter -n followed by number of 
 * lines to print. The number of lines must be larger than 0
 * (from assignment)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Maximum length of single line - excluding /0
#define LINE_LEN 511

/*
 * @brief Function for parsing arguments
 *
 * @param lines_num Pointer to number of lines to print
 * @param filename Pointer to name of the file to open
 *
 * @return 1 -n parameter includes '+' at the beginning of the number
 * @return -1 -n parameter doesnt meet the requirements
 * @return -2 -n parameter wasnt followed by a number
 */
int arguments(int argc, char *argv[], int *lines_num, char **filename)
{
    bool plus = false;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i],"-n"))
        {
            //check if there is any argument after -n
            if (i == argc-1) return -2;

            //check if the argument starts with '+' sign
            if (argv[i+1][0] == '+') plus = true;
            
            *lines_num = atoi(argv[++i]);
            if (*lines_num <= 0)
            {
                return -1;
            }
        } 
        else 
        {
            *filename = argv[i];
        }
    }

    if (plus) return 1;
    return 0;
}

/*
 * @brief Function to handle opening a file
 * 
 * @param filename Name of the file to open
 *
 * @return fr Pointer to an opened file descriptor
 *
 * @details Implicit way to open the file for reading
 */
FILE * file_open(char *filename)
{
    FILE *fr = fopen(filename, "r");
    if (fr == NULL)
    {
        fprintf(stderr, "Subor sa nepodarilo otvorit\n");
        exit(1);
    }

    return fr;
}

/*
 * @brief Function to print lines from 2D array
 *
 * @param lines 2D array containing lines from file
 * @param lines_num Amount of lines to be printed
 * @param index Iterator through lines
 * @param cyclic Indicator if actual number of line that have been read is higher
 *                 than lines to print
 */
void print_lines(char lines[][LINE_LEN+2], int lines_num, int index, bool cyclic)
{
    if (cyclic) 
    {
        for (int i = index; i != ((index) ? index-1 : lines_num-1); i=(i+1)%lines_num)
        {
            printf("%s",  lines[i]);
        }
        printf("%s", lines[(index) ? index-1 : lines_num-1]);
    } 
    else 
    {
        for (int i = 0; i < index; i++){
            printf("%s",  lines[i]);
        }
    }

}

int main(int argc, char *argv[]){
    
    int lines_num = 10;          //number of lines to print
    bool cyclic = false;         //indicator, if index surpassed lines_num 
    char *filename = NULL;       //name of the file   
    bool plus = false;           //indicator of '+' sign after '-n' option
    bool warning = false;        //waning message if line is longer than the limit

    switch (arguments(argc, argv, &lines_num, &filename))
    {
        case -1:
          fprintf(stderr, "Pocet riadkov musi byt cele cislo vacsie ako nula\n");
          exit(1);
        case -2:
          fprintf(stderr, "Chybajuci argument za parametrom -n\n");
          exit(2);
        case 1:
          plus = true;
          break;
    }

    FILE *fr;

    if (filename != NULL)
    {
        fr = file_open(filename);
    }
    else 
    {
        fr = stdin;   
    }

    char lines[lines_num][LINE_LEN+2];  //<circular buffer to store lines from a file
    int index = 0;                      //<index of circular buffer
    
    int line_counter = 0;
    while (fgets(lines[index], LINE_LEN+1, fr) != NULL)
    {
        if (strlen(lines[index]) == (LINE_LEN) && lines[index][LINE_LEN-1] != '\n')
        {
            lines[index][LINE_LEN] = '\n';
            lines[index][LINE_LEN+1] = '\0';
            while (fgetc(fr) != '\n')
                ;

            if (!warning)
            {
                warning = true;
                fprintf(stderr, "Prekrocenie limitu dlzky riadku\n");
            }
        }
        if (plus && line_counter+1 >= lines_num)
        {
            printf("%s", lines[index]);
        }

        index++;
            if (index > lines_num-1)
            {
                index %= lines_num;
                cyclic = true;   
            }
        line_counter++;
    }  
 
    if (!plus)
    {
       print_lines(lines, lines_num, index, cyclic);
    }

    fclose(fr);
    return 0;
}
