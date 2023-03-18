
Project Desciption: 

This shell prgroam is an implemention of a linux shell, it behaves similarly to what a real shell would do. Myshell  has two modes, interactive and batch mode. 
    In batch mode, the batch file is entered with the myshell executable on the command line as shown: 
"./myshell batch.txt". In this mode, the program reads the contents of the batch file that is filled 
 with commands that the shell will execute. In interactive mode, only the myshell executable is entered 
 on the command line as shown: "./myshell". This mode will act like the real linux shell as it will ask 
 for user inputs and commands to execute, after executing input, it willgo back to the shell to ask for 
 user input again utill "quit" is entered.
    There are many commands that can be entered for the shell to execute. One of them is I/O redirection , by entering "< [file]", it is asking to change the file descriptor of stdin to the [file]. "> [file]" and ">> [file]" are asking to change file descriptor of stdout to file (">>" to append, ">" to overwrite). An example
 of redirection: "cat  file1 > file2", this tells the shell to execute cat on file1 and print the 
result to stdout (which is now file2). Another command is pipes, this command "|" allows one process to communicate with the other. An example of pipe: "cat file1 | wc", one process execute cat file
and another process (wc) on the other side of "|" will return number of lines, word count, byte and characters count from the cat process. The "&" command can be used to support background processing of programs. An example of background processing: "program1 & program2 &", the programs infront of "&" will execute in the background and launch after it returns to the shell. 

Buildin commands are coded commands which is different compared to the external commands. The manuel for buildin commands: 

a.  cd <directory> - Change the current default directory to  <directory>. If the <directory> argument is not present, report the current directory. 
 If the directory does not exist an appropriate error should be reported. This command should also change the PWD environment variable.  
b.  clr - Clear the screen. 
c.  dir <directory> - List the contents of directory <directory>. 
d.  environ - List all the environment strings. 
e.  echo <comment> - Display  <comment> on the display followed by a 
new line (multiple spaces/tabs may be reduced to a single space). 
f.  help - Display the user manual using the more filter.  
g.  pause - Pause operation of the shell until 'Enter' is pressed. 
h.  quit - Quit the shell. 
External commands use fork and execv to execute commands and their flags. These commands are ls, cat, grep, sleep, etc......
