/*
* Cesar Lopez
* Professor Carroll
* CS570
* 11/29/2019
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
// Holds all past history, along with the history word counter and its last word
struct history{
    char pastargv[STORAGE * MAXITEM];
    int wordcounter;
    char lastw[MAXITEM];
};

struct history historyarg[9]; //initilize nine past history
int c; // Used in parse to hold the return value of getword.c which is the number of characters in the word
int oldsize = -1; // keep track of previous number of words in argv
char myargv[STORAGE * MAXITEM]; // Stores user current input
char *newargv[MAXITEM]; // Pointer to beginning of each word.
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
int commandCounter = 1; // Keeps track of the commands entered
int commandcountertemp = 0; // Used to make sure gives correct past history when needed
int appendFlag = 0; // determines if there is a file that needs to be appended
int bangCounter = 0;
int appenddiagnosticFlag = 0; // determines if there is a file that needs to be appended >>&
int argvFlag = 0; // determines if a file is in sent through argv
int poundFlag = 0; // checks if there is a comment in command
int childsection = 0;
int pipeflag = 0;
int fullwordcounter = 0;

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
int main(int argc, char *argv[] ){
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
        commandcountertemp = ((commandCounter-1)%9);
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

        fflush(stdin);
        saveHistory(commandcountertemp, fullwordcounter);
        commandCounter++;


        if(pipeflag != 0){
            if( strcmp(newargv[childsection],"&")  == 0){
                pipeflag = 0;
                continue;
            }
        }
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
        else if(strcmp(newargv[0],"cd") == 0  && (wordCount == 2 || poundFlag != 0)){

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

        if(pipeflag == 1){
            pipecode(inFile,outFile);
            pipeflag = 0;
            continue;
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
        poundFlag = 0;
    }
    if(argvFlag == 0){
        printf("p2 terminated.\n");
    }
    killpg(getpgrp(), SIGTERM);
    exit(0);
}

/*
*  Calls getword and able to return size of word.
*  especial cases if it runs to done, eof, !! and metacharacters
*  used to put user input in myarvg and newargv
*/
int parse(){
    int size = 0;
    int argvpointerPosition = 0; //moves the pointer from starting positon of array myargv
    int newargvpointerPosition = 0; // pointer position of array of pointer
    int simpleboolean = 0; // helps point to correct word
    int x = 0;
    poundFlag = 0;
    outputPointer = NULL;
    inputPointer = NULL;
    greaterThanFlag = 0;
    lessThanFlag = 0;
    pipeflag = 0;
    fullwordcounter = 0;

    // While loop will keep getting word until there is none left, returns 0
    while(( c = getword(myargv + argvpointerPosition) ) != 0 ){
        //if bang bang found then we want to use old newargv so just continue
        if(bangFlag != 0 || poundFlag != 0 ){
            continue;
        }
        if( c == 1 && argvFlag != 0 && myargv[argvpointerPosition] == '#' ){
            poundFlag++;
            continue;
        }
        if(c == -1 && size == 0) { // Done returns -1 and check if that is the first word
            doneFlag = 1;
            return size;
        }
        else if(c == -1 && strcmp(&myargv[argvpointerPosition], "done" ) == 0){
            // Condition if done is in the middle of sentence continue
            c = 4;
        }
        else if(c == 2 && (strcmp( &myargv[argvpointerPosition], "!$" ) == 0 )){
            c = strlen(historyarg[commandcountertemp-1].lastw);
            for(x = 0; x < c; x++){
                myargv[argvpointerPosition +x] = historyarg[commandcountertemp-1].lastw[x];
            }
        }
        else if(size == 0 && (strcmp( &myargv[argvpointerPosition], "!!" ) == 0 )){
            // adds null at the end and saves !! first before retrieving previous history
            myargv[argvpointerPosition + c] = '\0';
            saveHistory(commandcountertemp, 1 );
            return historyparse(commandcountertemp-1);
        }
        else if(size == 0 && (myargv[argvpointerPosition] == '!')
                && ( myargv[argvpointerPosition +1] == '0' ||
                     myargv[argvpointerPosition +1] == '1' ||
                     myargv[argvpointerPosition +1] == '2' ||
                     myargv[argvpointerPosition +1] == '3' ||
                     myargv[argvpointerPosition +1] == '4' ||
                     myargv[argvpointerPosition +1] == '5' ||
                     myargv[argvpointerPosition +1] == '6' ||
                     myargv[argvpointerPosition +1] == '7' ||
                     myargv[argvpointerPosition +1] == '8' ) ){
            //check which number used and send in that number. It is given in ascii number so subtract -49 to convert
            if( (((int) myargv[argvpointerPosition + 1]) - 49) < commandcountertemp) {
                myargv[argvpointerPosition + c] = '\0';
                saveHistory(commandcountertemp, 1);
                return historyparse(((int) myargv[argvpointerPosition + 1]) - 49);
            }
            else{ // if not valid history
                perror("History command not valid.\n");
                break;
            }

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
        else if(c == 1 && myargv[argvpointerPosition] == '|' && strcmp(&myargv[0], "echo") != 0){
            newargv[newargvpointerPosition++] = NULL;
            childsection = newargvpointerPosition;
            pipeflag++;
        }
        else if(simpleboolean < 1 ){
            // Will skip this round if above statement is true
            // Points word of myargv into newargv. Adds end of string in myargv of position plus c.
            // myargv will put next word after null terminal
            // arvpointer is c plus 1
            // Increment size of words counted
            newargv[newargvpointerPosition++] = myargv + argvpointerPosition;
            size++;
        }
        myargv[argvpointerPosition + c] = '\0';
        argvpointerPosition += c + 1;
        if(simpleboolean > 0){
            simpleboolean--;
        }
        fullwordcounter++;
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

// sends which line we are at along with current word counter. copies myargv to current array based off number
// of command number which is the first two lines. Rest goes up to beginning of last word in order start saving
// the last word into lastw
void saveHistory(int commandcounter, int wordcount){
    int index = 0;
    int nullcounter = 0;
    int i =0;
    memcpy(historyarg[commandcounter].pastargv, myargv, (STORAGE * MAXITEM));
    historyarg[commandcounter].wordcounter = wordcount;
    while(nullcounter != (wordcount-1) ){
        if(historyarg[commandcounter].pastargv[index] == '\0'){
            nullcounter++;
        }
        index++;
    }
    while(historyarg[commandcounter].pastargv[index] != '\0'){
        historyarg[commandcounter].lastw[i] = historyarg[commandcounter].pastargv[index];
        index++;
        i++;
    }
    historyarg[commandcounter].lastw[i] = '\0';
}

//Restores whatever index is given to function based of the past 10 history saved.
// need to ungetc in order to put back in stdin but needs to do it backwards in order to be read correctly
int historyparse(int numberargv){
    int nullcounter = 0;
    int lettercounter = 0;
    while(nullcounter < historyarg[numberargv].wordcounter){
        if(historyarg[numberargv].pastargv[lettercounter] == '\0'){
            nullcounter++;
        }
        lettercounter++;
    }
    fflush(stdin);
    ungetc(' ', stdin);
    while(lettercounter >= 0){
        if(historyarg[numberargv].pastargv[lettercounter] == '\0'){
            ungetc(' ', stdin);
        }
        else{
            ungetc(historyarg[numberargv].pastargv[lettercounter], stdin);
        }
        lettercounter--;
    }
    return parse();
}

void pipecode(int inFile, int outFile){
    int lchild;
    int grandchild;
    int filedes[2];
    int p_id = 0;

    fflush(stdout);
    fflush(stderr);
    lchild = fork();
    if(lchild == 0){
        pipe(filedes); // this creates pointers to write and read
        fflush(stdout);
        fflush(stderr);
        grandchild = fork(); // fork to be in grandchild
        if(grandchild == 0){
            //// check '<'
            if(lessThanFlag == 1){
                dup2(inFile, STDIN_FILENO);
                close(inFile);
            }
            dup2(filedes[1], STDOUT_FILENO);
            close(filedes[0]);
            close(filedes[1]);
            if((execvp(*newargv, newargv)) < 0){ // this executes the command
                fprintf(stderr, "%s Command not found. \n",newargv[0]);
                exit(9);
            }
        }

        //// check '>'
        if(greaterThanFlag == 1 && diagnosticFlag == 0 && appenddiagnosticFlag == 0){
            dup2(outFile,STDOUT_FILENO);
            close(outFile);
        }
        dup2(filedes[0], STDIN_FILENO);
        close(filedes[0]);
        close(filedes[1]);
        if((execvp(newargv[childsection], newargv+ childsection)) < 0){ // this executes the command
            fprintf(stderr, "%s Command not found. \n",newargv[0]);
            exit(9);
        }
    }

    for (;;) {
        p_id = wait(NULL);
        if (p_id == lchild) { //when the equal that means child has finished
            break;
        }
    }
}

void myhandler(){
}
