

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "header.h"


// return shell_vars[name]
char* getval(struct shell_vars* vart, char* name) {
    for (int i = 0; i < vart->last_idx; i++) {
        if (strcmp(vart->vars[i]->name, name) == 0) {
            return vart->vars[i]->val;
        }
    }
    return NULL;
}
// set shell_vars[name] = value;
int setvar(struct shell_vars* vart, char* setname, char* setval) {
    if (setname == NULL || setval == NULL)
        return 1;
        
    for (int i = 0; i < vart->last_idx; i++) {
        if (strcmp(vart->vars[i]->name, setname) == 0) {
            strcpy(vart->vars[i]->val, setval);
            return 0;
        }
    }
    if (vart->last_idx >= num_vars)
        return 1;
        
    strcpy(vart->vars[vart->last_idx]->name, setname);
    strcpy(vart->vars[vart->last_idx]->val, setval);
    vart->last_idx++;
    return 0;
}
// display all variables in shell_vars
void disp_vars(struct shell_vars* vart) {
    for (int i = 0; i < vart->last_idx; i++) {
        fprintf(stderr, "%s %s\n", vart->vars[i]->name, vart->vars[i]->val);
    }
    fprintf(stderr, "\n");
}
// initialize shell_vars container
void init_shell_vars(struct shell_vars* vart) {
    vart->vars = (struct shell_var**) malloc(num_vars * sizeof(struct shell_var*));
    for (int i = 0 ; i < num_cmd_strs; i++) {
        vart->vars[i] = (struct shell_var*) malloc(1 * sizeof(struct shell_var));
    }
}
