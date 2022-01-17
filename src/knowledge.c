/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "chat1002.h"

// Contains struct declarations for data structure and its function prototypes
#include "datastructure.h"

// Maximum line for an entity description as specified in the project specs
// LINE_MAX = 64 (MAX_ENTITY) + 1 (for '=' char) + 256 (MAX_RESPONSE) + 1 (for '\n' char) 
#define LINE_MAX MAX_ENTITY + MAX_RESPONSE + 2

 /*
  * Get the response to a question.
  *
  * Input:
  *   intent   - the question word
  *   entity   - the entity
  *   response - a buffer to receive the response
  *   n        - the maximum number of characters to write to the response buffer
  *
  * Returns:
  *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
  *   KB_NOTFOUND, if no response could be found
  *   KB_INVALID, if 'intent' is not a recognised question word
  */
int knowledge_get(const char* intent, const char* entity, char* response, int n)
{

	// Checks if a response exist in the section

	// Check to see if section exists
	ht* section = section_ht_get(sections, intent);

	// If section does not exists, return KB_INVALID
	if (section == NULL)
	{
		return KB_INVALID;
	}

	// Else, try to get the description value in the section with the entity key
	char* description_value = section_entity_ht_get(sections, intent, entity);

	// If there is a valid entry
	if (description_value != NULL)
	{

		// Copy the contents of the description into the response buffer
		strcpy(response, description_value);
		return KB_OK;
	}

	// Else, there is no key match with the given entity key, return KB_NOTFOUND
	return KB_NOTFOUND;
}

/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char* intent, const char* entity, const char* response)
{
	// Check to see if section exists
	ht *section = section_ht_get(sections, intent);

	// If section retrieval fails, return KB_INVALID 
	if (section == NULL) 
	{
		return KB_INVALID;
	}
	else
	{
		// Insert new response and overwrite if it exists to be added to the knowledge base
		// This is accounted in section_entity_ht_set()
		// If set operation is successful, return KB_FOUND
		if (section_entity_ht_set(sections, intent, entity, (char*) response))
		{
			return KB_FOUND;
		}
		
		return KB_NOMEM;
	}
}

