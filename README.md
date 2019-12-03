# csOSShell
Terminal logic project Class: Operating Systems

csOSShell directory:  
* Contains project zero and one. worked on the getword.c which was a function that allows  
for user input and was able to break down each word based on space, new line, and metacharacters. Returns number of 
characters of each word. Used loop to be able to return letterCount of each word until End of File or user types done.

p2 Folder: 
* Contains previous getword.c and worked on p2.c which is the main function. Program read input from user
or file passed in argv. Input will be handled by previous getword.c that is in the parse method.
* This program will read input from a user or handle a file passed in(argv). This command will be passed to the parse function below, which calls our getword.c.
* Able to handle change directory by user typing 'cd' along with path or folder. 
Handles errors if user inputs to many files. 
* Handles redirectories >, < 
* Background process when given '&' at the end of the command
* Allows for vertical piping, which ceats a child and within creates a grandchild to pipe the command to the other
* Past ten history. Able to use !! to get previus command or !#(!1,!2, !3..) to get a specfic previous command. Able to change to N amount of history since I used a struct and created an array of structs. The logic was able to easily grab N amount of history. but programmed to 10 for specification of project
* !$ is replaced with the last word used in the previous command

