/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "chat1002.h"

// Contains struct declarations for data structure and its function prototypes
#include "datastructure.h"

 // Declaring sections hashtable (an array of pointers to section_node struct)
section_node* sections[SECTION_TABLE_SIZE];
bool section_ht_initialized = false;

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char* chatbot_botname() 
{
    return "Zeus";
}

/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char* chatbot_username() 
{
    return "Prometheus";
}

/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char* inv[], char* response, int n) {

    /* check for empty input */
    if (inc < 1) {
        snprintf(response, n, "");
        return 0;
    }

    // If sections hash table is not initialized
    if (!section_ht_initialized)
    {

        // Initialize section hash table pointers. Set all section pointers to NULL.
        for (int i = 0; i < SECTION_TABLE_SIZE; i++)
        {
            sections[i] = NULL;
        }

        section_ht_initialized = true;
    }

    /* look for an intent and invoke the corresponding do_* function */
    if (chatbot_is_exit(inv[0]))
        return chatbot_do_exit(inc, inv, response, n);

    else if (chatbot_is_display(inv[0]))
        return chatbot_do_display(inc, inv, response, n);

    else if (chatbot_is_smalltalk(inv[0]))
        return chatbot_do_smalltalk(inc, inv, response, n);

    else if (chatbot_is_load(inv[0]))
        return chatbot_do_load(inc, inv, response, n);

    else if (chatbot_is_question(inv[0]))
        return chatbot_do_question(inc, inv, response, n);

    else if (chatbot_is_reset(inv[0]))
        return chatbot_do_reset(inc, inv, response, n);

    else if (chatbot_is_save(inv[0]))
        return chatbot_do_save(inc, inv, response, n);

    else {
        snprintf(response, n, "I don't understand \"%s\".", inv[0]);
        return 0;
    }

}

/*
 * Determine whether an intent is DISPLAY.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "display"
 *  0, otherwise
 */
int chatbot_is_display(const char* intent) 
{
    return compare_token(intent, "display") == 0;
}

/*
 * Perform the DISPLAY intent.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  0 (the chatbot always continues chatting after displaying knowledge)
 */
int chatbot_do_display(int inc, char* inv[], char* response, int n) 
{
    // Display section hashtable
    display_section_ht(sections);
    snprintf(response, n, "Knowledge base displayed.");

    return 0;
}

/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char* intent) 
{
    return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;
}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char* inv[], char* response, int n) 
{
    // Unload sections hashtable, free allocated memory for hash table
    unload_section_ht(sections);

    snprintf(response, n, "Goodbye!");

    return 1;
}


/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char* intent) 
{
    // Checks if intent is a load intent
    return compare_token(intent, "load") == 0;
}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char* inv[], char* response, int n) 
{
    // Initialize file name string buffer
    char file_name[MAX_ENTITY] = "";

    // Check through word array
    for (int i = 1; i < n; i++)
    {

        // If there is a word
        if (inv[i] != NULL)
        {
            
            // Build file name string
            strcat(file_name, inv[i]);
            strcat(file_name, " ");
        }

        // Else we have reached the end of the inv (input vector), no more words
        else
        {
            // Strip the last ' ' (whitespace) char
            file_name[strlen(file_name) - 1] = '\0';
            break;
        }
    }

    char support_file_type[3] = {'i', 'n', 'i'};

    // Check file type, chatbot only supports .ini files
    for (int i = 0, len = strlen(file_name); i < 3; i++)
    {
        if (!(tolower(file_name[len - 1 - i]) == support_file_type[i]))
        {
            snprintf(response, n, "File type not supported. Please use .ini files.");
            return 0;
        }
    }

    // Try to open file for reading
    FILE* f = fopen(file_name, "r");

    // If file pointer is NULL, could not open file
    if (f == NULL)
    {
        strcpy(response, "Could not open file for reading. Please check file name.");
        return 0;
    }

    // Call knowledge_read() function to load file contents into hashtable
    int pairs = knowledge_read(f);

    snprintf(response, n, "Read %i responses from %s.", pairs, file_name);

    fclose(f);

    return 0;
}


