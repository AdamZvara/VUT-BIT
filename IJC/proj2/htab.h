/**
 * @brief Interface for libhtab libraries - added doxygen documentation
 * @file htab.h
 * @author Taken from IJC project page 
 * @date 4.4.2021
 */


#ifndef __HTAB_H__
#define __HTAB_H__

#include <string.h>
#include <stdbool.h>  

struct htab;

/**
 * @typedef htab_t
 * @brief Partial declaration of structure
 */
typedef struct htab htab_t;

/** 
 * @typedef htab_key_t
 * @brief String - word from htab_pair
*/
typedef const char * htab_key_t;  

/**
 * @typedef htab_value_t
 * @brief Int - counter of word from htab_pair
*/
typedef int htab_value_t;

/**
 * @struct htab_pair
 * @brief Structure for storing words and their counters
*/
typedef struct htab_pair {
    htab_key_t   key;       ///< string - word from text
    htab_value_t value;     ///< Integer - counter of words in text
} htab_pair_t;              ///< typedef of htab_pair

/**
 * @brief Hash function for the Hash Table
 *
 * @param str Word to store in the HT
 *
 * @return Index to the HT
*/
size_t htab_hash_function(htab_key_t str);

/***********functions for working with the table********************/

/**
 * @brief Constructor for initializing HT
 *
 * @param n Number of pointers in array
 *
 * @return Pointer to newly initialized HT
 * @return NULL if malloc was not successful, or if n <= 0
*/
htab_t *htab_init(size_t n);

/**
 * @brief Creates and initializes new table (with data from another table)
 * @details Table, from which the data is moved is left empty
 *
 * @param n New number of pointers in array
 * @param from Old HT
 *
 * @return Pointer to new HT
*/
htab_t *htab_move(size_t n, htab_t *from);

/**
 * @brief Get number of records from HT
 *
 * @param t Pointer to HT
 *
 * @return Number of records
*/
size_t htab_size(const htab_t * t);

/**
 * @brief Get number of pointers in array
 *
 * @param t Pointer to a HT
 *
 * @return Number of pointers in array
*/
size_t htab_bucket_count(const htab_t * t);

/**
 * @brief Looks for string "key" in HT
 *
 * @param t Pointer to HT
 * @param key String - word to find in HT
 *
 * @return Pointer to record, if successful
 * @return NULL if unsuccessful
*/
htab_pair_t * htab_find(htab_t * t, htab_key_t key);

/**
 * @brief Looks for string "key" in HT, if no key is found
 * add key to HT
 *
 * @param t Pointer to HT
 * @param key String - word to find in HT
 *
 * @return Pointer to record
 * @return NULL if malloc was not successful
*/
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);

/**
 * @brief Remove a single records from HT
 *
 * @param t Pointer to HT
 * @param key String - word to remove
 *
 * @return True if function was successful
 * @return False if no record was found
*/
bool htab_erase(htab_t * t, htab_key_t key);

/** 
 * @brief Apply function f to each record in a table
 * 
 * @param t Pointer to a HT
 * @param f Function that will be applied for each record
*/
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));

/**
 * @brief Remove all records from HT
 *
 * @param t Pointer to HT
*/
void htab_clear(htab_t * t);

/**
 * @brief Hash Table destructor
 *
 * @param t Pointer to HT
 */
void htab_free(htab_t * t);

#endif // __HTAB_H__
