/**
 * @brief Header file for proj2.c
 * @file proj2.h
 * @author Adam Zvara
 * @date 22.4.2021
 */

#ifndef PROJ2_H
#define PROJ2_H

//forking
#include <sys/types.h>
#include <unistd.h>

//waiting
#include <sys/wait.h>

//shared variables
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>  

//semaphores management
#include <semaphore.h>

//variadic functions
#include <stdarg.h>

//random number seed
#include <time.h>

//standard libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//parsing arguments
#include "parse_args.h"

//define max values for elves and reindeers
#define MAX_ELVES 1000
#define MAX_RAIND 20
#define MAX_ELF_T 1000
#define MAX_REIND_T 1000

typedef struct shared
{
    int proc_count;         ///<Counter of all actions that happened
    int elves;              ///<Number of elves waiting for santas help
    int reindeers;          ///<Number of RD that came back
    bool workshop_closed;   ///<Indicator if workshop is closed
} shared_t;

/**
 * @brief Initializes all the semaphores in sem. structure
 *
 * @return 0 on success
 * @return 1 if any error occured
*/
int init_semaphores();

/**
 * @brief Deletes all the semaphores
 */
void delete_semaphores();

/**
 * @brief Initialize shared variables between processes
 *
 * @return 0 on succcess
 * @return 1 if any error occured
 */
int initialize_shared();

/**
 * @brief Function to clear shared variables
 */
void delete_shared();

/**
 * @brief Function to generate random number from min to max
 *
 * @param min Lower bound of the interval
 * @param max Upper bound of the interval
 *
 * @return Random number within the interval
 */
int random_number(int min, int max);

/**
 * @brief Write message to a file (used by processes)
 *
 * @param fr Pointer to an opened file
 * @param msg Last known parameter, format of the message to print out
 *
 * @details This function is variadic. Predefined format of this message is
 * "numOfProcess : *msg", so you don't need to include the first number 
 * in your msg string
 */
void print_msg(FILE *fr, const char *msg, ...);

#endif