/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char* intent) 
{
    // intent array
    char* question[] = { "what", "where", "who" };
    
    // counter to check for all intents in the array
    size_t qlen = sizeof(question) / sizeof(question[0]);
    for (unsigned int k = 0; k < qlen; k++)
    {
        // if intent matches, return 1
        if (compare_token(intent, question[k]) == 0)
        {
            return 1;
        }
    }
    
    // else, return 0
    return 0;
}


/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char* inv[], char* response, int n) 
{   
    // initialize counter for the word number for which the entity will start counting from
    int i = 0;

    // the second word in the response / question, "is" or "are"
    char *secondword = NULL;

    // initialize entity string
    char entity[MAX_ENTITY] = "";
    
    // initialize intent buffer
    char intent[MAX_INTENT] = "";

    // initialize string for response.
    char answer[MAX_RESPONSE + 1] = "";

    // initialize intent string for chatbot to relay back to user. (with capitalized first letter)
    char string[MAX_INTENT] = "";
    
    // check if entity exists
    if(inc > 1)
    {
        // assign intent string
        if (compare_token(inv[0], "what") == 0)
        {
            strcat(string, "What");

            // Copy the contents of "what" string into intent buffer
            strcpy(intent, "what");
        }
        else if (compare_token(inv[0], "where") == 0)
        {
            strcat(string, "Where");

            // Copy the contents of "where" string into intent buffer
            strcpy(intent, "where");
        }
        else if (compare_token(inv[0], "who") == 0)
        {
            strcat(string, "Who");

            // Copy the contents of "who" string into intent buffer
            strcpy(intent, "who");
        }
        
        // assign entity string
        if (compare_token(inv[1], "is") == 0)
        {
            secondword = "is";
            i = 2;
        }
        else if(compare_token(inv[1], "are") == 0)
        {
            secondword = "are";
            i = 2;
        }
        else
        {
            i = 1;
        }
        for (int k = i; k < n; k++)
        {
            /* this for loop concatenates everything into the entity string 
            after the intent word. */
            if (inv[k] != NULL)
            {
                strcat(entity, inv[k]);
                strcat(entity, " ");
            }
            else
            {
                break;
            }
        }
        
        // remove trailing space from for loop earlier
        entity[strlen(entity) - 1] = '\0';

        /* call knowledge_get function: if return KB_OK then proceed with response.
        If KB_NOTFOUND, will prompt user for input. This will insert the new entity
        into the knowledge base. If KB_INVALID, return invalid intent, and insert
        new intent into the knowledge base. */
        int knowledgecheck = knowledge_get(intent, entity, response, n);
        
        if(knowledgecheck == KB_OK)
        {
            /* if found, print out the response buffer. 
            Response buffer is modified in knowledge_get if KB_OK is returned. */
            return 0;
        }

        // Else if there is no valid description for the entity
        else if (knowledgecheck == KB_NOTFOUND)
        {
            // get response from user.
            prompt_user(answer, MAX_RESPONSE + 1, "I don't know. %s %s %s?", string, secondword, entity);

            // If user did not enter a valid response
            if (strlen(answer) <= 1)
            {
                snprintf(response, n, ":-(");
                return 0;
            }

            // put response into knowledge base.
            int putcheck = knowledge_put(intent, entity, answer);

            if(putcheck == KB_OK)
            {
                // if ok, print response to user with a kind gesture
                snprintf(response, n, "Thank you.");
            }
            else if(putcheck == KB_INVALID)
            {
                // if invalid, prompt user for valid intent
                snprintf(response, n, "Please give valid intent :-(");
            }
            else if (putcheck == KB_NOMEM)
            {
                // if no memory space, inform user
                snprintf(response, n, "No memory space :-(");
            }
        }

        // Else if there is no valid intent in the hashtable
        else if (knowledgecheck == KB_INVALID)
        {   
            // create new section in hashtable
            ht* intentsection = create_entity_ht();

            // Check for sufficient memory.
            if (intentsection == NULL)
            {
                snprintf(response, n, "No memory space :-(");
                return 0;
            }

            char *intentkey = malloc(strlen(intent) + 1);

            // Check for sufficient memory.
            if (intentkey == NULL)
            {

                /* Memory allocation failed for intentkey. 
                Free memory allocated for intentsection earlier on. */
                free(intentsection);
                snprintf(response, n, "No memory space :-(");
                return 0;
            }

            // Copy the contents of intent into intentkey buffer
            strcpy(intentkey, intent);

            // assign section key based on intent key
            section_ht_set(sections, intentkey, intentsection);

            // get response from user.
            prompt_user(answer, MAX_RESPONSE + 1, "I don't know. %s %s %s?", string, secondword, entity);

            // If user did not enter a valid response
            if (strlen(answer) <= 1)
            {
                snprintf(response, n, ":-(");
                return 0;
            }

            // put response into knowledge base.
            int putcheck = knowledge_put(intentkey, entity, answer);
            if(putcheck == KB_OK)
            {
                // if ok, print response to user with a kind gesture
                snprintf(response, n, "Thank you.");
            }
            else if(putcheck == KB_INVALID)
            {
                // if invalid, prompt user for valid intent
                snprintf(response, n, "Please give valid intent :-(");
            }
            else if (putcheck == KB_NOMEM)
            {
                // if no memory space, inform user
                snprintf(response, n, "No memory space :-(");
            }
        }
    }
    else
    {
        // if no entity, prompt user for valid entity
        snprintf(response, n, "Please give entity :-(");
    }

    return 0;
}


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char* intent) 
{
    // Check if intent is reset
    return compare_token(intent, "reset") == 0;
}


