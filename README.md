# csOSShell
Terminal logic project (Class in Progress: Operating Systems)

csOSShell directory: 
*Contains project zero and one. worked on the getword.c which was a function that allows  
for user input and was able to break down each word based on space, new line, and metacharacters. Returns number 
letters of word. Used loop to be able to return letterCount of each word until End of File or user types done.

p2 directory: 
*Contains previous getword.c and worked on p2.c which is the main function. Program read input from user
or file passed in argv. Input will be handled by previous getword.c that is in the parse method.
*This program will read input from a user or handle a file passed in(argv). This command will be passed to the parse function below, which calls our getword.c. 
*Able to handle change diretory by user typing 'cd' along with path or folder. 
Handles errors if user inputs to many files. 
*Also, handled redirectories >, < and background processes.

