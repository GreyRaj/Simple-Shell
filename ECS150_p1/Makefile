


sshell: sshell.o linklist.o cmd_formatters.o cmd_runners.o shell_vars.o
	gcc -g -Wall -Wextra -Werror -o sshell sshell.o linklist.o cmd_formatters.o cmd_runners.o shell_vars.o
	
sshell.o: sshell.c header.h
	gcc -g -Wall -Wextra -Werror -c sshell.c
	
linklist.o: linklist.c header.h
	gcc -g -Wall -Wextra -Werror -c linklist.c

cmd_formatters.o: cmd_formatters.c header.h
	gcc -g -Wall -Wextra -Werror -c cmd_formatters.c

cmd_runners.o: cmd_runners.c header.h
	gcc -g -Wall -Wextra -Werror -c cmd_runners.c

shell_vars.o: cmd_runners.c header.h
	gcc -g -Wall -Wextra -Werror -c shell_vars.c


clean:
	rm -fr *.o *.out *.exe
	
	