/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after being reset)
 */
int chatbot_do_reset(int inc, char* inv[], char* response, int n) 
{
    // Resets the chatbot, removing all knowledge
    knowledge_reset();

    snprintf(response, n, "%s Reset.", "Chatbot");

    return 0;

}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_save(const char* intent) 
{
    // Compare the intent with "save" 
    return compare_token(intent, "save") == 0;
}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char* inv[], char* response, int n) 
{

    // Variable initializations
    // File name buffer
    char filename[MAX_ENTITY];

    // File pointer to write a new file
    FILE *f;

    // String buffer to check for proper file type
    char *inifile;

    // Flag to check for empty knowledge
    bool can_save = false;

    // Check for valid section in hash table
    for (int i = 0; i < SECTION_TABLE_SIZE; i++)
    {

        // If there is a valid section, there are valid entries
        if (sections[i] != NULL)
        {

            // Set can_save to true
            can_save = true;
            break;
        }
    }

    // If there is no knowledge in knowledge base, inform user
    if (!can_save)
    {
        snprintf(response, n, "There is no knowledge to be saved!");
        return 0;
    }

    // Check the element after save (e.g save sample.ini)
    if(inv[1] != NULL)
    {   
        // Checks whether after save has "as" and "to"
        if(strcmp(inv[1], "as") == 0 || strcmp(inv[1], "to") == 0 ) 
        {

            if (compare_token(inv[2], ".ini") == 0)
            {
                snprintf(response, n, "Please specify a filename!");
                return 0;
            }

            // Copy the contents of the file name into buffer
            strcpy(filename, inv[2]);

            // Will return everything beyond "." e.g (".ini")
            inifile = strrchr(inv[2], '.');
        }
        else
        {

            if (compare_token(inv[1], ".ini") == 0)
            {
                snprintf(response, n, "Please specify a filename!");
                return 0;
            }

            // Copy the contents of the file name into buffer
            strcpy(filename, inv[1]);

            // for those without "as" and "to"
            inifile = strrchr(inv[1], '.');
        }

        // If there is a return value in inifile, compare it with ".ini" to see if it matches
        if(inifile != NULL && strcmp(inifile, ".ini") == 0)
        {            
            /*
            Open up a file with the file name specified
            Calls knowledge_write() and store data structure in file stream
            close file stream and write it to the system in .ini format
            */
            f = fopen(filename, "w"); 
            knowledge_write(f); 
            fclose(f); 

            snprintf(response, n, "My knowledge has been saved to %s.", filename);
        }
        else
        {            
            snprintf(response, n, "Please specify the correct type of file name ending with '.ini'.");
        }
    }
    else
    {
        snprintf(response, n, "Please specify a file name ending with '.ini.'");
    }

    return 0;
}


