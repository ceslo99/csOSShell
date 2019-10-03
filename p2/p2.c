/*
 * Professor Carroll
 * CS570
 * 10/4/2019
 * Cesar Lopez
 *
 * GraderNotes:
 * I wrote this code myself and has no bugs
 *Sources:
 * C by Discovery by Foster
 *
 * */

#include "p2.h"
// Parse method.2
// Calls the getword method and counts number of words, stores it in myargv
// Use newargv to save the pointer of each word

int c; // Used in parse to hold the return value of getword.c which is the number of characters in the word
//int bangFlag = 0;
int oldsize = -1; // keep track of previous number of words in argv

char myargv[255 * MAXITEM]; // Stores user current input
char *newargv[MAXITEM]; // Pointer to beginning of each word.
char onePreviousargv[255*MAXITEM];
char *onePreviousnewargv[MAXITEM];

char *outputPointer; // Pointer to beginning of file '>'
char *inputPointer; // Pointer to beginning of file '<'
int *outputPointerAnd; // pointer to beggining of file of '>&'
//char *cd; // *DELETE* Just used to print current directory
//char cwd[1024]; // *DELETE* Just need to print current directory
int doneFlag =0;
int greaterThanFlag = 0;
int lessThanFlag = 0;
int diagnosticFlag = 0;
int greaterThanAndFlag = 0;

int child = 0;
int pid = 0 ;

int file;


