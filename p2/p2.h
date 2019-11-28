#include "getword.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>




#define MAXITEM 100 /* max number of words per line */

void pipecode(int inFile, int outFile);
void saveHistory(int commandcounter, int wordcount);
int historyparse(int numberargv);
void myhandler();
int parse();
 