/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char* intent) 
{

    // Variable declarations for valid smalltalk checking
    char* array[] = 
    { 
        "Hello", "Hi", "Hey", "Tell", "Dead", "it", 
        "it's", "Greetings", "Bye", "Goodbye", "OK" 
    };

    size_t length = sizeof(array) / sizeof(array[0]);

    // Check for each word in array
    for (unsigned int i = 0; i < length; i++)
    {

        // If there is a match
        if (compare_token(intent, array[i]) == 0)
        {

            // Return 1, do_smalltalk()
            return 1;
        }
    }

    return 0;
}


/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 * inv - array of pointers to each word in the question
 * inc - The number of words in the question
 * n size of response buffer
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char* inv[], char* response, int n) 
{

    // Variable declarations for valid smalltalk output
    char* output[] =
    {
        "Hello there! We are programmed by humans of SIT!",
        "Can't tell you anything.",
        "Sir, Death is what makes life precious.",
        "I'm fine.",
        "Indeed it is."
    };
    char* greets[] = { "hello", "hi", "hey", "greetings" };

    size_t length = sizeof(greets) / sizeof(greets[0]);
    
    for (unsigned int i = 0; i < length; i++)
    {
        if (compare_token(inv[0], greets[i]) == 0)
        {
            snprintf(response, n, output[0]);
        }
    }
    if (compare_token(inv[0], "tell") == 0)
    {
        snprintf(response, n, output[1]);
    }
    else if (compare_token(inv[0], "dead") == 0)
    {
        snprintf(response, n, output[2]);
    }
    else if (compare_token(inv[0], "ok") == 0)
    {
        snprintf(response, n, output[3]);
    }
    else if (compare_token(inv[0], "it") == 0 || compare_token(inv[0], "it's") == 0)
    {
        snprintf(response, n, output[4]);
    }
    else if (compare_token(inv[0], "goodbye") == 0 || compare_token(inv[0], "bye") == 0)
    {
        snprintf(response, n, "Goodbye, See you soon!");
        return 1;
    }

    return 0;
}

/* 
 *  Function creates and allocates memory for an entity hash table. 
 *  It takes no arguments and returns a pointer to a ht struct.
 *  Ht stands for hash table.
 * 
 *  It essentialy returns a pointer to the hash table. 
 */
ht* create_entity_ht(void)
{

    // ALlocate memory for the entity hash table
    ht* new_entity_ht = malloc(sizeof(ht));

    // Check for sufficient memory
    if (new_entity_ht == NULL)
    {
        printf("Ran out of memory.\nNo memory is allocated.\n");
        return NULL;
    }

    // Allocate memory for the entries in the entity hash table
    new_entity_ht->entries = malloc(sizeof(node) * ENTITY_TABLE_SIZE);

    // Check for sufficient memory
    if (new_entity_ht->entries == NULL)
    {
        free(new_entity_ht);
        printf("Ran out of memory.\nNo memory is allocated.\n");
        return NULL;
    }

    // Set all hash table pointer to NULL
    for (int i = 0; i < ENTITY_TABLE_SIZE; i++)
    {
        new_entity_ht->entries[i] = NULL;
    }

    return new_entity_ht;
}

/*  This function sets the entity hashtable entry in the entity hashtable 
 *  for the related section node with the given section key, 
 *  entity key and the entity description. 
 *
 *  If it is a new entity entry (new key was provided), new memory is allocated
 *  and a new entry is inserted into the entity hash table.
 *
 *  If there is already an existing entry with the same key,
 *  the entry's value is updated instead.
 *
 *  It takes 4 arguments:
 *      1. The sections hashtable.
 *      2. The section key.
 *      3. The entity key.
 *      4. The description value.
 *
 *  It returns true if the kv pair is set successfully in the entity hash table, 
 *  else it returns false if it is fails the set operation.
 *
 *  Note: Section MUST exist for this function to work.
 */
