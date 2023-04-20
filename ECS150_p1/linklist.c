

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>


#include "header.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// linked list node

// display all values in node's args list (for debugging)
void disp_args(struct node* itr) {
    fprintf(stderr, "pipe #%d args:\n", itr->idx);
    for (int i = 0; i < num_cmd_strs; i++) {
        fprintf(stderr, "%s ", itr->args[i]);
    }
    fprintf(stderr, "\n");
}

// create new node
struct node* makenode() {
    // create the node
    struct node* newg = (struct node*) malloc(sizeof(struct node));
    
    // allocate space for entire args list
    newg->args = (char**) malloc(num_cmd_strs * sizeof(char*));
    for (int i = 0; i < num_cmd_strs; i++) {
        // allocate space for each args list row
        newg->args[i] = (char*) malloc(cmd_str_len * sizeof(char));
        memset(newg->args[i], '\0', cmd_str_len);       // fill in args list row with NULL char
    }
    newg->idx = 0; newg->redir_er = 0;
    return newg;
}

//~~~~~~~~~~~~~~~~~~~
// linked list class

// display the "args" lists of all nodes in list (for debugging)
void disp_list_args(struct list* mylist) {
    fprintf(stderr, "%d\n", mylist->num_procs);
    struct node* itr = mylist->head;
    while (itr != NULL) {
        disp_args(itr);
        itr = itr->next;
    }
}

// reset the linked list data
void resetlist(struct list* mylist) {
    mylist->accessor = mylist->head;        // set accessor to head
    mylist->num_procs = 1;          // # of processes at min. = 1
    
    // iterate thru nodes, and reset each
    struct node* itr = mylist->head;
    while (itr != NULL) {
        for (int i = 0; i < num_cmd_strs; i++) {
            itr->args[i] = NULL;        // reset the args list strs
            itr->redir_er = 0;
        }
        itr->exit_status = 0;   // reset node exit status value
        itr = itr->next;
    }
}

// initialize the list
void makelist(struct list* mylist) {
    mylist->head = makenode();
    struct node* itr = mylist->head;
    
    // iteratively append new nodes to end of list
    for (int i = 0; i < pip_cmd_max; i++) {
        struct node* newg = makenode();
        newg->idx = i+1;        // store index of node into node's "idx" var
        itr->next = newg;
        itr = itr->next;
    }
    mylist->accessor = mylist->head;
}

// sets: mylist->node[row]->args[col] = token (str)
void list_set(struct list* mylist, int row, int col, char* token) {
    // increment node* accessor to correct postion (row)
    while (mylist->accessor != NULL && mylist->accessor->idx != row) {
        mylist->accessor = mylist->accessor->next;
    
    } // handle incorrect [row] index case
    if (mylist->accessor == NULL || row >= pip_cmd_max) {
        fprintf(stderr, "PARSING PROBLEM\n");
    }
    mylist->accessor->args[col] = token;   // set the value in args
}

// // check if command is "set ___" ~~~ if so, restructure command format (unused)
// void check_for_varset(struct list* mylist) {
//     struct node* itr = (mylist)->head;
//     char varset_value[cmd_str_len];
    
//     while (itr != NULL && itr->args[0] != NULL) {
//         if (strcmp(itr->args[0], "set") == 0) {
//             itr->args[0] = "export";
            
//             strcpy(varset_value, itr->args[2]);
//             itr->args[2] = "=";
//             itr->args[3] = varset_value;
//         }
//         itr = itr->next;
//     }
// }

// set exit status for the current list accessor node
void set_exit_status(struct list* mylist, int exit_stat_val) {
    mylist->accessor->exit_status = WEXITSTATUS(exit_stat_val);
}
// increment list accessor to next node
void update_accessor(struct list* mylist) {
    mylist->accessor = mylist->accessor->next;
}

// print entire list's exit status's [][][][]
void print_exit_statii(struct list* mylist) {
    struct node* itr = mylist->head;
    for (int i = 0; i < mylist->num_procs; i++) {   // for each process exit status
        fprintf(stderr, "[%d]", itr->exit_status);
        itr = itr->next;
    }
    fprintf(stderr, "\n");
}

