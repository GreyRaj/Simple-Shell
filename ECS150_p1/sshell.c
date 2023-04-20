#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <termios.h>

#include "header.h"

// SSHELL Driver
int main() {
    // SINGLE FORK command vars
    char cmd[CMDLINE_MAX];          // entire cmd string
    char cmd_updated[CMDLINE_MAX] = "";     // first keyword of cmd (i.e. ls)
    char* args[num_cmd_strs];                     // = { cmd, "args[1]", NULL};
    int skipfork = 0;   // WHETHER we skip the forking process (happens IF detected cmd parsing error)
    
    // REDIRECT OUTPUT cmd
    int redir = 0;      // WHETHER we redirect output
    char newout[CMDLINE_MAX];   // new location to send output
    int args_endidx = 0;        // last index of args array
    
    // PIPED CMDS
    int pipout = 0;     // WHETHER we pipe output (0=no, 1=yes, 2=pipe STDOUT AND STDERR)

    struct list pipd_args_list;     // linked list, where each node has args list
    makelist(&pipd_args_list);      // init. linked list
    
    // variables
    struct shell_vars variable_tbl;
    init_shell_vars(&variable_tbl);
    
    
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // BIG LOOP TO KEEP ASKING FOR USER INPUT
    while (1) {
        // BELOW IS PROFESSOR'S CODE TO GET USER INPUT
        char* nl;
        int retval = 0;
        retval = 0 + retval;

        /* Print prompt */
        printf("sshell@ucd$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);
    
        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';


        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed '%s' [%d]\n", cmd, 0);
            break;
        }
        
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // OUR CODE
        
        // RESET all the command formatting/classification vars
        memset(cmd_updated,'\0', CMDLINE_MAX);
        strcpy(cmd_updated, cmd);
        
        redir= 0;
        memset(newout, '\0', CMDLINE_MAX);
        args_endidx = 0;
        skipfork = 0;
        
        pipout = 0;
        resetlist(&pipd_args_list);
        
        

        // FORMAT COMMAND INTO {cmd, args, NULL}
        if (strstr(cmd, "|") != NULL) {     // if we pipe output, each node has: {cmd, args, NULL}
            pipout = 1;
            format_piped_cmd(cmd_updated, &redir, newout, &args_endidx, &skipfork, &pipd_args_list, &variable_tbl);
        }
        else format_standard_cmd(cmd_updated, args, &redir, newout, &args_endidx, &skipfork, &variable_tbl);
        

        if (skipfork == 1)      // SKIP cmd execution IF DETECTED ERROR IN PARSING STAGE
            continue;
        

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //--------------COMMAND EXECUTION FOR PIPED PROCESSES--------------
        if (pipout) {
            run_pip_cmd(pipd_args_list, newout, redir, cmd, &variable_tbl);
            continue;
        }  
    
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //--------------COMMAND EXECUTION FOR SINGLE PROCESS--------------
        else run_standard_cmd(args, newout, redir, cmd, &variable_tbl);
    }
    return EXIT_SUCCESS;
}

