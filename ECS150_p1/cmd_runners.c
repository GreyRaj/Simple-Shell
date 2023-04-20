
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



// RUN pipe command
void run_pip_cmd(struct list pargs_list, char newout[CMDLINE_MAX], int redir, char cmd[CMDLINE_MAX], struct shell_vars* vars_tbl) {
    int fd[2];
    char cd_cmd[3] = "cd";
    int retval=0;
            
    int saved_fdout = dup(STDOUT_FILENO);
    int saved_fdin = dup(STDIN_FILENO);
    pargs_list.accessor = pargs_list.head;
    // check_for_varset(&pargs_list);
    // disp_list_args(&pargs_list);
    
    int numitr = pargs_list.num_procs;
    for (int p = 0; p < numitr; p++) {
        // CREATE current PIPE
        pipe(fd);
        int pid = fork();   // FORK()
        
        //-------CHILD-------
        if (pid == 0) {     // KID
            // RECONNECT OUTPUT TO CURRENT PIPE
            if (p < numitr- 1) {
                dup2(fd[1], STDOUT_FILENO);     // connect write to current pipe (IF NOT ON LAST PROC)
                if (pargs_list.accessor->redir_er == 1) // connect stderr to current pipe
                    dup2(fd[1], STDERR_FILENO);
            }
            else dup2(saved_fdout, STDOUT_FILENO);      // connect original stdout to current pipe (IF ON LAST PROC)
            close(fd[0]); close(fd[1]);     // close opened fds
            
            // CMD EXECUTION
            // SKIP 'cd' command
            if (strcmp(cd_cmd, pargs_list.accessor->args[0]) == 0) { exit(0); }
            
            // change output locn (to newout file)
            if (redir != 0 && p == numitr-1) {
                // create new file
                int fd_ = open(newout, O_WRONLY | O_CREAT, 0644);
                if (fd_ == -1) {
                    fprintf(stderr, "Error: cannot open output file\n");
                    exit(-1);
                }
                // send output to new file
                dup2(fd_, STDOUT_FILENO);
                
                // send stderr to this same new file
                if (redir == 2) dup2(fd_, STDERR_FILENO);
                close(fd_);
            }
            // execute the current command
            retval = execvp(pargs_list.accessor->args[0], pargs_list.accessor->args);
            retval += 0;
            exit(1);
        }
        //-------OVERHEAD PARENT-------
        
        int status;
        retval = waitpid(0, &status, 0);
        
        if (p < numitr-1)  dup2(fd[0], STDIN_FILENO);      // connect read to current pipe
        else dup2(saved_fdin, STDIN_FILENO);     // reconnect original stdin (IF ON LAST PROC)
        close(fd[0]); close(fd[1]);     // close opened fds
        
        // CMD EXECUTION
        // do cd command
        int up_status = 0;
        if (strcmp(cd_cmd, pargs_list.accessor->args[0]) == 0) {
            up_status = chdir(pargs_list.accessor->args[1]);     // up_status = updated status of (attempt to do cd)
            if (up_status != 0) {
                fprintf(stderr, "Error: cannot cd into directory\n");
            }
        }
        // handle variable command
        if (strcmp("set", pargs_list.accessor->args[0]) == 0) {
            up_status = setvar(vars_tbl, pargs_list.accessor->args[1], pargs_list.accessor->args[2]);
        }
        
        // wait for child
        if (up_status != 0) status = up_status;     // ignore child ret. value if command was "cd"
        pargs_list.accessor->exit_status = WEXITSTATUS(status);
        
        // completion message
        if (p == numitr - 1) {
            fprintf(stderr, "+ completed '%s' ", cmd);
            print_exit_statii(&pargs_list);
        }
        
        // FINAL UPDATES
        update_accessor(&pargs_list);       // increment args list accessor
    }
}  

// RUN standard command
void run_standard_cmd(char* args[num_cmd_strs], char newout[CMDLINE_MAX], int redir, char cmd[CMDLINE_MAX], struct shell_vars* vars_tbl) {
    
    char cd_cmd[3] = "cd";
    int retval=0; retval += 0;
    
    // // handle variable setting command
    // if (args[0] != NULL && args[0] == "set") {
    //     args[0] = "export";
    //     char varset_val[cmd_str_len];
    //     strcpy(varset_val, args[2]);
    //     args[2] = "=";
    //     args[3] = varset_val;
    // }
    
    //-------fork-------
    pid_t pid;
    pid = fork();
    

    //-------PARENT-------
    if (pid > 0) {
        // do cd command
        int up_status = 0;
        if (strcmp(cd_cmd, args[0]) == 0) {
            up_status = chdir(args[1]);     // up_status = updated status of (attempt to do cd)
            if (up_status != 0) {
                fprintf(stderr, "Error: cannot cd into directory\n");
            }
        }
        // handle variable command
        if (strcmp("set", args[0]) == 0) {
            up_status = setvar(vars_tbl, args[1], args[2]);
        }
        // wait for child
        int status = 0; status = 0 + status;
        waitpid(pid, &status, 0);
        if (up_status != 0) status = up_status;     // ignore child ret. value if command was "cd"
        fprintf(stderr, "+ completed '%s' [%d]\n", cmd, WEXITSTATUS(status));
    }
    //-------CHILD-------
    else if (pid == 0) {		// child
        // SKIP cd command
        if (strcmp(cd_cmd, args[0]) == 0) { exit(0); }
        
        // change output locn
        if (redir != 0) {
            int fd_ = open(newout, O_WRONLY | O_CREAT, 0644);
            if (fd_ == -1) {
                fprintf(stderr, "Error: cannot open output file\n");
                exit(-1);
            }
            dup2(fd_, STDOUT_FILENO);
            if (redir == 2) dup2(fd_, STDERR_FILENO);
            close(fd_);
        }
        retval = execvp(args[0], args);
        exit(1);
    }
}
