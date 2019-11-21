/*
* Cesar Lopez
* Professor Carroll
* CS570
* 10/9/2019
*
* GraderNotes:
* I wrote this code myself and has no bugs
* Sources:
* C by Discovery by Foster
*
* */

/* Parse method
 * Calls the getword method and counts number of words, stores it in myargv
 * Use newargv to save the pointer of beginning of word in myargv
 * Sets flags for certain cases for bang bang, >, <, >&
 * */

#include "p2.h"
void saveHistory(int commandcounter, int wordcount);
int historyparse(char numberargv[STORAGE * MAXITEM], int wordc);

int c; // Used in parse to hold the return value of getword.c which is the number of characters in the word
int oldsize = -1; // keep track of previous number of words in argv

char myargv[STORAGE * MAXITEM]; // Stores user current input
char *newargv[MAXITEM]; // Pointer to beginning of each word.

char oneargv[STORAGE * MAXITEM];
char twoargv[STORAGE * MAXITEM];
char thirdargv[STORAGE * MAXITEM];
char fourthargv[STORAGE * MAXITEM];
char fifthargv[STORAGE * MAXITEM];
char sixthargv[STORAGE * MAXITEM];
char seventhargv[STORAGE * MAXITEM];
char eighthargv[STORAGE * MAXITEM];
char ninthargv[STORAGE * MAXITEM];

int onewordcounter = 0;
int twowordcounter = 0;
int thirdwordcounter = 0;
int fourthwordcounter = 0;
int fifthwordcounter = 0;
int sixthwordcounter = 0;
int seventhwordcounter = 0;
int eightwordcounter = 0;
int ninthwordcounter = 0;


char *outputPointer; // Pointer to beginning of file '>'
char *inputPointer; // Pointer to beginning of file '<'
int doneFlag =0; // If user types done at the beginning of input
int greaterThanFlag = 0; // How many redirects counter for >
int lessThanFlag = 0; // How many redirects counted for <
int diagnosticFlag = 0; // how many >& found
int bangFlag = 0; // Found !! at the beginning of input
int child = 0; // Used to save forked process PID
int pid = 0 ; // Used to save wait return value


int file; // Save file if given in argument
int commandCounter = 1;
int commandCounterArray = 0;

int appendFlag = 0;
int bangCounter = 0;
int appenddiagnosticFlag = 0;
int argvFlag = 0;
int poundFlag = 0;


/*
 * first checks if there is anything in argv[1]. If there isn't anything then enters loop and calls parse.
 * checks for special conditions and will break when finds done is first word or EOF
 * Contains previous getword.c and worked on p2.c which is the main function. Program read input from user
 * or file passed in argv. Input will be handled by previous getword.c that is in the parse method.
 * This program will read input from a user or handle a file passed in(argv). This command will be
 * passed to the parse function below, which calls our getword.c.
 * Able to handle change directory by user typing 'cd' along with path or folder. Handles errors if user
 * inputs to many files.
 * Also, handled redirectories >, < and background processes.
 * */