bool section_entity_ht_set(section_node* sections[], const char* section_key, const char* entity_key, char* value)
{

    // Determine the bucket slot for the section
    unsigned int bucket = section_hash(section_key);

    // Set a pointer to point to the first item in the bucket slot
    section_node* section = sections[bucket];

    // Check if the section exists
    if (section == NULL)
    {
        printf("DEBUG: You are trying to set an entity - description key value pair in "
            "an entity hashtable that does not exist.\n"
            "Create a new section first using section_ht_set().\n"
            "No action was taken.\n");
        return false;
    }

    // Set a travesal pointer to the start of the section
    section_node* trav = section;

    // While the trav ptr points to an existing section
    while (trav != NULL)
    {

        /* Check for key match, if there is a key match,
        this means this is the section we want to insert data into.
        Key compares case sensitively. */
        if (strcmp(trav->section_key, section_key) == 0)
        {

            /* Set the section entity hashtable entry with the entity key value pair given.
            entity_ht_set returns true / false depending on the success of the set operation. */
            if (entity_ht_set(trav->section_ht, entity_key, value))
            {
                return true;
            }
            
            return false;
        }

        /* Else, key comparison is not the same, 
        we are not at the right section, go to the next section in list. */
        trav = trav->next;
    }

    // Else trav is NULL, we reached the end of the linked list at the bucket
    printf("DEBUG: You are trying to set an entity - description key value pair in "
        "an entity hashtable that does not exist.\n"
        "Create a new section first using section_ht_set().\n"
        "No action was taken.\n");
    return false;
}

/*  This function gets the entity hash table entry in the sections hash table
 *  with the given section key and entity description key value pair.
 *
 *  It takes 3 arguments:
 *      1. The sections hashtable.
 *      2. The section key.
 *      3. The entity key.
 *
 *  It returns a char* (a string) containing the entity description.
 * 
 *  If it is an entry that does not exist, NULL is returned.
 *
 *  Note: Section MUST exist for this function to work.
 */
char* section_entity_ht_get(section_node* sections[], const char* section_key, const char* entity_key)
{

    // Determine the bucket slot for the section
    unsigned int bucket = section_hash(section_key);

    // Set a pointer to point to the first item in the bucket slot
    section_node* section = sections[bucket];

    // Check if the section exists
    if (section == NULL)
    {
        printf("DEBUG: You are trying to get an entity - description key value pair in "
            "an entity hashtable that does not exist.\n"
            "Create a new section first using section_ht_set().\n"
            "No action was taken.\n");
        return NULL;
    }

    // Set a travesal pointer to the start of the section
    section_node* trav = section;

    // While the trav ptr points to an existing section
    while (trav != NULL)
    {

        /* Check for key match, if there is a key match,
        this means this is the section we want to insert data into.
        Key compares case sensitively. */
        if (strcmp(trav->section_key, section_key) == 0)
        {

            /* Get and return the entity description with the given key
            entity_ht_get() gets the value from the entity hash table. If a
            value is found, the value is return, else NULL is returned. */
            char* matched_value = entity_ht_get(trav->section_ht, entity_key);
            return matched_value;
        }

        // Else, we are not at the right section, go to the next section in list.
        trav = trav->next;
    }

    /* We reached the end of the list.
    There is an entry, but no matching key. Return NULL. */
    return NULL;
}

/*  This is a helper function that sets the entity hash table entry
 *  with the given entity description key value pair.
 *
 *  If it is a new entity entry (new key was provided), new memory is allocated
 *  and a new entry is inserted into the entity hash table.
 *
 *  If there is already an existing entry with the same key,
 *  the entry's value is updated instead.
 *
 *  It takes 3 arguments:
 *      1. The entity hashtable.
 *      2. The entity key.
 *      3. The description value.
 *
 *  It returns true if it is set, false if is not.
 */
