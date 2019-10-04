/*
 * Cesar Lopez
 * Professor Carroll
 * CS570
 * 10/4/2019
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
int oldsize = -1; // keep track of previous number of words in argv

char myargv[255 * MAXITEM]; // Stores user current input
char *newargv[MAXITEM]; // Pointer to beginning of each word.
char onePreviousargv[255*MAXITEM];
char *onePreviousnewargv[MAXITEM];

char *outputPointer; // Pointer to beginning of file '>'
char *inputPointer; // Pointer to beginning of file '<'
char *outputPointerAnd; // pointer to beggining of file of '>&'
int doneFlag =0;
int greaterThanFlag = 0;
int lessThanFlag = 0;
int diagnosticFlag = 0;
int greaterThanAndFlag = 0;
int bangFlag = 0;
int child = 0;
int pid = 0 ;

int file;

/*
 * first checks if there is anything in argv[1]. If there isn't anything then enters loop and calls parse.
 * checks for special conditions and will break when finds done is first word or EOF
 * */
int main(int argc, char *argv[] )
{

    int flags = 0;
    int mode = 0;
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
        wordCount = parse(); // Parse will add each word into myargv to have access to it

        /*
         * used to store previous user input as soon as it returns from parse.
         * since we place '\0' after each word, we use wrod count to find all
         * we do this since  normally it would stop the loop after it finds the first '\0'
         * saves every char in onepreviousargv*/
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

        if (wordCount == -1 || doneFlag) { // Done is seen in myargv first position then quit program
            break;
        }

        if(wordCount == 0){ // Will re issue the user prompt to type again
            continue;
        }

        //// check '>' , '<' error
        if(greaterThanFlag > 1 || lessThanFlag > 1){ // if to many directional arguments, error and oontinue
            perror("Too many redirectories.\n");
            continue;
        }


        //// check '>'
        if(greaterThanFlag == 1){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(outputPointer == NULL){ // if there is no file to point then error
                perror("No file name to create.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags,mode) ) < 0 ){ // checks if file doesn't exist
                perror("File already exists.\n");
                continue;
            }
        }

        //// check '<'
        if(lessThanFlag == 1){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(inputPointer == NULL){ // if there is no file to point then error
                perror("No file name to create.\n");
                continue;
            }
            if( (inFile = open(outputPointer,flags,mode) ) < 0 ){ // checks if file doesn't exist
                perror("File already exists.\n");
                continue;
            }
        }
        //// check '>&'
        if(greaterThanAndFlag == 1){
            flags = O_CREAT | O_EXCL | O_RDWR ;
            mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if(outputPointerAnd == NULL){ // if there is no file to point then error
                perror("No file name to create.\n");
                continue;
            }
            if( (outFile = open(outputPointer,flags,mode) ) < 0 ){ // checks if file doesn't exist
                perror("File already exists.\n");
                continue;
            }
        }

        /* Checks if last char in newargv is '&' to run process in the background.
         * in order to pass future cases must decrement wordcount and update backgroundFlag*/
        if( strcmp(newargv[wordCount-1],"&")  == 0){
            backgroundFlag++;
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
        }
        else if(strcmp(newargv[0],"cd") == 0  && wordCount == 2){
            if( chdir(newargv[1]) == -1){
                perror("No folder in current directory.\n");

            }
            else{
                chdir(newargv[1]);
            }
        }
        else if(strcmp(newargv[0],"cd") == 0  && wordCount > 2){
             perror("To many parameters for cd.\n");

        }
        else{
            //// fork code
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


                //// ">" code
                if(greaterThanFlag != 0 && diagnosticFlag == 0){
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
                    dup2(outFile,STDOUT_FILENO);
                    close(outFile);
                }

                if((execvp(*newargv, newargv)) < 0){ // this executes the command
                    perror("Command not found.\n");
                    exit(9);
                }
            }
            else if( child < 0){
                printf("Fork was unsucessful\n");
                exit(1);
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

    }

    kill(getpgrp(), SIGTERM);
    printf("p2 terminated. \n");
    exit(0);
}


/*
 *  calls getword and able to return size of word.
 *  especial cases if it runs to done, eof, !! and metacharacters
 */
int parse(){

    int size = 0;
    int argvpointerPosition = 0;
    int newargvpointerPosition = 0;

    greaterThanFlag = 0;
    lessThanFlag = 0;


    // While loop will keep getting word until there is none left, returns 0
    while(( c = getword(myargv + argvpointerPosition) ) != 0 ){

        //if bang bang found then we want to use old newargv so just continue
        if(bangFlag != 0){
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

        else if(c == -1){ // eof found

            break;
        }

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

        else if(c == 2 && strcmp(&myargv[argvpointerPosition], ">&") == 0 ){
            diagnosticFlag++;
            greaterThanFlag++;
            outputPointer = myargv + argvpointerPosition + 3;

        }
            // Will skip this round if above statement is true
            // Points word of myargv into newargv. Adds end of string in myargv of position plus c.
            // myargv will put next word after null terminal
            // arvpointer is c plus 1
            // Increment size of words counted
        else {
            newargv[newargvpointerPosition++] = myargv + argvpointerPosition;
            myargv[argvpointerPosition + c] = '\0';
            argvpointerPosition += c + 1;
            size++;
        }
    }

    /*
     * After is continues any left over words once !! is found then we need to put previous command in myarg
     * newargv hasnt changed since we continued above and return oldsize*/
    if(bangFlag != 0){
        bangFlag = 0;
        int k = 0;
        int hit =0;
        while(hit != oldsize){
            if(onePreviousargv[k] == '\0'){
                hit++;
                myargv[k] = '\0';
            }
            else{
                myargv[k] = onePreviousargv[k];
            }

            k++;
        }

        return oldsize;
    }
    newargv[newargvpointerPosition] = NULL;



    oldsize = size; // stores old size before returning
    return size;
}

void myhandler(){
}
