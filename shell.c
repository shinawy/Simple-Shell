#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define BUFFER_LEN 1024
#define MAX_PIPE_NUM 5
/* begin */
size_t read_command(char *cmd) {
if(!fgets(cmd, BUFFER_LEN, stdin)) /* get command and put it in line */
return 0; /* if user hits CTRL+D break */
size_t length = strlen(cmd); /*  get command length */
if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; /*  clear new line */
return strlen(cmd); /*  return length of the command read */
}
int build_args(char * cmd, char ** argv) {
char *token; /* split command into separate strings */
token = strtok(cmd," ");
int i=0;
while(token!=NULL){/*  loop for all tokens */
argv[i]=token; /*  store token */
token = strtok(NULL," "); /*  get next token */
i++; /*  increment number of tokens */
}
argv[i]=NULL; /* set last value to NULL for execvp */
return i; /*  return number of tokens */
}
void set_program_path (char * path, const char * bin, char * prog) {
memset (path,0,1024); /*  intialize buffer */
strcpy(path, bin); /* copy /bin/ to file path */
strcat(path, prog); /* add program to path */
int i;
for(i=0; i<strlen(path); i++) /* delete newline */
if(path[i]=='\n') path[i]='\0';
}



int find_char (char** argv, char* c, int arr[],int  argc){

	int index=0; 
	int i; 
	for (i=0; i<argc; i++){
		if (strcmp(argv[i],c)==0)
		{
		arr[index]=i;
		index++; 
		}

	}
	return index;
}