bool entity_ht_set(ht* hashtable, const char* key, char* value)
{

    // Determine the bucket slot for the description value
    unsigned int bucket = entity_hash(key);

    // Try to get an entry from the bucket
    node* entry = hashtable->entries[bucket];

    // There is no entry, put it into the bucket slot
    if (entry == NULL)
    {

        // Create a new entry, helper function used to allocate memory.
        node* new_entry = create_entity_entry(key, value);

        // If NULL is returned, ran out of memory
        if (new_entry == NULL)
        {

            // Return false, set opertaion failed
            return false;
        }

        // Set the entity hash table entry pointer to point to the new entry.
        hashtable->entries[bucket] = new_entry;

        // Return true, set operation successful
        return true;
    }

    /* Else there is already an entry in bucket,
    create a linked list instead. */

    // Create a travesal pointer to the first entry in the bucket
    node* trav = hashtable->entries[bucket];

    // Create another travesal to keep track of the previous item in the bucket
    node* prev = hashtable->entries[bucket];

    // While there is still a node in the bucket (linked list)
    while (trav != NULL)
    {

        // If there is a key match, key compares case sensitively.
        if (strcmp(trav->entity_key, key) == 0)
        {

            // Allocate memory for new value
            char *new_value = malloc(strlen(value) + 1);

            // Check for sufficient memory
            if (new_value == NULL)
            {
                printf("Ran out of memory.\nNo memory is allocated.\n");
                return false;
            }

            // If there's enough memory, copy the contents of value into new_value
            strcpy(new_value, value);

            // Free existing description_value
            free(trav->description_value);

            // Replace the value
            trav->description_value = new_value;

            // Return true, set operation successful
            return true;
        }

        // Else there is no match, check next entry
        prev = trav;
        trav = trav->next;
    }

    /* Reach end of linked list for the hashtable[bucket] 
    if program execution reaches here. This means we want to insert a new entry. */

    // Insert new entry
    // If trav is NULL, we are at the end of the linked list for the bucket slot
    if (trav == NULL)
    {

        // Create a new entry, helper function used to allocate memory
        node* new_entry = create_entity_entry(key, value);

        // If NULL is returned, ran out of memory
        if (new_entry == NULL)
        {
            return false;
        }

        // Insert entry
        prev->next = new_entry;
    }

    return true;
}

/* Helper function used to allocate memory for an entry in the entity hashtable
 * It takes as its arguments:
 *  1. key - entity string
 *  2. value - description string related to entity
 */
node* create_entity_entry(const char* key, char* value)
{
    // Create a new node
    node* new_entry = malloc(sizeof(node));

    // Check for sufficient memory
    if (new_entry == NULL)
    {
        printf("Ran out of memory.\nNo memory is allocated.\n");
        return NULL;
    }

    // Create a new key
    new_entry->entity_key = malloc(strlen(key) + 1);

    // If ran out of memory
    if (new_entry->entity_key == NULL)
    {
        
        // Free the existing allocated memory for the entry node
        free(new_entry);
        printf("Ran out of memory.\nNo memory is allocated.\n");
        return NULL;
    }

    // Create a new value
    new_entry->description_value = malloc(strlen(value) + 1);

    // If ran out of memory
    if (new_entry->description_value == NULL)
    {

        // Free the existing allocated memory for the entry key
        free((char *) new_entry->entity_key);

        // Free the existing allocated memory for the entry node
        free(new_entry);
        printf("Ran out of memory.\nNo memory is allocated.\n");
        return NULL;
    }

    // Copy the key and value data into the entry
    strcpy((char *) new_entry->entity_key, key);
    strcpy(new_entry->description_value, value);

    // Set new node next pointer to NULL
    new_entry->next = NULL;

    return new_entry;
}

/*  This is a helper function that gets the entity hash table entry
 *  with the given entity description key value pair.
 *
 *  It takes 2 arguments:
 *      1. The entity hashtable.
 *      2. The entity key.
 *
 *  It returns a char* (a string) containing the entity description.
 * 
 *  If it is an entry that does not exist, NULL is returned.
 */
char* entity_ht_get(ht* hashtable, const char* key)
{

    // Determine the bucket slot
    unsigned int bucket = entity_hash(key);

    // Try to get an entry from the bucket
    node* entry = hashtable->entries[bucket];

    // There is no entry at the bucket slot, return NULL
    if (entry == NULL)
    {
        return NULL;
    }

    // There is an entry, check key.
    // Create a trav pointer to the head of the linked list at the bucket slot
    node* trav = entry;
    while (trav != NULL)
    {

        // If there is a key match, key compares case sensitively
        if (strcmp(trav->entity_key, key) == 0)
        {

            // Return the entity description
            return trav->description_value;
        }

        // Else, traverse to the next linked entry
        trav = trav->next;
    }

    /* We reached the end of the list.
    There is an entry, but no matching key. Return NULL. */
    return NULL;
}

