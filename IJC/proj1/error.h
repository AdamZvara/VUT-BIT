/**
 * @brief Header file for error.c, task B) - IJC-DU1
 * @file error.h
 * @author Adam Zvara - xzvara01, FIT
 * @date 6.3.2021
 * @details Compiled with gcc 9.3.0 tested on Ubuntu 20.04.1 
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef ERROR_H
#define ERROR_H

/**
 * @brief Prints out warning message without exiting the program
 * @param fmt Format of the printed out message
 */
void warning_msg(const char *fmt, ...);


/**
 * @brief Prints out warning message and exits
 * @param fmt Format of the printed out message
 */
void error_exit(const char *fmt, ...);

#endif
