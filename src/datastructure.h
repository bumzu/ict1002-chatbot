#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

// Maximum hash table size for sections hash table
// Value can be easily changed depending on user needs
#define SECTION_TABLE_SIZE 4

// Maximum hash table size for entity hash table
// Value can be easily changed depending on user needs
#define ENTITY_TABLE_SIZE 256

// Represents a node in an entity hash table
typedef struct node {
    const char* entity_key;
    char* description_value;
    struct node* next;
} node;

// Represents a hashtable that has an array of entries
typedef struct ht
{
    node** entries;
} ht;

// Represents a node in a section hash table
typedef struct section_node {
    const char* section_key;
    ht* section_ht;
    struct section_node* next;
} section_node;

// sections[] is initialized in chatbot.c
/* extern is used here so that other c source files in project gets access 
to this variable if this header file is included in that c source file. */
extern section_node* sections[SECTION_TABLE_SIZE];

/* Data structure implementation and functions defined in chatbot.c */
ht* create_entity_ht(void);
char* section_entity_ht_get(section_node* sections[], const char* section_key, const char* entity_key);
bool section_entity_ht_set(section_node* sections[], const char* section_key, const char* entity_key, char* value);
ht* section_ht_get(section_node* sections[], const char* section_key);
bool section_ht_set(section_node* sections[], const char* key, ht* hashtable);
void display_section_ht(section_node* section[]);
void unload_section_ht(section_node* section[]);

/* Data structure hash functions */
unsigned int hash(const char* word, unsigned int max_table_size);
unsigned int entity_hash(const char* word);
unsigned int section_hash(const char* word);

/* 
The following contain functions NOT meant to be used directly.
*/

/* Entity Hashtable Helper functions defined in chatbot.c */
char* entity_ht_get(ht* hashtable, const char* key);
bool entity_ht_set(ht* hashtable, const char* key, char* value);
node* create_entity_entry(const char* key, char* value);
void display_entity_ht(ht* hashtable);
void unload_entity_ht(ht* hashtable);

/* Section Hashtable Helper functions defined in chatbot.c */
section_node* create_section_entry(const char* key, ht* hashtable);

#endif
