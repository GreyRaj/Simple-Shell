# **ECS 150 Project 1 Report**
### **Team Members: Rajveer Grewal and Ayush Saha**

## **General Summary**

In this project, our team created a simple shell that executes user-supplied
commands. It handles them with optional arguments. It handles a selection of
typically builtin commands. It also handles output redirection, as well as
piping of consecutive commands. Lastly, it handles output redirection of
commands to files or pipes, as well as simple environmental variables.

## **Testing and High-Level Design Choices**

### Command Line Input Parser

#### Testing (Overview):
We had to focus on each test case one at time, draw diagrams on paper, and talk
out ideas with each other to have more insight. We both worked on it at same
time to build up common understanding of the the Command Input Line.
			
Our process for testing our code is as follows: at each step, we would run a
variety of test cases. After ensuring the code passed the test case set, we
would move onto testing new cases and modifying the code to support them.
We found that splitting our code/methods into sections was easier (i.e.
splitting piped command handling apart from regular command handling), so we
ended up having entirely different functions and data structures to handle
different types of test cases.
					
We also decided to add new small checking components (i.e. variables to index
the number of commands seen and number of characters it has been since the last
redirection symbol). We also had to fix the ordering of checks/ updates.
Lastly, we decided to use a data structure (link list to store command
arguments), because it helped make it easier to handle command line string
parsing.

#### High-Level Design Choices
The main function in sshell.c handles some of the basic aspects of a command
line input. We store the entire command line in the array "cmd" and only the
first part (the command itself) in "cmd_updated". We also set "args" to be a
pointer to strings that store the arguments of said command. We store all of
these things before we can even begin the parsing process. We also decided to
set some flags here that will be useful later in the parcing process. "skipfork"
determines whether to skip the forking process if an error is detected when
parsing the command. "redir" is another flag used to determine whether output
redirection is involved in the command line input. "newout" is initialized as an
array that would store any potential new destination for the output to be
redirected to. "pipout" is similar to "redir", except it is a flag to determine
whether to pipe the output instead.

With all of these preparations out of the way, the last thing we included before
getting into the actual formatting of the command line was creating a linked
list and shell_vars data structure. The linked list "pipd_args_list" is
specifically to store arguments of piped commands. "variable_tb1" stores the
shell variables. With all of these parts of the command line input stored and
indicator flags set up, we just have to format the command line and execute the
commands.

We made the decision to seperate these two remaining tasks into different .c
files with different functions. This would help maintain sshell.c to be concise
and make our code easier to follow. We call either *format_standard_cmd* or
*format_piped_cmd*. These functions are described in "cmd_formatters.c". One
function handles standard commands and the other handles piped. They return the
command line input parsed and formatted as {cmd, args, NULL}. After getting back
a properly formatted command line, we call either the function
*run_standard_cmd* or *run_pip_cmd*. These functions are described in
"cmd_runners.c". These functions execute the now properly formatted command
line. Because of the while loop, the program prompts the user to enter their
next command upon the completion of the execution of the previous command.

### Piping

#### Testing
For testing and debugging, we made easy functions that helped us. We printed the
entire args list data structure of the linked list for example in order to get
an inside look as to how the commands and arguments were being stored. We
actually then had to copy over the terminal output into a small representational
function of our main project's piping loop, and try seeing if it works then.
This helped us realize that the compiler was reading our commands incorrectly,
and that there was an invisible space being printed. The compiler was accessing
and failing on this invisible space because it thought it was an input for the
piped command. We then had to set all of the possible unused parts of the
arguments as NULL properly. Then our piping implementation worked finally, due
to this testing process.

#### High-Level Design Choices
Our implementation of piping stems from the basic scripts provided from lecture.
We just built on top of that basic structure. We had the child writing to the
pipe, then the parent reading form it. Then from there, we had it repeat for a
new child branching out from the parent. We used dup2() to manipulate the
reading and writing of these commands.  Then, since you can technically pipe as
many commands as you want, we had our code do this in a a for loop, so that we
could handle as many commands being piped. Child processes would be created,
write to a pipe, and then die whilst the parent stays alive controlling the for
loop. Our main design choice came when we realized we needed to store the
arguments of the various piped commands, which is why we have linklist.c. The
linked list allows us to store these arguments as we execute the commands.