/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE* f)
{

	// Initialize pairs counter
	unsigned int pairs = 0;

	// Declare buffer to read line from file
	char input_buffer[LINE_MAX] = { 0 };

	// Declare variables for use later (explanation provided further down)
	unsigned int i = 0, j = 0, k = 0;
	bool entry_flag = false;
	bool section_flag = false;
	bool valid_section = false;
	bool is_section_exist = false;
	char *recognized_intents[3] = {"what", "where", "who"};

	// Initialize buffers for use
	char section_key_buffer[LINE_MAX] = { 0 };
	char entity_key_buffer[MAX_ENTITY] = { 0 };
	char description_value_buffer[MAX_RESPONSE] = { 0 };

	// Read the file line by line
	while ((fgets(input_buffer, LINE_MAX, f)) != NULL)
	{
		
		/* If the line is a section, this indicates the start of a new section.
		buffer[0] is used to check for '[' to determine start of section. */
		if (input_buffer[0] == '[')
		{

			section_flag = true;

			/* i index used to track the closing bracket.
			According to project specs, once a section is found, 
			it starts with a '[' char and it is guaranteed to have a ']' char. 
			But we must ignore all chars after the closing ']' char.
			*/
			i = 1;
			while (input_buffer[i] != ']')
			{
				i++;
			}

			// Check for empty section "[]"
			if (i == 1)
			{
				section_flag = false;
			}

			if (section_flag)
			{
				// Copy input buffer over to section key buffer
				for (unsigned int q = 0; q < i - 1; q++)
				{
					section_key_buffer[q] = tolower(input_buffer[1 + q]);
				}

				// Set the NULL terminator in string
				section_key_buffer[i - 1] = '\0';

				// Check to see if intent is recognized by our chatbot
				valid_section = false;
				for (int q = 0; q < 3; q++)
				{
					if (strcmp(section_key_buffer, recognized_intents[q]) == 0 )
					{
						valid_section = true;
						break;
					}
				}

				// Set the section key and entity hashtable insert it into sections hash table
				// If valid section is determined
				if (valid_section)
				{

					// Try to get the section from the sections hash table
					ht* section = section_ht_get(sections, section_key_buffer);

					// Section does not exist in the hash table
					if (section == NULL)
					{

						// Create the entity hash table (a new section)
						ht* new_section = create_entity_ht();

						if (new_section == NULL)
						{
							printf("Ran out of memory.\nNo memory is allocated.\n");
        					return -1;
						}

						// Allocate memory for the section key
						char* section_key = malloc(strlen(section_key_buffer) + 1);

						// Check for sufficient memory.
						if (section_key == NULL)
						{
							free(new_section);
							printf("Ran out of memory.\nNo memory is allocated.\n");
        					return -1;
						}

						// Copy the contents of buffer over to the section key
						strcpy(section_key, section_key_buffer);

						// Add section to section hash table
						section_ht_set(sections, section_key, new_section);
					}

					/* Else if the section exists in hash table
					The entry is inserted in the next step */
				}
			}
		}

		// Else if is a blank line, continue
		else if (input_buffer[0] == '\n')
		{
			section_flag = false;
			continue;
		}

		// Else, check if it is an entry in a section
		else
		{

			if (valid_section)
			{
				
				// i index used to write entity_key from buffer
				// j index used to track if there is a valid entry in line
				// k index used to write description_value from buffer
				i = 0, j = 0; k = 0;

				// Flag check to determine if line parsed in file is an entry
				entry_flag = false;

				// Use j index to seek up to '=' to determine an entry
				do
				{

					// Check for empty entry "="
					if (input_buffer[0] == '=')
					{
						break;
					}

					/* Stop when either an entry is determined
					or a newline is found (newline signifies end of line) */
					if (input_buffer[j] == '=')
					{
						entry_flag = true;
						break;
					}

					else if (input_buffer[j] == '\n')
					{
						break;
					}

					j++;
				} while (j < LINE_MAX);

				// If there is a valid entry
				if (entry_flag)
				{
					// Use i index to seek up to j
					while (i != j)
					{

						// Set entity_key
						entity_key_buffer[i] = input_buffer[i];
						i++;
					}

					// Set NULL terminator in string
					entity_key_buffer[i] = '\0';

					// Move i index pass '=' char
					i++;

					// Seek up to the end of the line excluding the '\n' character
					while (i != strlen(input_buffer) - 1)
					{

						// Set description_value
						description_value_buffer[k] = input_buffer[i];
						k++;
						i++;
					}

					// Set NULL terminator in string
					description_value_buffer[k] = '\0';
					
					/* If there is a value, we are replacing the description value.
					Else, value is updated in entity hash table. Function returns true if set,
					false otherwise. Either ran out of memory, invalid inserts (section does not exist).
					. */
					if (section_entity_ht_set(sections, section_key_buffer, entity_key_buffer, description_value_buffer))
					{
						// Increment pair counter
						pairs++;
					};
				}
			}
		}
	}

	return pairs;
}

/*
 * Reset the knowledge base, removing all known entities from all intents.
 */
void knowledge_reset()
{

	/* Unload section hashtables. Memory allocated is freed.
	All pointers in sections hash table are initialize to NULL. */
    unload_section_ht(sections);
}

/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE* f)
{
	// To hold each section hashtable [who] [what] [where]
	ht *temp = NULL;

    // Iterate through sections hashtable
    for (int i = 0; i < SECTION_TABLE_SIZE; i++)
    {
        // While there is a section in the bucket
        if (sections[i] != NULL)
        {
            // Set a travesal pointer to the head of the list for the index / bucket
            section_node* trav = sections[i];

            // While the end of the list is not reached for the current index
            while (trav != NULL)
            {
				// Add the section key [who] etc to the file stream
				fprintf(f, "[%s]\n", trav->section_key);

				// Hold the temp value of each traversal entity
				temp = trav->section_ht;

				// Iterate through the entity hashtable
				for (int i = 0; i < ENTITY_TABLE_SIZE; i++)
				{
					// If there is an entry in the entity hashtable
					if (temp->entries[i] != NULL)
					{
						// Set a travesal pointer to the start of the bucket
						node* trav = temp->entries[i];

						// While there is a linked entry in the bucket
						while (trav != NULL)
						{
							// Add the entity key and description value to filestream
							fprintf(f, "%s=%s\n", trav->entity_key, trav->description_value);

							// Set travesal to the next linked entry in bucket
							trav = trav->next;
						}
					}
				}

				trav = trav->next;
				fprintf(f, "\n");
            }
        }
    }
}