/*  This is a helper function to display the entity hashtable
 *  for debugging purposes.
 *
 *  It takes 1 arguments:
 *      1. The entity hashtable to be displayed.
 */
void display_entity_ht(ht* hashtable)
{

    // Iterate through the entity hashtable
    for (int i = 0; i < ENTITY_TABLE_SIZE; i++)
    {
        // If there is an entry in the entity hashtable
        if (hashtable->entries[i] != NULL)
        {
            printf("\thashtable[%i]: ", i);

            // Set a travesal pointer to the start of the linked list at bucket
            node* trav = hashtable->entries[i];

            // While there is a linked entry in the bucket
            while (trav != NULL)
            {

                // Print the contents
                printf("{ %s=%s } -> ", trav->entity_key, trav->description_value);

                // Set travesal to the next linked entry in bucket
                trav = trav->next;
            }

            printf("NULL\n");
        }
    }
}

/*  This is a helper function to Unload entity hash table from memory
 *
 *  It takes 1 arguments:
 *      1. The entity hashtable to unload.
 */
void unload_entity_ht(ht* hashtable)
{

    // Iterate through the entity hashtable
    for (int i = 0; i < ENTITY_TABLE_SIZE; i++)
    {

        // Go through an active bucket and free the entire list
        while (hashtable->entries[i] != NULL)
        {

            // Set a travesal pointer to the next item of the bucket
            node* trav = hashtable->entries[i]->next;

            // Free the entity key
            free((char *) hashtable->entries[i]->entity_key);

            // Free the description value
            free((char *) hashtable->entries[i]->description_value);

            // Free the existing entry in the bucket
            free(hashtable->entries[i]);

            // Set bucket pointer to trav
            hashtable->entries[i] = trav;
        }
    }

    // Free the entries array in struct
    free(hashtable->entries);

    // Free the hashtable struct itself
    free(hashtable);
}

/*  This function gets the entity hash table in the sections hash table
 *  with the given section key.
 *
 *  It takes 2 arguments:
 *      1. The sections hashtable.
 *      2. The section key.
 *
 *  It returns a ht* (an entity hash table) containing the entity description 
 *  key value pairs.
 * 
 *  If it is an entry that does not exist, NULL is returned.
 */
ht* section_ht_get(section_node* sections[], const char* section_key)
{

    // Determine bucket slot
    unsigned int bucket = section_hash(section_key);

    // Try to get an entry from the bucket
    section_node* entry = sections[bucket];

    // There is no entry, return null
    if (entry == NULL)
    {
        return NULL;
    }

    // There is an entry, check key
    section_node* trav = entry;
    while (trav != NULL)
    {

        // If there is a key match, compares case sensitively
        if (strcmp(trav->section_key, section_key) == 0)
        {

            // Return the entity hash table
            return trav->section_ht;
        }

        // Else, go to the next node
        trav = trav->next;
    }

    /* We reached the end of the list.
    There is an entry, but no matching key. Return NULL. */
    return NULL;
}

/*  This function sets the entity hash table with the given section key
 *  and the new entity hash table.
 *
 *  It takes 3 arguments:
 *      1. The sections hashtable.
 *      2. The section key.
 *      3. The entity hash table as the value.
 *
 *  It returns true if it is set, false if is not.
 * 
 *  Note: This function assumes that the key and the hash table are already
 *  created OUTSIDE of this function.
 * 
 *  Example usage:
 * 
 *  Steps to create / set a new section in the sections hash table:
 *      1. Determine section key.
 * 
 *          const char* key1 = "what";
 * 
 *      2. Create a new hashtable.
 * 
 *          ht* new_hashtable1 = create_entity_ht();
 * 
 *      3. Set the entity hash table value in the sections hashtable
 *
 *          section_ht_set(sections, key1, new_hashtable1);
 */
