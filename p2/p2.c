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

struct history
{
    char myargv[STORAGE * MAXITEM]; // Stores user current input
    char onePreviousargv[STORAGE * MAXITEM];

    char *outputPointer; // Pointer to beginning of file '>'
    char *inputPointer; // Pointer to beginning of file '<'
    int doneFlag; // If user types done at the beginning of input
    int greaterThanFlag; // How many redirects counter for >
    int lessThanFlag; // How many redirects counted for <
    int diagnosticFlag; // how many >& found
    int bangFlag; // Found !! at the beginning of input
    int child; // Used to save forked process PID
    int pid; // Used to save wait return value

    int file; // Save file if given in argument
    int commandCounter;
    int appendFlag;
    int bangCounter;
    int appenddiagnosticFlag;
    int argvFlag;
    int poundFlag;
    int wordCount;

};

struct history historyArray[10];


int c; // Used in parse to hold the return value of getword.c which is the number of characters in the word
int oldsize = -1; // keep track of previous number of words in argv

char myargv[STORAGE * MAXITEM]; // Stores user current input
char *newargv[MAXITEM]; // Pointer to beginning of each word.
int argvwordCount[10];
char onePreviousargv[STORAGE * MAXITEM];
char twoPreviousargv[STORAGE * MAXITEM];
char thirdPreviousargv[STORAGE * MAXITEM];
char fourthPreviousargv[STORAGE * MAXITEM];
char fifthPreviousargv[STORAGE * MAXITEM];
char sixthPreviousargv[STORAGE * MAXITEM];
char seventhPreviousargv[STORAGE * MAXITEM];
char eightPreviousargv[STORAGE * MAXITEM];
char ninthPreviousargv[STORAGE * MAXITEM];

char *onePreviousnewargv[MAXITEM];
char *twoPreviousnewargv[MAXITEM];
char *thirdPreviousnewargv[MAXITEM];
char *fourthPreviousnewargv[MAXITEM];
char *fifthPreviousnewargv[MAXITEM];
char *sixthPreviousnewargv[MAXITEM];
char *seventhPreviousnewargv[MAXITEM];
char *eightPreviousnewargv[MAXITEM];
char *ninthPreviousnewargv[MAXITEM];


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
int commandCounterArray = 1;

int appendFlag = 0;
int bangCounter = 0;
int appenddiagnosticFlag = 0;
int argvFlag = 0;
int poundFlag = 0;

