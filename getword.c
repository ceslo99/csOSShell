/*
 * Professor Carroll
 * CS570
 * 9/4/2019
 * Cesar Lopez
 *
 * GraderNotes:
 * I wrote this code myself
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

    // Store character from getchar()
    int characterValue;

    // count number of letters of word
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

    // First Character is new-line
    if (characterValue == '\n' ) {
        *w = '\0';
        return 0;
    }

    // First character is End Of File
    if (characterValue == EOF ) {
        *w = '\0';
        return -1;
    }
    //* added new *//

    if(characterValue == '<' || characterValue == '|' || characterValue == '#' || characterValue == '&' ){
        // Word is found, terminate with end of string
        *w = characterValue;
        *w++;
        letterCounter++;
        *w = '\0';
        return letterCounter;
    }

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
        ungetc(characterValue, stdin);
        return letterCounter;

    }

    if(characterValue == '\\'){
        characterValue = getchar();
    }

        //stores character to pointer
        *w = characterValue;
        //moves pointer ahead one
        *w++;
        //increments letterCounter by one
        letterCounter++;


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

        if(characterValue == '<' || characterValue == '|' || characterValue == '#' || characterValue == '&' ){
            // Word is found, terminate with end of string
            *w = '\0';
            //need to add 'back metacharacters' to be detected
            ungetc(characterValue, stdin);
            return letterCounter;
        }

        if(characterValue == '>' ){
            // Word is found, terminate with end of string
            *w = '\0';

            //need to add '>' to be detected
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
            //need to add 'New Line' to be detected
            ungetc('\n', stdin);
            return letterCounter;
        }

        if(characterValue == '\\'){
            characterValue = getchar();
        }

        if( characterValue != '\n'){
            //stores character to pointer
            *w = characterValue;
            //moves pointer ahead one
            *w++;
            //increments letterCounter by one
            letterCounter++;


       }
        else{
            ungetc('\n', stdin);
        }

        if(letterCounter >= 254){
            return letterCounter;
        }

    }

    *w = '\0';
    if( strcmp(p, "done") == 0){
        return -1;
    }
    // Check for left over word
    if(letterCounter != 0){
        return letterCounter;
    }

    return -1;
}
