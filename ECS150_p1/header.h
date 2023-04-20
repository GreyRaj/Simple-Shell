

#include <stdio.h>

#ifndef LINKLIST_H
#define LINKLIST_H

// globals
#define CMDLINE_MAX 512
#define pip_cmd_max 4        // max # piped cmds
#define num_cmd_strs 16      // max # strs per command
#define cmd_str_len 32       // max str length of cmd str
#define num_vars 40


// shell variable class
struct shell_var {
    char name[cmd_str_len];
    char val[cmd_str_len];
};
// shell variables container
struct shell_vars {
    struct shell_var** vars;
    int last_idx;
};
char* getval(struct shell_vars* vart, char* name);
int setvar(struct shell_vars* vart, char* setname, char* setval);
void disp_vars(struct shell_vars* vart);
void init_shell_vars(struct shell_vars* vart);



// linked list node
struct node {
    int idx;            // index of args list
    char** args;        // array of cmd strings
    struct node* next;  // ptr to next node
    int exit_status;    // exit status value
    int redir_er;       // WHETHER to redirect stderror to next node (process)
};
void disp_args(struct node* itr);
struct node* makenode();

// linked list
struct list {
    struct node* head;      // pointer to head node
    struct node* accessor;  // iterable pointer to access any node in list
    int num_procs;      // # of processes cmd asks for
};

void disp_list_args(struct list* mylist);
void resetlist(struct list* mylist);
void makelist(struct list* mylist);
void list_set(struct list* mylist, int row, int col, char* token);
void set_exit_status(struct list* mylist, int exit_stat_val);
void update_accessor(struct list* mylist);
void print_exit_statii(struct list* mylist);


// command runners
void run_pip_cmd(struct list mylist, char newout[CMDLINE_MAX], int chout, char cmd[CMDLINE_MAX], struct shell_vars* vart);
void run_standard_cmd(char* args[num_cmd_strs], char newout[CMDLINE_MAX], int chout, char cmd[CMDLINE_MAX], struct shell_vars* vart);

// command formatters
void format_standard_cmd(char cmd[CMDLINE_MAX], char* args[], int* chout, char newout[CMDLINE_MAX], int* args_endidx, int* skipfork, struct shell_vars* vart);
void format_piped_cmd(char cmd[CMDLINE_MAX], int* chout, char newout[CMDLINE_MAX], int* args_endidx, int* skipfork, struct list* pip, struct shell_vars* vart);

#endif
