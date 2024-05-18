/**
 * @brief   Header file for parsing command line options
 * @file    parse_args.h
 * @author  xzvara01(@vutbr.cz)
 * @date    20.04.2022
 */

#ifndef _ARG_PARSE_H
#define _ARG_PARSE_H 1

#include "common.h"

#define TIMEOUT_DEFAULT  5000

#define help()           {fprintf(stderr, "%s", help_string); exit(0);}
#define INVALID_ARG 2
#define MISSING_ARG 3
#define UNKNOWN_OPT 4

/** 
 * @enum port_format
 * 
 * @brief Define format of scanned ports used program options
 */
enum port_format
{
    CONT = 1, /*!< continuous format: range (eg. 20-30) */
    DISC  /*!< discrete format: concrete values (eg. 20,30) */
};

/** 
 * @struct port
 * 
 * @brief Define boundaries of continuous port type or concrete port values for discrete port type 
 */
struct port
{
    int start;
    int end;
    int array_length;
    int *array;
};

/** 
 * @struct arguments
 * 
 * @brief Store program options 
 */
struct arguments
{
    char interface[256];
    char domain[256];
    enum port_format tcp_type;
    struct port tcp;
    enum port_format udp_type;
    struct port udp;
    int timeout;
};

/**
 * @brief Parse arguments from command line into struct arguments structure
 * 
 * @param[in] argc Number of program arguments
 * @param[in] argv Actual program arguments
 * 
 * @return Pointer to static structure 'arguments' containing parsed options
 */
struct arguments *parse_args(int argc, char *argv[]);

/**
 * @brief Free pointers allocated for port arrays in parse_args
 * 
 * @param[in] uargs Number of arguments
 */
void free_args(struct arguments *uargs);

/**
 * @brief Print argument structure
 * 
 * @param[in] user_args Argument structure
 */
void print_args(struct arguments user_args);

#endif