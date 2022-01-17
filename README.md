# ICT1002_C_GP Team 15 C Project (Chatbot)

## Description of the chatbot

The project is a simple chatbot that can respond to simple queries made up of
a single question word (called the intent) and an object (called the entity). 
The chatbot will be able to answer questions such as Where is the printer? or What is C? by identifying 
the intent (where or what) and entity (printer or C) then looking up the answer corresponding 
to this pair in a simple database.

The chatbot will also be able to learn new answers by asking questions of the user. 
If a user asks a question for which the database does not contain an answer, the user will be given 
the option to supply an answer that can be used to respond to the same question in future. 
(This simulates having the chatbot refer the unknown question to a third party who does know the answer.)

### Description of each file:

- chat1002.h
	- This header file contains the macro definitions for input lengths and function return codes, 
	as well as function prototypes provided in the skeleton code. 

- datastructure.h
	- This header file contains the structure definition for the data structure implemented for
	the knowledge base to store the "intents", "entities" and "descriptions". 
	The data structure implementation is a nested hash table. We have an outer hash table of "sections", 
	it contains key value pairs of strings as the key (section key), and the value as the hash table 
	for the entries for the specified section (1 section key to 1 hash table). This inner hash table 
	contains the entries for the specified section. This inner hash table contains key value pairs 
	of strings as the key (the entity), and the value as the description for the entity, which is
	also a string.

	- This header file also contains the macro definitions used for setting the size of the hash
	table as well as function prototypes for the data structure operations. 
	These operations included getter and setter functions, as well as helper functions such as 
	displaying the hash table contents as well as unloading the data structure 
	(freeing the allocated memory).

- main.c
	- This is the source file for the main execution of the chatbot program. It calls chatbot_main()
    	which is defined in chatbot.c.

- chatbot.c
	- This is the source file that contains the main execution of the chatbot. 
	It contains the logic for the chatbot as well as the function declarations for the data structure 
	operations. Chatbot_main() also initializes the data structure for use during its first call.

- knowledge.c
	- This is the source file that contains the function declarations for handling the knowledge
	base operations.

- sample.ini
	- This is a sample test file for use to try the chatbot program :)
