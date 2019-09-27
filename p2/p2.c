/*
* Cesar Lopez
* CS570
* Professor Carroll
* */

#include "p2.h"
// Parse method.
// Calls the getword method and counts number of words, stores it in argv
// Use newargv to save the pointer of each word

int parse();
int c; //
char argv[255 * MAXITEM]; // Stores user input
char *newargv[MAXITEM]; // Pointer to beginnign of each word.
char *cd; // *DELETE* Just used to print current directory
char cwd[1024]; // *DELETE* Just need to print current directory
int doneFlag =0;
int main()

{
    int wordCount = 0;
    for(;;) {
        doneFlag = 0;
        printf("%%1%% ");
        wordCount = parse();
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
        printf("--------------------\n");

        if(wordCount == 0){
            continue;
        }

        if(strcmp(newargv[0],"cd") == 0  && wordCount == 1){

            chdir(getenv("HOME"));
            getcwd(cwd, sizeof(cwd));
            printf("Current working dir: %s\n", cwd);
        }
        else if(strcmp(newargv[0],"cd") == 0  && wordCount == 2){
            if( chdir(newargv[1]) == -1){
                printf("No file or folder in current directory\n");
            }
            else{
                chdir(newargv[1]);
                getcwd(cwd, sizeof(cwd));
                printf("Current working dir: %s\n", cwd);
            }
        }
        else if(strcmp(newargv[0],"cd") == 0  && wordCount > 2){
            printf("\nTo many parameters for cd\n" );
        }

        if (wordCount == -1 || doneFlag ) {
            break;
        }

    }

    return 0;
}

int parse(){

    int size = 0;
    int argvpointerPosition = 0;
    int newargvpointerPosition = 0;

    while(( c = getword(argv + argvpointerPosition) ) != 0){
        if(c == -1){
            doneFlag = 1;
            return size;
        }
        else {
            newargv[newargvpointerPosition++] = argv + argvpointerPosition;
            argv[argvpointerPosition + c] = '\0';
            argvpointerPosition += c + 1;
            size++;
        }
    }

    newargv[newargvpointerPosition] = NULL;

    return size;
}

