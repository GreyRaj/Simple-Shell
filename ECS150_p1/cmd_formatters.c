
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"



// VALIDATE STANDARD COMMAND, STORE INTO ARGS LIST
void format_standard_cmd(char cmd[CMDLINE_MAX], char* args[], int* redir, char newout[CMDLINE_MAX], int* args_endidx, int* skipfork, struct shell_vars* variable_tbl) {
                
    // VARIABLE resets        
    memset(newout, '\0', CMDLINE_MAX);     // newfile name (for redirected output)
    char* token = strtok(cmd, " ");      // word tokenizer
    char* prev_token;                   // prev. word
    
    // ITERATING THRU CMD
    int i = 0;
    while (token) {
        args[i] = NULL;
        
        //-------bad start of cmd-------
        
        // error: cmd started with > or |
        if ((strcmp(token, ">&") == 0  || strcmp(token, ">") == 0 )&& i == 0) {
            printf("Error: missing command\n");
            *skipfork = 1;
            break;
        }
        //-------output redirection-------
        
        // deal with modifying "newout" if user specifies ">"
        
        // redir = (0=no, 1=yes, 2=yes AND redirect STDERR)
        if (strcmp(token, ">") == 0 || strcmp(token, ">&") == 0) {
            *redir = 1;
            *args_endidx = i;
            if (strcmp(token, ">&") == 0)
                *redir = 2;
        }
        if (*redir) {
            if (i == *args_endidx + 1)  // save newfile name (if we saw "> ___ ")
                strcpy(newout, token);
            // error: if too many cmds after ">"
            else if ( i > *args_endidx + 1) {
                *skipfork = 1;
                printf("Error: mislocated output redirection\n");
                break;
            }
        }
        
        //-------too many args-------
        
        if (i >= num_cmd_strs) {
            *skipfork = 1;
            printf("Error: too many process arguments\n");
            break;
        }
        //-------output redirection-------

        // normally motify args array (if not redir)
        if (! (*redir)) {
            if (token[0] == '$')
                args[i] = getval(variable_tbl, token+1);
            else args[i] = token;
            
        }
        else args[i] = NULL;
        
        prev_token = token;
        token = strtok(NULL, " ");        // get next word
        i++;
    }
    args[i] = NULL;     // set last arg NULL


    //-------bad end of cmd-------

    // if cmd ended with >
    if (*redir && ( strcmp(">", prev_token)==0 || strcmp(">&", prev_token)==0)) {//i == *args_endidx + 1) {
        printf("Error: no output file\n");
        *skipfork = 1;
    }
}


// VALIDATE command, store cmd into args list
void format_piped_cmd(char cmd[CMDLINE_MAX], int* redir, char newout[CMDLINE_MAX], int* args_endidx, int* skipfork, struct list* pip_args, struct shell_vars* variable_tbl) {
                        
    memset(newout, '\0', CMDLINE_MAX);     // reset redirected output var
    char* token = strtok(cmd, " ");      // create word tokenizer
    char* prev_token;       // previous token tracker
    
    int row=0,col=0;        // command conversion format: node[row]->args[col]
    int saved_row_end = 0;  // index of last row before seeing ">"
    
    while (token) {
        // if token == | and col == 0
        if (col == 0 && (token[0] == '|' || token[0] == '>')) {
            *skipfork = 1;
            printf("Error: missing command\n");
            break;
        }
        // modify args list filling in ~~~ if token == |
        if (strstr(token, "|") != NULL) {
            list_set(pip_args, row, col, NULL);
            // redirect stderror
            pip_args->accessor->redir_er = 0;
            if (strstr(token, "&") != NULL)
                pip_args->accessor->redir_er = 1;
            row++; col = 0;
            pip_args->num_procs += 1;
            prev_token = token;
            token = strtok(NULL, " ");
            
            continue;
        }
        // modify redir ~~~~ if we find > redirect
        if ((strcmp(token, ">") == 0 || strcmp(token, ">&") == 0) && !(*redir)) {
            *redir = 1;
            *args_endidx = col; saved_row_end = row;
            list_set(pip_args, row, col, NULL);
            
            if (strcmp(token, ">&") == 0)
                *redir = 2;
        }
        // parsing error: too many args
        if (row >= pip_cmd_max || col >= num_cmd_strs) {
            *skipfork = 1;
            printf("Error: too many process arguments\n");
            break;
        }
        
        // fill in node args list
        if (!(*redir)) {
            if (token[0] == '$')
                list_set(pip_args, row, col, getval(variable_tbl, token+1));
            else list_set(pip_args, row, col, token);     // pip_args->args[row][col] = token
        }
        if (*redir && col == *args_endidx + 1 && row == saved_row_end){      // if we're on " > '____' "
            strcpy(newout, token);                                          // copy "___" into "newout" str
            list_set(pip_args, row, col, NULL);  // original output args set to NULL
        }
         // parse error: if extra strings follow "> ____"
        else if (*redir && (col != *args_endidx && col != *args_endidx+1)) {
            *skipfork = 1;
            printf("Error: mislocated output redirection\n");
            break;
        }
        // increment word tokenizer
        prev_token = token;
        token = strtok(NULL, " ");
        col++;
    }
    
    // printf("num procs: %d\n", pip_args->num_procs);
    list_set(pip_args, row, col, NULL);      // set last index NULL
    
    //-------bad end of cmd-------
    
    // if cmd ended with |
    if (strcmp("|", prev_token)==0) {
        printf("Error: missing command\n");
        *skipfork = 1;      // skip fork (parsing er)
    }

    // if cmd ended with >
    if (*redir && ( strcmp(">", prev_token)==0 || strcmp(">&", prev_token)==0)) {//i == *args_endidx + 1) {
        printf("Error: no output file\n");
        *skipfork = 1;      // skip fork (parsing er)
    }
}