## Purpose of each additional .C File

### **cmd_formatters.c**

*format_standard_cmd()* takes a command string, tokenizes it into words, and
checks if the command is a valid standard command, storing the arguments into an
array. If output redirection is specified (e.g., ">" or ">&"), it will store the
new output file name in a separate variable. If the command starts with ">" or
"|", it is considered a bad start and an error message is printed. If there are
too many arguments, it also prints an error message. If the command ends with
">" or "|", an error message is printed as well.

*format_piped_cmd()* takes a similar approach but is used for piped commands
(commands that involve piping the output of one command to the input of
another). The function tokenizes the command string and modifies the arguments
list based on whether a pipe symbol ("|") is found. If output redirection is
specified, it will store the new output file name in a separate variable. If the
command contains too many arguments or the number of commands exceeds the
maximum allowed, an error message is printed.

Both functions take a *struct shell_vars* argument, which is a structure
containing variables that can be used within the command. The *getval()*
function is called to retrieve the value of a variable, which can be specified
in a command argument with the"$" prefix (e.g., "$HOME"). If an error is
encountered during parsing, the "skipfork" pointer is set to 1, indicating that
a fork should not be executed.


### **cmd_runners.c**
This .c file is a program for executing the commands that we run through it in
'sshell.c'. The program can handle both standard commands and piped commands. 

The program consists of two functions: *run_pip_cmd* and *run_standard_cmd*.
*run_pip_cmd* is called when the input command contains one or more pipes, while
*run_standard_cmd* is called when there are no pipes.

The function *run_pip_cmd* takes a 'struct list' object (a linked list of
commands and their arguments, explained in linklist.c), a string representing
the output file, an integer indicating whether to redirect output to a file, and
a string representing the full command entered by the user. The function first
creates a new process for each command in the pipeline and sets up the pipes
connecting them. It then executes each command in turn by calling "execvp". The
output of each command is directed to the input of the next command in the
pipeline. If output redirection is requested, the output of the last command is
redirected to the specified file. The function waits for each process to
complete and prints an exit status message for each command.

The function *run_standard_cmd* takes an array of strings representing the
arguments for a single command, a string representing the output file, an
integer indicating whether to redirect output to a file, and a string
representing the full command entered by the user. The function creates a new
process to execute the command using "execvp", waits for the process to
complete, and prints an exit status message for the command. If output
redirection is requested, the output of the command is redirected to the
specified file.

This .c file also handles the "cd" command as well as handling the variable
command "set".

### **linklist.c**
This .c file provides a framework for creating and manipulating linked lists
with nodes that contain arguments lists. This was for handling piped command
line inputs.

There are a set of functions that operate on the linked list as a whole. These
include a function to display the argument lists of all nodes in the list, a
function to reset the data in the list, a function to initialize a new list with
nodes, and functions to set the value of a specific argument in a specific node
and to increment the list accessor to the next node.

There are also functions to handle specific tasks related to the linked list,
including setting the exit status for the current node, printing the exit status
for all nodes in the list, and an unused function to check if a command is a
variable set command.

### **shell_vars.c**
This .c file  implements simple environmental variables for a shell. It includes
several functions that interact with a struct called "shell_vars".

The shell_vars struct contains an array of pointers to shell_var structs, each
of which contains a name and a val. The name is a single character (from 'a' to
'z') and val is a string value assigned to that variable.

The first function, *getval*, takes in a shell_vars struct and a name character
and returns the value associated with that variable. It searches through the
array of shell_var structs to find the one with a matching name field and
returns the associated val field.

The second function, *setvar*, takes in a shell_vars struct, a setname
character, and a setval string. It searches through the array of shell_var
structs to find the shell_var with a matching name field. If it finds a match,
it updates the associated val field with the provided setval. If no matching
shell_var is found, it creates a new one and adds it to the array.

The third function, *disp_vars*, takes in a shell_vars struct and prints out all
of the name/val pairs in the array of shell_var structs.

The fourth function, *init_shell_vars*, takes in a shell_vars struct and
initializes its array of shell_var pointers. It dynamically allocates an array
of num_vars pointers to shell_var structs and sets each one to point to a new
shell_var struct.