int main(int argc, char *argv[] )
{

    int devnull; // Saves open dev/null
    int flags = 0; // Flags for open, create, read ,write
    int mode = 0; // Mode of open files
    int outFile = 0; // Saves file that is open
    int inFile = 0; // Saves file that is open
    int backgroundFlag = 0; // & counter
    int wordCount = 0; // Needed for number of words to be able to check conditions later
    setpgid(0,0);

    /*
     * Checks if there is something in argv
     * tries to open the file and if successful dup2 to replace stdin for file
     * */

    if(argc > 1){
        file = open( argv[1], O_RDONLY );
        if ( file < 0 )// If file doesn't exist
        {
            perror(argv[1]);
        }
        else
        {
            argvFlag++;
            dup2(file,STDIN_FILENO);

        }
    }
    for(;;) { // Will break if there is a done or EOF signal
        doneFlag = 0;
        diagnosticFlag = 0;
        appenddiagnosticFlag = 0;
        appendFlag = 0;
        backgroundFlag = 0;
        if(argvFlag == 0){
            printf("%%%d%% ",commandCounter); // For user prompt
        }

        wordCount = parse(); // Parse will add each word into myargv to have access to it

        /*
         * Used to store previous user input as soon as it returns from parse.
         * since we place '\0' after each word, we use word count to find all
         * we do this since  normally it would stop the loop after it finds the first '\0'
         * saves every char in onepreviousargv
         * */


        if (wordCount == -1 || doneFlag) { // Done is seen in myargv first position then quit program
            break;
        }

        if(wordCount == 0){ // Will re issue the user prompt to type again
            continue;
        }

        saveHistory(commandCounterArray, wordCount);
        commandCounter++;

        /* Checks if last char in newargv is '&' to run process in the background.
         * in order to pass future cases must decrement wordcount and update backgroundFlag
         * */

        if( strcmp(newargv[wordCount-1],"&")  == 0){
            backgroundFlag++;
            newargv[wordCount -1] = NULL; //remove ampersand and put a null there
            wordCount--;
        }

        /*
         * Checks for all possible cases of CD
         * First checks if user just types cd and takes to home directory
         * Others checks if there is a certain path it wants to go
         * Last checks if to many paths given
         * if cd is not given then fork!
         * */

        if(strcmp(newargv[0],"cd") == 0  && wordCount == 1){

            chdir(getenv("HOME"));
            continue;
        }

        else if(strcmp(newargv[0],"cd") == 0  && wordCount == 2){
            if( chdir(newargv[1]) == -1){ //changes directory in this line but if -1 then report error.
                perror("No folder in current directory.\n");
                continue;
            }
            else{
                continue;
            }
        }

        else if(strcmp(newargv[0],"cd") == 0  && wordCount > 2){

            perror("To many parameters for cd.\n");
            continue;
        }

        /*
         * Checks if there is errors of input output first
         * If to many parameters, pointed to a NULL file or already exist, don't overwrite
         * open files for both input and output if nothing fails
         * */

        //// check '>' , '<' error
        // if to many directional arguments, error and continue
        if(greaterThanFlag > 1 || lessThanFlag > 1 || appendFlag > 1 ||
           (greaterThanFlag != 0 && appendFlag != 0) || (diagnosticFlag != 0 && appenddiagnosticFlag != 0)){
            fprintf(stderr,"Too many redirectories.\n");
            continue;
        }


        //// check '>'
        if(greaterThanFlag == 1 && diagnosticFlag == 0 && appenddiagnosticFlag == 0){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(outputPointer == NULL){ // if there is no file to point then error
                fprintf(stderr,"No file name to create.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags,mode) ) < 0 ){ // checks if file doesn't exist
                perror(outputPointer);
                continue;
            }
        }

        //// check '>>'
        if(appendFlag == 1 && diagnosticFlag == 0){
            flags = O_APPEND | O_RDWR ;
            //mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(outputPointer == NULL){ // if there is no file to point then error
                fprintf(stderr,"No file name to open.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags) ) < 0 ){ // checks if file doesn't exist
                perror(outputPointer);
                continue;
            }
        }


        //// check '<'
        if(lessThanFlag == 1){
            flags = O_RDONLY;
            if(inputPointer == NULL){ // if there is no file to point then error
                fprintf(stderr,"No file name to create.\n");
                continue;
            }
            if ((inFile = open(inputPointer, flags)) == -1) { // checks if file doesn't exist
                perror(inputPointer);
                continue;
            }
        }

        //// check '>&'
        if( diagnosticFlag != 0 ) {
            flags = O_CREAT | O_EXCL | O_RDWR ;
            mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(outputPointer == NULL){ // if there is no file to point then error
                fprintf(stderr,"No file name to create.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags,mode) ) < 0 ){ // checks if file doesn't exist
                perror(outputPointer);
                continue;
            }
        }

        //// check '>>&'
        if( appenddiagnosticFlag != 0 ) {
            flags = O_APPEND | O_RDWR ;
            //mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(outputPointer == NULL){ // if there is no file to point then error
                fprintf(stderr,"No file name to open.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags) ) < 0 ){ // checks if file doesn't exist
                perror(outputPointer);
                continue;
            }
        }


        //// fork code
        fflush(stdout);
        fflush(stderr);

        child = fork();


        if(child < 0){
            printf("Fork was unsucessful\n");
            exit(1);
        }
        else if(child == 0){
            if(backgroundFlag != 0 && lessThanFlag == 0){
                flags = O_RDONLY;
                if((devnull = open("/dev/null",flags)) < 0){
                    perror("/dev/null.\n");
                    exit(9);
                }
                dup2(devnull,STDIN_FILENO);
                close(devnull);
            }

            //// ">" code
            if(greaterThanFlag != 0 && diagnosticFlag == 0){
                dup2(outFile,STDOUT_FILENO);
                close(outFile);
            }
            //// ">>" code
            if(appendFlag != 0 && diagnosticFlag == 0){
                dup2(outFile,STDOUT_FILENO);
                close(outFile);
            }

            //// "<" code
            if(lessThanFlag != 0){
                dup2(inFile,STDIN_FILENO);
                close(inFile);
            }
            //// ">&"
            if(diagnosticFlag != 0){
                dup2(outFile,STDOUT_FILENO);
                dup2(outFile,STDERR_FILENO);
                close(outFile);
            }
            //// ">&"
            if(appenddiagnosticFlag != 0){
                dup2(outFile,STDOUT_FILENO);
                dup2(outFile,STDERR_FILENO);
                close(outFile);
            }
            if((execvp(*newargv, newargv)) < 0){ // this executes the command
                fprintf(stderr, "%s Command not found. \n",newargv[0]);
                exit(9);
            }

        }
        // place in background and set STDIN set to/dev/null when & is present
        if(backgroundFlag !=0){
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

    killpg(getpgrp(), SIGTERM);
    if(argvFlag == 0){
        printf("p2 terminated.\n");
    }

    exit(0);
}

/*
*  Calls getword and able to return size of word.
*  especial cases if it runs to done, eof, !! and metacharacters
*  used to put user input in myarvg and newargv
*/
int parse(){

    int size = 0;
    int argvpointerPosition = 0;
    int newargvpointerPosition = 0;
    int simpleboolean = 0;
    poundFlag = 0;
    outputPointer = NULL;
    inputPointer = NULL;


    greaterThanFlag = 0;
    lessThanFlag = 0;


    // While loop will keep getting word until there is none left, returns 0
    while(( c = getword(myargv + argvpointerPosition) ) != 0 ){
        //if bang bang found then we want to use old newargv so just continue
        if(bangFlag != 0){
            continue;
        }
        if(poundFlag != 0){
            continue;
        }
        if( c == 1 && argvFlag != 0 && myargv[argvpointerPosition] == '#' ){
            poundFlag++;
            continue;
        }

        if(c == -1 && size == 0){ // Done returns -1 and check if that is the first word

            doneFlag = 1;
            return size;
        }

            // Condition if done is in the middle of sentence continue
        else if(c == -1 && strcmp(&myargv[argvpointerPosition], "done" ) == 0){
            c = 4;
        }
            //allows to continue future words
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!!" ) == 0 )){
           // bangFlag++;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!1" ) == 0 )){
            //bangFlag++;
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(oneargv, onewordcounter);


            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!2" ) == 0 )){
//            bangFlag++;
//            bangCounter = 1;
//            continue;
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(twoargv, twowordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!3" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 2 );
            commandCounterArray--;
            return historyparse(thirdargv, thirdwordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!4" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(fourthargv, fourthwordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!5" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(fifthargv, fifthwordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!6" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(sixthargv, sixthwordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!7" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(seventhargv, seventhwordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!8" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(eighthargv, eightwordcounter);
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!9" ) == 0 )){
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandCounterArray, 1 );
            commandCounterArray--;
            return historyparse(ninthargv, ninthwordcounter);
        }


        else if(c == -1){ // eof found

            break;
        }

        // Checks for > sign and . leaves pointer in same spot, next round will be replaced by file name
        // since not adding into and pointing at same location
        if(c == 1 && myargv[argvpointerPosition] == '>' ){
            greaterThanFlag++;
            simpleboolean = 2;
            outputPointer = myargv + argvpointerPosition+2;
        }
        else if(c == 2 && strcmp(&myargv[argvpointerPosition], ">>") == 0){
            appendFlag++;
            simpleboolean = 2;
            outputPointer = myargv + argvpointerPosition+3;
        }
        else if(c == 1 && myargv[argvpointerPosition] == '<'){
            lessThanFlag++;
            simpleboolean = 2;
            inputPointer = myargv + argvpointerPosition+2;

        }

        else if(c == 2 && strcmp(&myargv[argvpointerPosition], ">&") == 0 ){
            diagnosticFlag++;
            greaterThanFlag++;
            simpleboolean = 2;
            outputPointer = myargv + argvpointerPosition +3;

        }
        else if(c == 3 && strcmp(&myargv[argvpointerPosition], ">>&") == 0 ){
            appenddiagnosticFlag++;
            greaterThanFlag++;
            simpleboolean = 2;
            outputPointer = myargv + argvpointerPosition +4;

        }
            // Will skip this round if above statement is true
            // Points word of myargv into newargv. Adds end of string in myargv of position plus c.
            // myargv will put next word after null terminal
            // arvpointer is c plus 1
            // Increment size of words counted
        else if(simpleboolean < 1 ){
            newargv[newargvpointerPosition++] = myargv + argvpointerPosition;
            size++;
        }

        myargv[argvpointerPosition + c] = '\0';
        argvpointerPosition += c + 1;
        if(simpleboolean > 0){
            simpleboolean--;
        }


    }

    newargv[newargvpointerPosition] = NULL;

    oldsize = size; // stores old size before returning
    if(outputPointer != NULL){ // issue where it was ampersand was being saved as output file
        if(strcmp(outputPointer,"&") == 0){
            outputPointer = NULL;
        }
    }
    return size;
}

void myhandler(){
}



void saveHistory(int commandcounter, int wordcount){
    int k = commandcounter%10;

    if(k == 0 ){
        memcpy(oneargv, myargv, (STORAGE * MAXITEM));
        onewordcounter = wordcount;
    }
    if(k == 1 ){
        memcpy( twoargv, myargv, (STORAGE * MAXITEM));
        twowordcounter = wordcount;
    }
    if(k == 2 ){
        memcpy(thirdargv, myargv, (STORAGE * MAXITEM));
        thirdwordcounter = wordcount;
    }
    if(k == 3 ){
        memcpy(fourthargv, myargv, (STORAGE * MAXITEM));
        fourthwordcounter = wordcount;
    }
    if(k == 4 ){
        memcpy(fifthargv, myargv, (STORAGE * MAXITEM));
        fifthwordcounter = wordcount;
    }
    if(k == 5 ){
        memcpy(sixthargv, myargv, (STORAGE * MAXITEM));
        sixthwordcounter = wordcount;
    }
    if(k == 6 ){
        memcpy(seventhargv, myargv, (STORAGE * MAXITEM));
        seventhwordcounter = wordcount;
    }
    if(k == 7 ){
        memcpy(eighthargv, myargv, (STORAGE * MAXITEM));
        eightwordcounter = wordcount;
    }
    if(k == 8 ){
        memcpy(ninthargv, myargv, (STORAGE * MAXITEM));
        ninthwordcounter = wordcount;
    }
    commandCounterArray++;

}

int historyparse(char numberargv[STORAGE * MAXITEM], int wordc){
    int i = 0;
    int nullcounter = 0;
    int lettercounter = 0;


    while(nullcounter < wordc){
        if(numberargv[i] == '\0'){
            nullcounter++;
        }

        lettercounter++;
        i++;
    }
    fflush(stdin);
    ungetc('\n', stdin);
    while(lettercounter >= 0){
        if(numberargv[lettercounter] == '\0'){
            ungetc(' ', stdin);
        }
        else{
            ungetc(numberargv[lettercounter], stdin);

        }

        lettercounter--;
    }

    return parse();

}


