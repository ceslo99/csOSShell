/*
 * Professor Carroll
 * CS570
 * 9/4/2019
 * Cesar Lopez
 *
 * GraderNotes:
 * I wrote this code myself and no bugs
 *Sources:
 * C by Discovery by Foster
 *
 * */
#include "getword.h"

/*
 * Will grab user input and return each word found.
 * If "done" found, it will terminate the program or until it finds EOF
 * Returning integer. Return [-1] to terminate file
 *
 * Parameters
 * char *w: is the pointer the beginning of a char array of size 255  */
int getword(char *w){

    // Store character from getchar() user stdin
    int characterValue;

    // counter for number of letters of word
    int letterCounter = 0;

    //pointer to the beginning of w always
    char *p = w;

    // Gets leading character from STDIN
    characterValue = getchar();

    /* Base cases */
    // Move forward until spaces are eliminated
    while(characterValue == ' '){
        characterValue = getchar();
    }

    // First Character is new-line then return 0
    if (characterValue == '\n' ) {
        *w = '\0';
        return 0;
    }

    // First character is End Of File return -1
    if (characterValue == EOF ) {
        *w = '\0';
        return -1;
    }
    //* added new *//

    // First character is a metacharacter. Checking for only single metacharacters.
    // add to buffer, increment counter by 1 and return letter.
    if(characterValue == '<' || characterValue == '|' || characterValue == '#' || characterValue == '&' ){
        // Word is found, terminate with end of string
        *w = characterValue;
        *w++;
        letterCounter++;
        *w = '\0';
        return letterCounter;
    }
    

    // First character is a metacharacter with possible longer metacharacter to be found.
    // keeps adding metacharacter to buffer until it finds something else besides a metacharacter.
    //
    if( characterValue == '>' ){
        *w = characterValue;
        *w++;
        letterCounter++;
        characterValue = getchar();
        if(characterValue == '&'){
            *w = characterValue;
            *w++;
            letterCounter++;
            *w = '\0';
            return letterCounter;
        }
        else if(characterValue == '>'){
            *w = characterValue;
            *w++;
            letterCounter++;
            characterValue = getchar();
            if(characterValue == '&'){
                *w = characterValue;
                *w++;
                letterCounter++;
                *w = '\0';
                return letterCounter;
            }
        }

        *w = '\0';
        //need to add the characterValue it just got from stdin to be detected again since getchar() removes it from stdin
        ungetc(characterValue, stdin);
        return letterCounter;

    }

    // First character is a '\' grabs rhe next character and next few lines stores it into buffer
    if(characterValue == '\\'){
        characterValue = getchar();
    }

    //stores character to pointer
    *w = characterValue;
    //moves pointer ahead one
    *w++;
    //increments letterCounter by one
    letterCounter++;
    
    // Continues while loop until EOF is found
    while( (characterValue = getchar()) != EOF ){

        // Checks if space is found
        if(characterValue == ' '){
            // Word is found, terminate with end of string
            *w = '\0';
            // Check if word is "done"
            if( strcmp(p, "done") == 0){
                return -1;
            }
            return letterCounter;
        }

        //* added new *//

        // Checks if single metacharacter is found. Similar to finding a space. We want to return the previous word 
        // and ungetc the metacharacter to be found again.
        if(characterValue == '<' || characterValue == '|' || characterValue == '#' || characterValue == '&' ){
            // Word is found, terminate with end of string
            *w = '\0';
            //need to add 'back metacharacters' to be detected again since getchar() removes it from stdin
            ungetc(characterValue, stdin);
            return letterCounter;
        }

        // Checks if single metacharacter is found. Similar to finding a space. We want to return the previous word 
        // and ungetc the metacharacter to be found again and start to search for longest possible metacharacter above
        if(characterValue == '>' ){
            // Word is found, terminate with end of string
            *w = '\0';

            //need to add '>' to be detected again since getchar() removes it from stdin
            ungetc('>', stdin);
            return letterCounter;
        }

        // Check if new line is found then insert new line again
        if(characterValue == '\n'){
            // Word is found, terminate with end of string
            *w = '\0';
            // Check if word is "done"
            if( strcmp(p, "done") == 0){
                return -1;
            }
            //need to add 'New Line' to be detected again since getchar() removes it from stdin
            ungetc('\n', stdin);
            return letterCounter;
        }
        
        // checks for '\' and grabs the next character.
        if(characterValue == '\\'){
            characterValue = getchar();
        }

        //Next code will check if it is a '\n' in order to add it to the buffer or not.
        if( characterValue != '\n'){
            //stores character to pointer
            *w = characterValue;
            //moves pointer ahead one
            *w++;
            //increments letterCounter by one
            letterCounter++;


       }
        // if it is '\n' then put it back to be found again
        else{
            // Need to add 'New Line' to be detected again since getchar() removes it from stdin
            ungetc('\n', stdin);
        }

        // Checks if buffer is filled. If it is, then we return the word and keep reading in stdin
        if(letterCounter >= 254){
            return letterCounter;
        }

    }

    // Sets end of string since it is out of while loop and might still have word in buffer for next few checks
    *w = '\0';

    // Checks if word is "done", if last character is the EOF signal since got out of while loop above
    if( strcmp(p, "done") == 0){
        return -1;
    }

    // Check for left over word. If there is, then returns that word and will e xit on above Base cases
    if(letterCounter != 0){
        return letterCounter;
    }
    

    // Return -1 if there is no words left in buffer
    return -1;
}
