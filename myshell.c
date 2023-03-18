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
#include "utility.h"


int main(int argc, char *argv[], char *envp[])
{
  /*********************************************************************************\
   * 
   * (Interactive mode or batch mode)
   * Prompt for input unless exit is entered 
   * 
   * 
   * *******************************************************************************/ 
    
  //Batch mode  
  if(argc > 1){

      int status; 
        int num =0;
        int *pid;
    FILE *fp;
    char batchbuff[1000];

    fp = fopen(argv[1],"r");

    if(fp == NULL){
        printf("Error in opening batch file.\n");
        fclose(fp);
        exit(1);
    }

    while(fgets(batchbuff,1000,fp)){

       if(strstr(batchbuff,"&")){   //if & is entered for background prcessing  
                int length = strlen(batchbuff);
                for(int i =0; i < length; i++){ //find number of & in string and create that number of child processes 
                    if(batchbuff[i] == '&'){
                        num++;
                    }
                }
                char buffer[num][50];
                for(int i = 0; i < num;i++){ //putting a temporary varibale in each buffer
                    strcpy(buffer[i]," ");
                }

                char *piece = strtok(batchbuff," "); 
             
                int i = 0;
                while(piece != NULL){
                    if(strcmp(piece,"&") == 0){    //if pointed to &, skip over
                        i++;        //move to next buffer
                        piece = strtok(NULL," ");      
                        if(piece == NULL){
                            break;
                        }
                    }

                    strcat(buffer[i],piece); 
                    strcat(buffer[i]," "); 
                    piece = strtok(NULL," ");
                }   
            
                pid = (int *)malloc(num * sizeof(int));
                if(pid == NULL){
                    printf("Error in allocating memory.\n");
                    break;
                }
        
                for(int i =0 ; i < num ; i++){
                    if((pid[i] = fork()) < 0){
                        printf("Error in forking.\n");
                        return 1;
                    }
                    else if(pid[i] == 0){
                        if(strstr(buffer[i],"|")){   //pipe case
                            pipe_func(batchbuff,envp);
                        }
                        builtcmd(buffer[i],envp);
                        external_cmd(buffer[i]);       
                    }
                    else{
                        waitpid(-1,&status,WNOHANG);
                    }  
                }
    
            }         
            else if(strstr(batchbuff,"<") || strstr(batchbuff,">")){         //redirection case
                int stdin_save = dup(STDIN_FILENO);      //restore stdin and stdout
                int stdout_save = dup(STDOUT_FILENO);          
                redirect(batchbuff,envp);
                fflush(stdin);
                fflush(stdout);
                dup2(stdin_save,STDIN_FILENO);
                dup2(stdout_save,STDOUT_FILENO);
                close(stdin_save);
                close(stdout_save);            
            }
            else if(strstr(batchbuff,"|")){   //pipe case
                pipe_func(batchbuff,envp);
            }
            else{
                builtcmd(batchbuff,envp);
                external_cmd(batchbuff);      
            }
    }

    fclose(fp);
  }
  else{ //Interactive mode
        int status; 
        int num =0;
        int *pid;
        char path[1000];
        int count =1;
        size_t bufsize = 50;
        while(count){
            char *line = (char *)malloc(bufsize * sizeof(char));
            if(line == NULL){
                printf("Error in allocating memory.\n");
                exit(1);
            }

            getcwd(path,100);
            strcat(path,"/MyShell> ");

            //Prompts for user input
            printf("%s",path); 
            getline(&line,&bufsize,stdin);
            line[strcspn(line, "\n")] = 0;    //remove newline
        
            if(strstr(line,"&")){   //if & is entered for background prcessing  
                int length = strlen(line);
                for(int i =0; i < length; i++){ //find number of & in string and create that number of child processes 
                    if(line[i] == '&'){
                        num++;
                    }
                }
                char buffer[num][50];
                for(int i = 0; i < num;i++){ //putting a temporary varibale in each buffer
                    strcpy(buffer[i]," ");
                }

                char *piece = strtok(line," "); 
             
                int i = 0;
                while(piece != NULL){
                    if(strcmp(piece,"&") == 0){    //if pointed to &, skip over
                        i++;        //move to next buffer
                        piece = strtok(NULL," ");      
                        if(piece == NULL){
                            break;
                        }
                    }

                    strcat(buffer[i],piece); 
                    strcat(buffer[i]," "); 
                    piece = strtok(NULL," ");
                }   
            
                pid = (int *)malloc(num * sizeof(int));
                if(pid == NULL){
                    printf("Error in allocating memory.\n");
                    break;
                }
        
                for(int i =0 ; i < num ; i++){
                    if((pid[i] = fork()) < 0){
                        printf("Error in forking.\n");
                        return 1;
                    }
                    else if(pid[i] == 0){
                        if(strstr(buffer[i],"|")){   //pipe case
                            pipe_func(line,envp);
                        }
                        builtcmd(buffer[i],envp);
                        external_cmd(buffer[i]);       
                    }
                    else{
                        waitpid(-1,&status,WNOHANG);
                    }  
                }
    
            }         
            else if(strstr(line,"<") || strstr(line,">")){         //redirection case
                int stdin_save = dup(STDIN_FILENO);      //restore stdin and stdout
                int stdout_save = dup(STDOUT_FILENO);          
                redirect(line,envp);
                fflush(stdin);
                fflush(stdout);
                dup2(stdin_save,STDIN_FILENO);
                dup2(stdout_save,STDOUT_FILENO);
                close(stdin_save);
                close(stdout_save);            
            }
            else if(strstr(line,"|")){   //pipe case
                pipe_func(line,envp);
            }
            else{
                builtcmd(line,envp);
                external_cmd(line);      
            }
            free(line);
        }
  }
    return 0;
}
