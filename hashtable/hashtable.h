/*
	Author: Enebz
	Description: This is my own implementation of a hash table for string:string pairs with chaining in C.
*/
#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Get ssize_t type based on the OS and name it ht_ssize_t
#ifdef _WIN32
	#include <BaseTsd.h>
	typedef SSIZE_T ht_ssize_t;
#else
	#include <unistd.h>
	typedef ssize_t ht_ssize_t;
#endif


/* Data structures prototypes */

// Node of the linked list which is the bucket / chain of the hashtable
typedef struct node node;

// Hashtable
typedef struct hashtable hashtable;


/* Data structures definitions */

struct node
{
	char* key;
	char* value;
	node* next;
	node* prev;
};

struct hashtable
{
	int size;
	node** container;
};


/* Function prototypes */

// Hash function: DJB2
unsigned long   ht_hash                 (char* key);

// Create a hashtable with a given size
hashtable*      ht_create               (int size);

// Fucntion to delete and free the hashtable from the memory
void            ht_destroy              (hashtable* ht);

// Insert a key-value pair into the hashtable
// Error codes: 0 = success, 1 = memory allocation error, 2 = key already exists
int             ht_insert               (hashtable* ht, char* key, char* value);

// Insert a key-value pair into the hashtable
// This function will force insert the key-value pair into the hashtable even if the key already exists
// This is useful when you want to update the value of a key without checking if it already exists twice
int             ht_force_insert         (hashtable* ht, char* key, char* value);

// Update the value of a key in the hashtable
int             ht_update               (hashtable* ht, char* key, char* value);

// Delete a key-value pair from the hashtable
int             ht_delete               (hashtable* ht, char* key);

// Clear the hashtable
int             ht_clear                (hashtable* ht);

// Get the value of a key from the hashtable
char*           ht_get                  (hashtable* ht, char* key);

// Get the node of a key from the hashtable
node*           ht_get_node             (hashtable* ht, char* key);

// Get the node of a key from the hashtable at a given bucket index
node*           ht_get_node_at_index    (hashtable* ht, char* key, unsigned long index);

// Gets the chain of a bucket at a given index
node*           ht_get_bucket           (hashtable* ht, unsigned long index);

// Get the keys of the hashtable
char**          ht_keys                 (hashtable* ht);

// Get the values of the hashtable
char**          ht_values               (hashtable* ht);

// Get the count of the hashtable
ht_ssize_t      ht_count                (hashtable* ht);

// Print the hashtable
void            ht_print                (hashtable* ht);

// Print the bucket at a given index
void            ht_print_bucket         (hashtable* ht, unsigned long index);

#endif // _HASHTABLE_H