char lastword;
char templastword[MAXITEM];
int lastwordSize = 0;


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
    int x = 0;
    for(x = 0; x < 10; x++){
        historyArray[x].doneFlag =0; // If user types done at the beginning of input
        historyArray[x].greaterThanFlag = 0; // How many redirects counter for >
        historyArray[x].lessThanFlag = 0; // How many redirects counted for <
        historyArray[x].diagnosticFlag = 0; // how many >& found
        historyArray[x].bangFlag = 0; // Found !! at the beginning of input
        historyArray[x].child = 0; // Used to save forked process PID
        historyArray[x].pid = 0 ; // Used to save wait return value

        historyArray[x].commandCounter = 1;
        historyArray[x].appendFlag = 0;
        historyArray[x].bangCounter = 0;
        historyArray[x].appenddiagnosticFlag = 0;
        historyArray[x].argvFlag = 0;
        historyArray[x].poundFlag = 0;
        historyArray[x].wordCount = 0;
    }

    int devnull; // Saves open dev/null
    int flags = 0; // Flags for open, create, read ,write
    int mode = 0; // Mode of open files
    int outFile = 0; // Saves file that is open
    int inFile = 0; // Saves file that is open
    int backgroundFlag = 0; // & counter
    int wordCount = 0; // Needed for number of words to be able to check conditions later
    int k = 0; // needed for bang bang while loop
    int hit =0; // // need to \0 counter for bang bang while loop
    (void) signal(SIGTERM, myhandler);
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
        commandCounterArray = commandCounterArray %10;
        doneFlag = 0;
        diagnosticFlag = 0;
        appenddiagnosticFlag = 0;
        appendFlag = 0;
        backgroundFlag = 0;
        if(argvFlag == 0){
            printf("%%%d%% ",commandCounter); // For user prompt
        }

        historyArray[commandCounterArray].wordCount = parse(); // Parse will add each word into myargv to have access to it
        argvwordCount[0] = wordCount;

        /*
         * Used to store previous user input as soon as it returns from parse.
         * since we place '\0' after each word, we use word count to find all
         * we do this since  normally it would stop the loop after it finds the first '\0'
         * saves every char in onepreviousargv
         * */


        memcpy( historyArray[commandCounterArray].onePreviousargv, myargv, (STORAGE * MAXITEM));


        memcpy( ninthPreviousargv, eightPreviousargv, (STORAGE * MAXITEM));
        memcpy( eightPreviousargv, seventhPreviousargv, (STORAGE * MAXITEM));
        memcpy( seventhPreviousargv, sixthPreviousargv, (STORAGE * MAXITEM));
        memcpy( sixthPreviousargv, fifthPreviousargv, (STORAGE * MAXITEM));
        memcpy( fifthPreviousargv, fourthPreviousargv, (STORAGE * MAXITEM));
        memcpy( fourthPreviousargv, thirdPreviousargv, (STORAGE * MAXITEM));
        memcpy( thirdPreviousargv, twoPreviousargv, (STORAGE * MAXITEM));
        memcpy( twoPreviousargv, onePreviousargv, (STORAGE * MAXITEM));
        memcpy( onePreviousargv, myargv, (STORAGE * MAXITEM));

        memcpy( ninthPreviousnewargv, eightPreviousnewargv, MAXITEM);
        memcpy( eightPreviousnewargv, seventhPreviousnewargv, MAXITEM);
        memcpy( seventhPreviousnewargv, sixthPreviousnewargv, MAXITEM);
        memcpy( fifthPreviousnewargv, fourthPreviousnewargv, MAXITEM);
        memcpy( fourthPreviousnewargv, thirdPreviousnewargv, MAXITEM);
        memcpy( thirdPreviousnewargv, twoPreviousnewargv, MAXITEM);
        memcpy( twoPreviousnewargv, onePreviousnewargv, MAXITEM);
        memcpy( onePreviousnewargv, newargv, MAXITEM);
        int countdown = 9;
        for(countdown; countdown > 0; countdown--){
            argvwordCount[countdown] = argvwordCount[countdown-1] ;
        }


        if (wordCount == -1 || doneFlag) { // Done is seen in myargv first position then quit program
            break;
        }

        if(wordCount == 0){ // Will re issue the user prompt to type again
            continue;
        }

        commandCounter++;
        commandCounterArray++;

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
            bangFlag++;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!1" ) == 0 )){
            bangFlag++;
            bangCounter = 0;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!2" ) == 0 )){
            bangFlag++;
            bangCounter = 2;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!3" ) == 0 )){
            bangFlag++;
            bangCounter = 3;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!4" ) == 0 )){
            bangFlag++;
            bangCounter = 4;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!5" ) == 0 )){
            bangFlag++;
            bangCounter = 5;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!6" ) == 0 )){
            bangFlag++;
            bangCounter = 6;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!7" ) == 0 )){
            bangFlag++;
            bangCounter = 7;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!8" ) == 0 )){
            bangFlag++;
            bangCounter = 8;
            continue;
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!9" ) == 0 )){
            bangFlag++;
            bangCounter = 9;
            continue;
        }
        else if((strcmp( &myargv[argvpointerPosition], "!$" ) == 0 )){//&myargv[argvpointerPosition-lastwordSize-1]

            int n = 0;
            //for(n = 0; n < lastwordSize; n++){
            myargv[argvpointerPosition] = "@!";
            printf("*%s\n",&myargv[argvpointerPosition]);
           // }

            c = lastwordSize;
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

        if(myargv[argvpointerPosition] != '\0' ){
            int n = 0;
            for(n= 0; n < c; n++){
                templastword[n] = myargv[argvpointerPosition+ n];
            }
           // memcpy (templastword, myargv[argvpointerPosition], sizeof(myargv[argvpointerPosition]));
            printf("*%s\n",templastword);
            lastwordSize = c;
        }

        myargv[argvpointerPosition + c] = '\0';
        argvpointerPosition += c + 1;
        if(simpleboolean > 0){
            simpleboolean--;
        }


    }

    /*
     * After is continues any left over words once !! is found then we need to put previous command in myarg
     * newargv hasnt changed since we continued above and return oldsize
     * */
    if(bangFlag != 0){
        bangFlag = 0;

        if(bangCounter == 2){
            memcpy(myargv, twoPreviousargv, (STORAGE * MAXITEM));
            printf("*&&*\n");
            dup2(*myargv, STDIN_FILENO);
            int k = 0;
             k =parse();
             printf("*&*\n");
            return k;
            memcpy(newargv, twoPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[2];

        }
        else if(bangCounter == 3){
            memcpy(myargv, thirdPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, thirdPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[3];
        }
        else if(bangCounter == 4){
            memcpy(myargv, fourthPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, fourthPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[4];
        }
        else if(bangCounter == 5){
            memcpy(myargv, fifthPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, fifthPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[5];
        }
        else if(bangCounter == 6){
            memcpy(myargv, sixthPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, sixthPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[6];
        }
        else if(bangCounter == 7){
            memcpy(myargv, seventhPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, seventhPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[7];
        }
        else if(bangCounter == 8){
            memcpy(myargv, eightPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, eightPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[8];
        }
        else if(bangCounter == 9){
            memcpy(myargv, ninthPreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, ninthPreviousnewargv, MAXITEM);
            oldsize = argvwordCount[9];
        }

        else{
            memcpy(myargv, onePreviousargv, (STORAGE * MAXITEM));
            memcpy(newargv, onePreviousnewargv, MAXITEM);
        }
        bangCounter = 0;
        return oldsize;
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