int  handle_redirection(int array_variable [] , char ** argv,char** argv_updated, int arr [], int argc){

/* search if any input redirection is there; */
int is_redirected= 0;

find_char(argv, "<", arr, argc);



if (arr[0]!=-1){
	
	array_variable [0]=arr[0];

is_redirected=1; 
array_variable [2]= open(argv[arr[0]+1], O_RDONLY);
	if (array_variable [2]<0){
		perror (argv[arr[0]+1]);
		exit(1);
	}
	dup2(array_variable [2], 0);
}

/* reset the array */
arr[0]=-1;

/* search if any output redirection is there; */
find_char(argv, ">", arr, argc);

if (arr[0]!=-1){
array_variable [1]= arr[0];
/* printf ("output red is true and outred = %d\n", array_variable [1] );  */
is_redirected=1;
/*printf("name of the file is %s\n", argv [arr[0]+1]);*/
array_variable [3]= open(argv[arr[0]+1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (array_variable [3]<0){
		perror (argv[arr[0]+1]);
		exit(1);
	}
	dup2(array_variable [3], 1);
}

int stopindex= (array_variable [0]<array_variable [1])? array_variable [0]: array_variable [1];


 


if (is_redirected){
	int i_2=0;
	int i=0;
	for (i=0;i <stopindex; i++){
		argv_updated[i]= argv[i];
		
		i_2=i;
	}
	
	argv_updated[i_2+1]= NULL;

}


return is_redirected;

}


int  create_subargument(char** argv, char ** argvup, int startindex, int argc){
	int i=0; int j=0;
int counter=0;


	for (i=startindex; i< argc; i++){
		
		

		if (!strcmp(argv[i], "|")){
			
			
			
			argvup[i]='\0';
			return counter;	
		}
		
		else{
				
			
			argvup[j]= argv[i];
			counter++;
			
		}

		argvup [j+1]= '\0';
		
		
		j++;
	}
	return counter ;


}




int main(){
char line[BUFFER_LEN]; /* get command line */
char* argv[100]; /* user command */
char* argvup [100]; /*this is for piping*/
char* argv_updated[100];
char* argv_echo[50]; 
char line_echo [50];
const char* bin= "/bin/"; /* set path at bin */
char path[1024]; /* full file path */
char path_echo [512];
int argc, argc_echo; /* arg count */
int pid1; int pid2; int pid_echo; 
char value [512]; 
char var [2]; 
char s[100];
// chdir(getenv("HOME"));
// chdir("../..");
while(1){
	
printf("%s>> ", getcwd(s,100)); /* print shell prompt */
if (read_command(line) == 0 )
{printf("\n"); break;} /*  CRTL+D pressed */
if (strcmp(line, "exit") == 0) break; /* exit */

if (strchr(line, '`')!=NULL){
	strncpy(line_echo, line+3, strlen(line)-2);
	line_echo[strlen(line)-4]='\0';
	argc_echo= build_args (line_echo,argv_echo); /*  build program argument */
	set_program_path (path_echo,bin,argv_echo[0]); /*  set program full path */
	

}

char* is_piped=  strchr(line, '|');
 /* printf("testing for pipe character in %s  is %s\n", line,  strchr(line, '|'));*/
argc = build_args (line,argv); /*  build program argument */

set_program_path (path,bin,argv[0]); /*  set program full path */
char* str=argv[0];
 /* printf("argv[0]==%s\n%d\n%d\n", argv[0], argv[0]==str, !strcmp(argv[0], "cd")); */


/* this is for doing the search for certain characters// */

 /* printf("testing for pipe character in %s  is %s\n", line,  strchr(line, '|'));*/


int* arr; 
arr=(int*) malloc (argc * sizeof (int));
int i;
for (i=0; i<argc; i++)
	arr[i]=-1; 




if (!strcmp(argv[0],"cd")){
printf("this is cd command\n");
chdir(argv[1]);
printf("path: %s\n", getenv("HOME"));
printf("path:%s\n",getcwd(s,100));
}

else if (strchr(argv[0], '=')!=NULL){
	int fd_echo[2];
	pipe(fd_echo);
	strncpy (var,argv[0],1);
	if (strchr(argv[0],'`')!= NULL){
		
		pid_echo= fork(); 
		if(pid_echo==0){
			dup2(fd_echo[1],1);
			close(fd_echo[0]); 
		execve(path_echo,argv_echo,0); 
		fprintf(stderr, "Child process could not do execve\n");
		}else wait(NULL); 

		close (fd_echo[1]);
		read (fd_echo[0], value, 512 );
		close(fd_echo[0]); close (fd_echo[1]);
		
		
	}

	else {
	strncpy (value,argv[0]+2,strlen(argv[0])-1);	
	}
	

setenv(var, value, 1 );
continue;

}


else if (!strcmp(argv[0],"echo")){
if (strchr(argv[1],'$')!= NULL){

strncpy(var, argv[1]+1, strlen(argv[1]));

printf("%s\n", getenv(var));
continue;

}
else {

pid2= fork(); 
if(pid2==0){ 
execve(path,argv,0); 
fprintf(stderr, "Child process could not do execve\n");
}else wait(NULL); 

}

}





else if (is_piped != NULL){



int numofpipes;
numofpipes= find_char(argv, "|", arr, argc);

if (numofpipes> MAX_PIPE_NUM){
printf("Number of Pipes exceeds the maximum number of pipes\n");
continue;
}


/*create 2d array of pipes*/
int** fd;  int status;
fd = (int**) malloc (numofpipes * sizeof (int*)); 

int * pid; 
	pid= (int* ) malloc ((numofpipes+1) * sizeof(int));

for (i=0; i<numofpipes; i++)
	fd[i] = (int*) malloc (2 * sizeof (int)); 

int j;

for ( i=0; i< numofpipes; i++)
	pipe(fd[i]);


for ( i=0; i< numofpipes; i++)
	printf("%d\n", arr[i]);

int inner; 

int is_redirected1;

int inred1=argc;
int infiledesc1 = -1;
int outred1=argc; 
int outfiledesc1 =-1;

int array_variable1[4];
array_variable1[0]= inred1;
array_variable1[1]= outred1;
array_variable1[2]= infiledesc1;
array_variable1[3]= outfiledesc1;

int arr1[2];
arr1[0]=-1; 
arr1[1]=-1;

int argcup;




for (i=0; i<numofpipes+1; i++){

		inred1=argc;
		infiledesc1 = -1;
		outred1=argc; 
		outfiledesc1 =-1;
		array_variable1[0]= inred1;
		array_variable1[1]= outred1;
		array_variable1[2]= infiledesc1;
		array_variable1[3]= outfiledesc1;
		
		/* printf("arrayofindex%d+1 is %d\n", i, i==0 ? 0: arr[i-1] ); */
		argcup=create_subargument(argv, argvup, i==0 ? 0: arr[i-1] + 1, argc);
		set_program_path(path,bin, argvup[0]);
		/*
		j=0;
		while (argvup[j] != '\0'){
			
		printf ("Subargument is %s", argvup[j]);
		j++;
		}
		*/

		printf("\n"); 
		

		 if (i==0){
			 	
			
			pid[i] = fork();
			if (pid[i]==0){
				/* this is for closing the file. */
				
				for (inner=0; inner<numofpipes; inner++){
					if (inner==i)
						close (fd[inner][0]);
					
					else{
					
						close (fd[inner][0]);
						close (fd[inner][1]);					
					}
				}
				
				

				
				dup2(fd[i][1],1);
				
				is_redirected1= handle_redirection(array_variable1, argvup, argv_updated, arr1, argcup);

				inred1 = array_variable1[0];
				outred1= array_variable1[1];
				infiledesc1= array_variable1[2];
				outfiledesc1= array_variable1[3];



				if(is_redirected1){
					
					execve(path,argv_updated,0); /*  if failed process is not replaced */
					perror(argv_updated[0]);	/* it failed! */
					
				}	
				else {
					execve (path, argvup,0);
					perror(argvup[0]);	/* it failed! */
					
					}

				
				perror(argvup[0]);	/* it failed! */

			}
		 }

		 else if (i==numofpipes){

			pid[i] = fork();
			if (pid[i]==0){
				/* this is for closing the file. */
				
				for (inner=0; inner<numofpipes; inner++){
					
					
					 if (inner== i-1)
						close(fd [inner][1]);
					
					else{
					
						close (fd[inner][0]);
						close (fd[inner][1]);
					
					}

				}
				
				
				dup2(fd[i-1][0],0);
				
				is_redirected1= handle_redirection(array_variable1, argvup, argv_updated, arr1, argcup);

				inred1 = array_variable1[0];
				outred1= array_variable1[1];
				infiledesc1= array_variable1[2];
				outfiledesc1= array_variable1[3];



				if(is_redirected1){
					
					execve(path,argv_updated,0); /*  if failed process is not replaced */
					perror(argv_updated[0]);	/* it failed! */
					
				}	
				else {
					execve (path, argvup,0);
					perror(argvup[0]);	/* it failed! */
					
					}	

			}


		 }


		 else {
			 pid[i] = fork();
			 if (pid[i]==0){
				/* this is for closing the file. */
				
				for (inner=0; inner<numofpipes; inner++){
					if (inner==i-1)
						close (fd[inner][1]);
					
					else if (inner == i )
						close (fd[inner][0]);				
					else{
					
						close (fd[inner][0]);
						close (fd[inner][1]);
					
					}


				}
				
				
				dup2(fd[i-1][0],0);
				
				dup2(fd[i][1],1);
				
				
				is_redirected1= handle_redirection(array_variable1, argvup, argv_updated, arr1, argcup);

				inred1 = array_variable1[0];
				outred1= array_variable1[1];
				infiledesc1= array_variable1[2];
				outfiledesc1= array_variable1[3];



				if(is_redirected1){
					
					execve(path,argv_updated,0); /*  if failed process is not replaced */
					perror(argv_updated[0]);	/* it failed! */
					
				}	
				else {
					execve (path, argvup,0);
					perror(argvup[0]);	/* it failed! */
					
					}

				 
				perror(argvup[0]);	/* it failed! */

				
				
			}

		 }
	 
	 		


	}
	
	/* this is for closing the pipes. */
		for (inner=0; inner<numofpipes; inner++){
								
				close (fd[inner][0]);
				close (fd[inner][1]);	
		}



for (i=0 ;i < numofpipes + 1; i++){
		while ((pid[i] = wait(&status)) != -1)	/* pick up all the dead children */
		fprintf(stderr, "process %d exits with %d\n", pid[i], WEXITSTATUS(status));
	}

free(pid);



for ( i=0; i< numofpipes; i++)
	free(fd[i]);

free(fd);
is_piped=NULL;
continue;



}



else{


pid1= fork(); /* fork child */
if(pid1==0){ /* Child */

/* this is for doing the search for certain characters// */


for (i=0; i<argc; i++)
	arr[i]=-1; 

/* search if any input redirection is there; */
int is_redirected;

int inred=argc;
int infiledesc = -1;
int outred=argc; 
int outfiledesc =-1;
int array_variable[4];
array_variable[0]= inred;
array_variable[1]= outred;
array_variable[2]= infiledesc;
array_variable[3]= outfiledesc;



is_redirected= handle_redirection(array_variable, argv, argv_updated, arr, argc);

inred = array_variable[0];
outred= array_variable[1];
infiledesc= array_variable[2];
outfiledesc= array_variable[3];


free (arr);
	if(is_redirected){
		printf("the path is %s\n", path);
		execve(path,argv_updated,0); /*  if failed process is not replaced */
		if (outred!=argc)
			close(outfiledesc);
		if (inred!=argc)
			close(infiledesc);
	}	
	else 
		execve (path, argv,0);

	/* printf("this is after execve!!\n"); */
	/*  then print error message */
fprintf(stderr, "Child process could not do execve\n");
}else wait(NULL); /* Parent */
}

}

return 0;
}