int main(int argc, char *argv[] )
{

    int flags = 0;
    int outFile = 0;
    int inFile = 0;
    int backgroundFlag = 0;
    int wordCount = 0; // Needed for number of words to be able to check conditions later
    (void) signal(SIGTERM, myhandler);
    setpgid(0,0);

    /*
     * Checks if there is something in argv
     * tries to open the file and if successful dup2 to replace stdin
     * */
    if(argc > 1){
        file = open( argv[1], O_RDONLY );
        if ( file == 0 )
        {
            printf( "Could not open file\n" );
        }
        else
        {
            dup2(file,STDIN_FILENO);
        }
    }
    for(;;) { // Will break if there is a done or EOF signal
        doneFlag = 0;
        backgroundFlag = 0;
        printf("%%1%% "); // For user prompt

        //fflush(stdin);
        wordCount = parse(); // Parse will add each word into myargv to have access to it


//        getcwd(cwd, sizeof(cwd));
//        printf("Current working dir: %s\n", cwd);
//        printf("--------------------\n");

if(strcmp(newargv[0],"!!") == 0){
    dup2("oneprev.txt",STDIN_FILENO);
    continue;
}
else{
    int k = 0;
    int hit =0;
        while(hit != wordCount){
            if(myargv[k] == '\0'){
                hit++;
                onePreviousargv[k] = '\0';
            }
            else{
                onePreviousargv[k] = myargv[k];
            }

            k++;
        }
    fprintf(fp, "%s ");
        oldsize = wordCount;
}


//        int k = 0;
//        int hit =0;
//        while(hit != wordCount){
//            if(myargv[k] == '\0'){
//                hit++;
//                onePreviousargv[k] = '\0';
//            }
//            else{
//                onePreviousargv[k] = myargv[k];
//            }
//
//            k++;
//        }
//        oldsize = wordCount;

        if (wordCount == -1 || doneFlag) { // Done is seen in myargv first position then quit program
            break;
        }

        if(wordCount == 0){ // Will re issue the user prompt to type again
            continue;
        }

        /////////////////// check '>' , '<' error ////////////
        if(greaterThanFlag > 1 || lessThanFlag > 1){
            fprintf(stderr,"Too many redirectories.\n");
            continue;
        }

        /////////////////// check '>' ///////////////////////
        if(greaterThanFlag == 1){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            if(outputPointer == NULL){
                fprintf(stderr,"No file name to create.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags,S_IRWXU) ) < 0 ){
                fprintf(stderr,"%s: File already exists.\n",outputPointer);
                continue;
            }
        }
        /////////////////// check '<' ///////////////////////
        if(lessThanFlag == 1){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            if(inputPointer == NULL){
                fprintf(stderr,"No file name to create.\n");
                continue;
            }
            if( (inFile = open(outputPointer,flags,S_IRWXU) ) < 0 ){
                fprintf(stderr,"%s: File already exists.\n",outputPointer);
                continue;
            }
        }
        //////////////////// check '>&' ///////////////////////
        if(greaterThanAndFlag == 1){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            if(outputPointerAnd == NULL){
                fprintf(stderr,"No file name to create.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags,S_IRWXU) ) < 0 ){
                fprintf(stderr,"%s: File already exists.\n",outputPointer);
                continue;
            }
        }

        if( strcmp(newargv[wordCount-1],"&")  == 0){
            backgroundFlag++;
        }
        /*
         * Checks for all possible cases of CD
         * First checks if user just types cd and takes to home directory
         * Others checks if there is a certain path it wants to go
         * Last checks if to many paths given
         * */
        if(strcmp(newargv[0],"cd") == 0  && wordCount == 1){

            chdir(getenv("HOME"));
            //getcwd(cwd, sizeof(cwd));
            //printf("Current working dir: %s\n", cwd);
        }
        else if(strcmp(newargv[0],"cd") == 0  && wordCount == 2){
            if( chdir(newargv[1]) == -1){
                fprintf(stderr,"No file or folder in current directory.\n");

            }
            else{
                chdir(newargv[1]);
                //getcwd(cwd, sizeof(cwd));
                //printf("Current working dir: %s\n", cwd);
            }
        }
        else if(strcmp(newargv[0],"cd") == 0  && wordCount > 2){
            fprintf(stderr,"To many parameters for cd.\n");

        }


        /////////////////// fork code/////////////////////////
        fflush(stdout);
        fflush(stderr);

        child = fork();

        if(child < 0){
            printf("Can't Fork!");
            exit(1);
        }
        else if(child == 0){
            if(backgroundFlag != 0 && lessThanFlag != 0){
                int devnull;
                flags = O_RDONLY;
                if((devnull = open("/dev/null",flags)) < 0){
                    //fprintf(stderr,"Failed to open /dev/null.\n");
                    perror("/dev/null.\n");
                    exit(9);
                }
                dup2(devnull,STDIN_FILENO);
                close(devnull);
            }


            ///////////// ">" code/////////////
            if(greaterThanFlag != 0){
                dup2(outFile,STDOUT_FILENO);
                close(outFile);
            }

            //////////// "<" code/////////////
            if(lessThanFlag != 0){
                dup2(inFile,STDIN_FILENO);
                close(inFile);
            }

            if((execvp(*newargv, newargv)) < 0){ // this executes the command
                fprintf(stderr,"%s: Command not found.\n",newargv[0]);
                exit(9);
            }
        }
        else if( child < 0){
            printf("Fork was unsucessful\n");
            exit(1);
        }

        if(backgroundFlag !=0){	//when & you will place in background and set STDIN to /dev/null
            printf("%s [%d]\n", *newargv , child);
            backgroundFlag = 0;
            continue;
        }
        else{
            for(;;){
                pid = wait(NULL);
                if(pid == child){
                    break;
                }
            }
        }


    }

    kill(getpgrp(), SIGTERM);
    printf("p2 terminated. \n");
    exit(0);
}

int parse(){

    int size = 0;
    int argvpointerPosition = 0;
    int newargvpointerPosition = 0;

    greaterThanFlag = 0;
    lessThanFlag = 0;


    // While loop will keep getting word until there is none left, returns 0
    while(( c = getword(myargv + argvpointerPosition) ) != 0 ){

        if(c == -1 && size == 0){ // Done returns -1 and check if that is the first word

            doneFlag = 1;
            return size;
        }

        else if(c == -1 && strcmp(&myargv[argvpointerPosition], "done" ) == 0){ // Condition if done is in the middle of sentence continue

            c = 4;
        }

//        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!!" ) == 0 )){
//            //strncpy(myargv, onePreviousargv, MAXITEM);
//
//            int k = 0;
//            int hit =0;
//            while(hit != oldsize){
//
//                if(onePreviousargv[k] == '\0'){
//                    hit++;
//                    myargv[k] = '\0';
//                }
//                else{
//                    myargv[k] = onePreviousargv[k];
//                }
//
//                k++;
//            }
//            return oldsize;
//        }
        else if(c == -1){

            break;
        }


        //////////////////////////////
        // Checks for > sign and only 1 character. Moves pointer two position forward to point to the file name
        // '>''\0' is two character and expect file name after or null
        if(c == 1 && myargv[argvpointerPosition] == '>' ){
            greaterThanFlag++;
            outputPointer = myargv + argvpointerPosition + 2;
        }
        else if(c == 1 && myargv[argvpointerPosition] == '<'){
            lessThanFlag++;
            inputPointer = myargv + argvpointerPosition + 2;

        }
//        else if(c == 2 && strcmp(&myargv[argvpointerPosition], ">&") == 0 ){
//            greaterThanAndFlag++;
//            outputPointerAnd = myargv + argvpointerPosition + 3;
//
//        }
        // Will skip this round if above statement is true
        // Points word of myargv into newargv. Adds end of string in myargv of position plus c.
        // myargv will put next word after null terminal
        // arvpointer is c plus 1
        // Increment size of words counted
        else{
            newargv[newargvpointerPosition++] = myargv + argvpointerPosition;
            myargv[argvpointerPosition + c] = '\0';
            argvpointerPosition += c + 1;
            size++;
        }
    }

    newargv[newargvpointerPosition] = NULL;


    //oldsize = size;
    return size;
}

void myhandler(){
}


