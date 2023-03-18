/*
Harris Kwong
CIS 3207 Lab section 04
Project 2
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>

//function for cd command
void changecd(char buff[]){
    
    
    char *piece = strtok(strstr(buff,"cd")," ");
 
    char s[1000];
    
    if( (piece = strtok(NULL," ")) == NULL ){ //point to next string, and if no arugment entered it will print current directory 
        printf("Current directory: %s\n", getcwd(s, 1000));
    }
    else if(chdir(piece) == 0){ 
        printf("Directory changed to %s\n",getcwd(s,1000));
    }
    else{  //Wrong arugment 
        printf("Error. Directory path is invalid.\n");
    }
    setenv("PWD",getcwd(s,1000),1); //change path enivronment variable
}

//function to list directories and subdirectories
void listfiles( const char* pathname) {

 
  DIR* directory = opendir(pathname);
      if(directory == NULL){
            printf("Error. Cannot open directory.\n");
            closedir(directory);
            return;
        }

    printf("Reading files in: %s\n", pathname);

    struct dirent* entry;
    entry = readdir(directory);

    while (entry != NULL) {
        

    if(entry->d_type == DT_DIR){
        printf("DIR: %s/%s\n",pathname, entry->d_name);
    }
    else{
        printf("->FILE: %s\n", entry->d_name);    
     }

    //if the current item is a directory, it recursivly calls the listfiles function until entry = NULL.
    //strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 are to prevent looping and causing an error
    if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[1000] = { 0 };
            strcat(path, pathname);
            strcat(path, "/");
            strcat(path, entry->d_name);
            listfiles(path);
        }
        entry = readdir(directory);
    }

    closedir(directory);
    
}
//use strtok to look through strings in line to use in the function of built in commands
int parsestr(char str[], char cmd[]){
    
    char *piece = strtok(strstr(str,cmd)," "); //point to command
   
            piece = strtok(NULL, " ");    //point to arugment 

            if(strcmp(cmd,"dir") == 0){ //dir command
                if(piece == NULL || strcmp(piece,">") ==0 || strcmp(piece,">>") ==0 ||  strcmp(piece,"<") ==0 || strcmp(piece,"|") ==0){     //print current directory
                    listfiles(".");
                }
                else{        // print directory as entered
                    listfiles(piece);
                }
            }
            else if(strcmp(cmd,"echo") == 0){ //echo command
                while(piece != NULL){
                    if(strcmp(piece,">") ==0 || strcmp(piece,">>") ==0 ||  strcmp(piece,"<") ==0 || strcmp(piece,"|") ==0){ 
                        return 1;
                    }
                    printf("%s ",piece);
                    piece = strtok(NULL," ");
                }
                printf("\n\n");
            }
 
    return 0;
}

//function to execute built in command
void builtcmd(char str[], char * envp[]){
  
    //Exit program
    if(strstr(str,"quit")){
        printf("Exiting.\n");
        //fclose(fp);
        exit(1);
    }
    else if(strstr(str,"help")){ //Prints help manuel by using more filter

        int status_code;
        char *arg[3];
        arg[0] = "more";
        arg[1] = "readme_doc.txt";
        arg[2] = NULL; 
        if(fork() ==0){
            status_code = execvp(arg[0],arg);  
            if(status_code == -1){
                printf("Error in execvp.\n");
                return;
            }     
        }
        else{
            wait(NULL);
        }
    }
    else if(strstr(str,"clr")){     //Clear the screen by printing 50 new lines
        for(int i =0; i < 50;i++){
            printf("\n");
        }
    }
    else if(strstr(str,"pause")){ //Pause program untill enter is pressed
        printf("Press enter to continue. ");
        getchar();
    }
    else if(strstr(str,"dir")){ //List contents of directory
        parsestr(str,"dir");
    }
    else if(strstr(str,"cd")){ //Change directory or show current directory
        changecd(str);
    }
    else if(strstr(str,"echo")){ //Repeat user comment 
        parsestr(str,"echo");
    }
    else if(strstr(str,"environ")){ //Prints all the environment strings
        int i;
        for (i = 0; envp[i] != NULL; i++){
            printf("\n%s", envp[i]);
        }
        printf("\n");
    }


}



//Function to execute external command by using fork and execvp        
void external_cmd(char str[]){
    char *piece = strtok(str," "); //pointed to command
    char excommand[10];
    char test1[10] = "/bin/";
    char test2[15] = "/usr/bin/";
    char* argument_list[3];
    strcpy(excommand,piece); //copy command into excommand
    strcat(test1,piece);
    strcat(test2,piece);

    if(strcmp(piece,"dir") ==0 || strcmp(piece,"echo")== 0 || strcmp(piece,"more")){
        return;
    }
    if(access(test1,X_OK) == -1 || access(test2,X_OK) == -1 ){ //check if command entered is a external command
        return;
    }


    if(fork() == 0){ //child process

        piece = strtok(NULL," ");
 
       
            argument_list[0] = excommand;
            argument_list[1] = piece;
            argument_list[2] = NULL;

        
        int status_code = execvp(argument_list[0], argument_list); //check for error
        if(status_code == -1){
            printf("Error in execvp.\n");
            return;
        }
    }
    else{
            wait(NULL); // wait for the child proccess to finish
    }
}        

//Function for pipe when "|" is entered 
int pipe_func(char str[],char *envp[]){
 
    char test1[10] = "/bin/";
    char test2[15] = "/usr/bin/";
    int num =0;
    

    if(strstr(str,"|") == NULL){
        return 1;
    }
    char *buffer[3];
    char *buffer2[3];
    int count = 0;
    

    char *piece = strtok(str," ");  //point to command

    if(strcmp(piece,"dir") ==0 || strcmp(piece,"echo") ==0){
        num = 1;
    }

    strcat(test1,piece);
    strcat(test2,piece);



        while(strcmp(piece,"|") !=0 ){ //find command and arugment 
            buffer[count] = piece; 
            piece = strtok(NULL," "); 
            count++;
            if(count == 2){
                break;
            }
        }
  
          char* prog1[count +1];  

        for(int i =0; i < count; i++){  
            prog1[i] = buffer[i];               
        }

        prog1[count] = NULL;
 
    if(access(test1,X_OK) == -1 || access(test2,X_OK) == -1 ){ //check if command entered is a external command
        num =1;  // set num to 1 to indicate a built in command
    }


    piece = strtok(NULL," "); //pointed to command after "|"

    count =0;
    while(piece != NULL){
        buffer2[count] = piece; 
        piece = strtok(NULL," "); 
        count++;
        if(count == 2){ 
            break;
        }
    }
    char *prog2[count+1];

    for(int i =0; i < count; i++){
        prog2[i] = buffer2[i];
    }
    prog2[count] = NULL;

   
    int p[2];

    if(pipe(p) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    if(fork() == 0){            //first child

        close(STDOUT_FILENO);  //closing stdout
        dup(p[1]);         //replacing stdout with pipe write 
        close(p[0]);       //closing pipe read
        close(p[1]);

        if(num == 1){ //external command
            builtcmd(str,envp);
        }
        else{
            if(execvp(prog1[0], prog1) == -1){
                printf("execvp failed.\n");
                return 1;
            }            
        }
    }

    if(fork() == 0){            //2nd child
    
        close(STDIN_FILENO);   //closing stdin
        dup(p[0]);         //replacing stdin with pipe read
        close(p[1]);       //closing pipe write
        close(p[0]);

        if(execvp(prog2[0], prog2) == -1){
            printf("execvp failed.\n");
            return 1;
        }
    }

    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);

    return 0;
}

//redirect stdin or stdout
int redirect(char str[],char *envp[]){
    int file;
    int num =0;
    char *piece;
    int file2;
    char *ret;     

    if(strstr(str,"<")){ //found < entered
            num = 1;
            piece = strtok(strstr(str,"<")," ");
            piece = strtok(NULL," ");  //Now it is pointed to the input file 
               
            if(piece == NULL){  //if the next string after "<" is null 
                printf("Please enter input file after \"<\".\n");
                return 1;
            }

            file = open(piece, O_RDONLY | O_CREAT); //open input file for read only and create if it does not exist

            if(file == -1){
                printf("Error, unable to open %s.\n",piece);
                close(file);
                return 1;
            }
            dup2(file,STDIN_FILENO); //Replace stdin file descripter 0 with input file
            close(file);  
    }
    if(strstr(str,">")){       //found >> or > entered

        num =1;
        ret = strtok(strstr(str,">")," ");
        ret = strtok(NULL," ");

        if(ret == NULL){  //if the next string after ">" or ">>" is null 
            printf("Please enter outfile file after \">\" or \">>\".\n");
            return 1;
        }
               
        file2 = open(ret, O_WRONLY | O_CREAT | O_TRUNC); //open output file for write only and create if it does not exist and append if >> is entered

        if(strstr(str,">>")){
            file2 = open(ret, O_WRONLY | O_CREAT | O_APPEND); //open output file for write only and create if it does not exist and overwrite content if > is entered 
        }


        if(file2 == -1){
            printf("Error, unable to open %s.\n",ret);
            close(file2);
            return 1;
        }
            dup2(file2,STDOUT_FILENO); //Replace stdout file descripter 1 with outfile file
            close(file2); 

    } 
    if(num == 0){
        return 1;
    }
        
    external_cmd(str);
    builtcmd(str,envp); 

    return 0;       
}