


shell.exe: shell.o
	gcc shell.o -o shell.exe

shell.o: shell.s
	gcc -c shell.s -o shell.o

shell.s: shell_pre.c
	gcc -S shell_pre.c -o shell.s

shell_pre.c: shell.c 
	gcc -E -ansi shell.c -o shell_pre.c 

run: 
	gcc shell.c -o newshell.exe 
	./newshell.exe 

clean: 
	rm shell_pre.c shell.s shell.o shell.exe 