bool section_ht_set(section_node* sections[], const char* key, ht* hashtable)
{
    // Determine bucket slot
    unsigned int bucket = section_hash(key);

    // Try to get an section from the bucket
    section_node* section = sections[bucket];

    // There is no entry, create a new section and insert it into the sections hashtable
    if (section == NULL)
    {

        // create_section_entry() used to allocate memory for a section_node
        section_node* new_section = create_section_entry(key, hashtable);

        // If new_section is NULL, ran out of memory to allocate
        if (new_section == NULL)
        {
            return false;
        }

        sections[bucket] = new_section;
        return true;
    }

    // There is an a section, check for conflicting section
    section_node* trav = section;
    section_node* prev = section;
    while (trav != NULL)
    {

        // If there is a key match, we are replacing an entire section
        if (strcmp(trav->section_key, key) == 0)
        {

            // Unload the entity hash table
            unload_entity_ht(trav->section_ht);

            // Set new entity hash table
            trav->section_ht = hashtable;
            return true;
        }

        // Else, go to the next node
        prev = trav;
        trav = trav->next;
    }

    // Reached the end of the list for the bucket
    if (trav == NULL)
    {
        // Create new section and insert it into the list at bucket
        section_node* new_section = create_section_entry(key, hashtable);

        // If new_section is NULL, ran out of memory to allocate
        if (new_section == NULL)
        {
            return false;
        }

        prev->next = new_section;
    }

    return true;
}

/* Helper function used to allocate memory for an section in the section hashtable
 * It takes as its arguments:
 *  1. key - section key string
 *  2. hashtable - the new entity description hashtable
 */
section_node* create_section_entry(const char* key, ht* hashtable)
{

    // Allocate memory for the new section
    section_node* section = malloc(sizeof(section_node));

    // Check for sufficient memory.
    if (section == NULL)
    {
        printf("Ran out of memory.\n No memory is allocated.\n");
        return NULL;
    }

    // Set section_node struct member fields
    section->section_key = key;
    section->section_ht = hashtable;
    section->next = NULL;

    return section;
}

/*  This is a helper function to display the section hashtable
 *  for debugging purposes.
 *
 *  It takes 1 arguments:
 *      1. The section hashtable to be displayed.
 */
void display_section_ht(section_node* section[])
{

    // Iterate through sections hashtable
    for (int i = 0; i < SECTION_TABLE_SIZE; i++)
    {

        // While there is a section in the bucket
        if (section[i] != NULL)
        {

            // Display section contents
            printf("section %i:\n", i);

            // Set a travesal pointer to the head of the list for the index / bucket
            section_node* trav = section[i];

            // While the end of the list is not reached for the current index
            while (trav != NULL)
            {

                // Display the key
                printf("------------------------------------------------------------------------------------------------------\n%s:", trav->section_key);

                // Call display_entity_ht() to display the entity hashtable
                display_entity_ht(trav->section_ht);
                printf("------------------------------------------------------------------------------------------------------\n");

                trav = trav->next;
            }
        }
    }
}

/*  This is a helper function to unload sections hash table from memory
 *
 *  It takes 1 arguments:
 *      1. The section hashtable to unload.
 */
void unload_section_ht(section_node* section[])
{

    // Iterate through the sections hashtable
    for (int i = 0; i < SECTION_TABLE_SIZE; i++)
    {
        section_node* trav = section[i];

        // While there is a valid section node in the current bucket linked list
        while (trav != NULL)
        {

            // Set trav pointer to the next section node
            trav = trav->next;

            // Free the section key
            free((char *) section[i]->section_key);

            // Free the entity hash table
            unload_entity_ht(section[i]->section_ht);

            // Free the section node
            free(section[i]);

            // Move pointer to the next section node
            section[i] = trav;
        }
    }
}

// Hashes a word to a number
unsigned int hash(const char* word, unsigned int max_table_size)
{
    // credits goes to djb2 hash function from http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    int c;

    while ((c = *word++))
    {
        hash = ((hash << 5) + hash) + tolower(c); // hash * 33 + c //
    }

    return hash %  max_table_size;
}

// Hashes entity string to a number
unsigned int entity_hash(const char* word)
{
    return hash(word, ENTITY_TABLE_SIZE);
}

// Hashes section string to a number
unsigned int section_hash(const char* word)
{
    return hash(word, SECTION_TABLE_SIZE);
}
