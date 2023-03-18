#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void builtcmd(char str[], char * envp[]);
int parsestr(char str[], char cmd[]);
void listfiles( const char* pathname);
void external_cmd(char str[]);
int pipe_func(char str[],char *envp[]);
void changecd(char str[]);
int redirect(char str[],char *envp[]);