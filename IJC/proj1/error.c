/**
 * @brief Implementation of error messages, task B) - IJC-DU1
 * @file error.c
 * @author Adam Zvara - xzvara01, FIT
 * @date 6.3.2021
 * @details Compiled with gcc 9.3.0 tested on Ubuntu 20.04.1 
 */

#include <stdarg.h>
#include "error.h"

void warning_msg(const char *fmt, ...)
{
    fprintf(stderr ,"CHYBA: ");
    
    va_list args; 
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void error_exit(const char *fmt, ...)
{
    fprintf(stderr ,"CHYBA: ");
    
    va_list args; 
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    exit(1);
}
