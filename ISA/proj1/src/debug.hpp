/**
 * @file debug.hpp
 * @author xzvara01 (xzvara01@stud.fit.vutbr.cz)
 * @brief Debug macros for printing out values to stderr
 * @date 2022-10-06
 *
 */

#ifndef _DEBUG_HPP
#define _DEBUG_HPP

/* Define return values */
#define DEBUG_STRING "srcIP %u, dstIP %u, srcPort %u, dstPort %u, protocol %u, first %u, last %u, octets %u, packets %u\n"

/* Define debug macros */

// print out macro for main program (flow.cpp)
#define DEBUG_MAIN 0
#if DEBUG_MAIN == 1
    #define dfprintf(...) {fprintf(stderr, __VA_ARGS__);}
#else
    #define dfprintf(...) {}
#endif

// print out macro for parsing program (parse.cpp)
#define DEBUG_PARSE 0
#if DEBUG_PARSE == 1
    #define dpprintf(...) {fprintf(stderr, __VA_ARGS__);}
#else
    #define dpprintf(...) {}
#endif

// print idividual flow when inserting to flow cache
#define DEBUG_FLOWINSERT 0
#if DEBUG_FLOWINSERT == 1
    #define dicprintf(...) {\
        fprintf(stderr, "[store.cpp] created: ");\
        fprintf(stderr, DEBUG_STRING, __VA_ARGS__);}
    #define diuprintf(...) {\
        fprintf(stderr, "[store.cpp] updated: ");\
        fprintf(stderr, DEBUG_STRING, __VA_ARGS__);}
#else
    #define dicprintf(...) {}
    #define diuprintf(...) {}
#endif

// print exported flows
#define DEBUG_EXPFLOW 0
#if DEBUG_EXPFLOW == 1
    #define deprintf(...) {\
        fprintf(stderr, __VA_ARGS__);}
    #define dexprintf() {\
        fprintf(stderr, DEBUG_STRING,\
            record->second.srcaddr, record->second.dstaddr, record->second.srcport, record->second.dstport, record->second.prot,\
            record->second.First, record->second.Last, record->second.dOctets, record->second.dPkts);}

#else
    #define dexprintf(...) {}
    #define deprintf(...) {}
#endif

// print whole flow cache
#define DEBUG_FLOWCACHE 0
#if DEBUG_FLOWCACHE == 1
    #define dfcprintf(...) {\
        fprintf(stderr, "[flow.cpp] Printing flow cache:\n");\
        for (auto item = flow_cache.cbegin(); item != flow_cache.cend(); ++item) {\
            fprintf(stderr, DEBUG_STRING,\
            item->second.srcaddr, item->second.dstaddr, item->second.srcport, item->second.dstport, item->second.prot,\
            item->second.First, item->second.Last, item->second.dOctets, item->second.dPkts);}}
#else
    #define dfcprintf(...) {}
#endif

#endif