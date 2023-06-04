/*
	Author: Enebz
	Description: This is my own implementation of a hash table for string:string pairs with chaining in C.
*/


#include "hashtable.h"


/* Function definitions */

unsigned long ht_hash(char* key)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *key++))
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

hashtable* ht_create(int size)
{
	hashtable* table = (hashtable*)malloc(sizeof(hashtable));

	// Check if the hashtable was created successfully
	if (table == NULL)
	{
		return NULL;
	}

	table->size = size;
	table->container = (node**)malloc(sizeof(node*) * size);

	// Check if the hashtable array was created successfully
	if (table->container == NULL)
	{
		return NULL;
	}

	// Set buckets to NULL
	for (int i = 0; i < size; i++)
	{
		table->container[i] = NULL;
	}

	return table;
}

void ht_destroy(hashtable* ht)
{
	node* tmp;

	for (int i = 0; i < ht->size; i++)
	{
		for (node* ptr = ht->container[i]; ptr != NULL; ptr = tmp)
		{
			tmp = ptr->next;
			free(ptr);
		}
	}
	free(ht);
}

int ht_insert(hashtable* ht, char* key, char* value)
{
	unsigned long index = ht_hash(key) % ht->size;

	// Check if the key already exists
	if (ht_get_node_at_index(ht, key, index) != NULL)
	{
		return 2;
	}

	// Create node
	node* new_node = (node*)malloc(sizeof(node));

	if (new_node == NULL)
	{
		return 1;
	}

	new_node->key = key;
	new_node->value = value;
	new_node->next = ht->container[index];
	new_node->prev = NULL;
	
	// Insert node
	if (ht->container[index] != NULL)
	{
		ht->container[index]->prev = new_node;
	}

	ht->container[index] = new_node;
	
	return 0;
}

int ht_force_insert(hashtable* ht, char* key, char* value)
{
	unsigned long index = ht_hash(key) % ht->size;

	// Create node
	node* new_node = (node*)malloc(sizeof(node));

	if (new_node == NULL)
	{
		return 1;
	}

	new_node->key = key;
	new_node->value = value;
	new_node->next = ht->container[index];
	new_node->prev = NULL;
	
	// Insert node
	if (ht->container[index] != NULL)
	{
		ht->container[index]->prev = new_node;
	}

	ht->container[index] = new_node;

	return 0;
}

int ht_update(hashtable* ht, char* key, char* value)
{
	unsigned long index = ht_hash(key) % ht->size;

	node* node_to_update;

	if ((node_to_update = ht_get_node_at_index(ht, key, index)) == NULL)
	{
		return ht_force_insert(ht, key, value);
	}

	node_to_update->value = value;

	return 0;
}

int ht_delete(hashtable* ht, char* key)
{
	unsigned long index = ht_hash(key) % ht->size;

	node* node_to_delete;

	if ((node_to_delete = ht_get_node_at_index(ht, key, index)) == NULL)
	{
		return 1;
	}

	node* left_node = node_to_delete->prev;
	node* right_node = node_to_delete->next;

	// If the node to delete's left is the head of the linked list
	if (left_node == NULL)
	{
		// If right_node is NULL, that is fine, because then the bucket is empty
		ht->container[index] = right_node;
	} else {
		left_node->next = right_node;
	}

	// If the node to delete's right is NULL
	if (right_node != NULL)
	{
		right_node->prev = left_node;
	}

	free(node_to_delete);
	return 0;
}

int ht_clear(hashtable* ht)
{
	node* tmp;

	for (int i = 0; i < ht->size; i++)
	{
		for (node* ptr = ht->container[i]; ptr != NULL; ptr = tmp)
		{
			tmp = ptr->next;
			free(ptr);
		}
		ht->container[i] = NULL;
	}

	return 0;
}

char* ht_get(hashtable* ht, char* key)
{
	node* node_found;

	if ((node_found = ht_get_node(ht, key)) == NULL)
	{
		return NULL;
	}

	return node_found->value;
}

node* ht_get_node(hashtable* ht, char* key)
{
	unsigned long index = ht_hash(key) % ht->size;

	// Loop through the linked list to find the key
	// Begin at the bucket head which is a pointer to the head of the linked list
	for (node* ptr = ht->container[index]; ptr != NULL; ptr = ptr->next)
	{
		if (strcmp(key, ptr->key) == 0)
		{
			return ptr;
		}
	}

	return NULL;
}

node* ht_get_node_at_index(hashtable* ht, char* key, unsigned long index)
{
	// Loop through the linked list to find the key
	// Begin at the bucket head which is a pointer to the head of the linked list
	for (node* ptr = ht->container[index]; ptr != NULL; ptr = ptr->next)
	{
		if (strcmp(key, ptr->key) == 0)
		{
			return ptr;
		}
	}

	return NULL;
}

node* ht_get_bucket(hashtable* ht, unsigned long index)
{
	return ht->container[index];
}

char** ht_keys(hashtable* ht)
{
	// We will not use ht_count() because it is O(n) and we want to avoid that
	char** keys = (char**)malloc(1);

	if (keys == NULL)
	{
		return NULL;
	}

	size_t keys_size = 0;

	for (int i = 0; i < ht->size; i++)
	{
		for (node* ptr = ht->container[i]; ptr != NULL; ptr = ptr->next)
		{
			keys_size++;
			keys = (char**)realloc(keys, keys_size * sizeof(char*));

			if (keys == NULL)
			{
				return NULL;
			}

			keys[keys_size - 1] = ptr->key;
		}
	}

	if (keys_size == 0)
	{
		return NULL;
	}

	return keys;
}

char** ht_values(hashtable* ht)
{
	// We will not use ht_count() because it is O(n) and we want to avoid that
	char** values = (char**)malloc(1);

	if (values == NULL)
	{
		return NULL;
	}

	size_t values_size = 0;

	for (int i = 0; i < ht->size; i++)
	{
		for (node* ptr = ht->container[i]; ptr != NULL; ptr = ptr->next)
		{
			values_size++;
			values = (char**)realloc(values, values_size * sizeof(char*));

			if (values == NULL)
			{
				return NULL;
			}

			values[values_size - 1] = ptr->value;
		}
	}

	if (values_size == 0)
	{
		return NULL;
	}

	return values;
}

ht_ssize_t ht_count(hashtable* ht)
{
	ht_ssize_t count = 0;

	for (int i = 0; i < ht->size; i++)
	{
		for (node* ptr = ht->container[i]; ptr != NULL; ptr = ptr->next)
		{
			count++;
		}
	}

	return count;
}

void ht_print(hashtable* ht)
{
	for (int i = 0; i < ht->size; i++)
	{
		ht_print_bucket(ht, i);
	}

	printf("\n");
}

void ht_print_bucket(hashtable* ht, unsigned long index)
{
	printf("%lu\t%p\t", index, ht->container[index]);
	for (node* ptr = ht->container[index]; ptr != NULL; ptr = ptr->next)
	{
		printf("%s:%s - ", ptr->key, ptr->value);
	}
	printf("NULL\n");
}
