#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<strings.h>
#include<time.h>
#include<sys/types.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pwd.h>
#include "utils.h"

#include<readline/readline.h>
#include<readline/history.h>

#include "command.h"
#include "completion.h"

int parsing (char*);

typedef struct {
    char* first_word;
    char* second_word;
    char* argument;
} WORDS;

WORDS words;

WORDS handle_line(char*, char*);
int redirection_output(WORDS);
void interrupt_handler(int sigInt);

int parsing (char *buffer)
{
    WORDS result;
    int i = 0;
    int found = 0;
    
    for(i = 0; i < strlen(buffer); i++) {
        
        if(!rl_alphabetic(buffer[i]) && !isspace(buffer[i]))
        {
            if(buffer[i] == '>')
            {
                result = handle_line(buffer, ">");
                found = 1;
                //system(result.first_word);
                redirection_output(result);
            }
            else if(buffer[i] == '<')
            {
                result = handle_line(buffer, "<");
                found = 1;
            }
            else if(buffer[i] == '|')
            {
                result = handle_line(buffer, "|");
                found = 1;
            }
        }
        else {
            execute_line(buffer);
            return 0;
        }
    }
    
    return found;
}

WORDS handle_line(char *buffer, char* arg)
{
    words.first_word = strtok(buffer, arg);
    words.second_word = strtok(NULL, arg);
    
    words.first_word = stripwhite(words.first_word);
    words.second_word = stripwhite(words.second_word);
    
    words.argument = arg;
    
    return words;
}

void interrupt_handler(int sigInt) {
	if(sigInt == SIGINT) {
		printf("Error! Interrupt signal, CHILD process terminated");
		fflush(stdout);
		kill(getpid(), SIGTERM);
	}
}

int redirection_output(WORDS w)
{
	int pid, cpid, wstatus, fd;

	signal (SIGINT, interrupt_handler);

	pid = fork();
	if(pid == -1) {
		perror("fork");
		return 0;
	}
	else if (pid==0) {

		fd = open(w.second_word, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if (fd < 0) {
			perror("open");
			return -1;
		}
		dup2(fd, STDOUT_FILENO);
        
        	if (fd != STDOUT_FILENO) {
           		 close(fd);
		}
        
      		  sleep(5);
              static char syscom[1024];
              sprintf (syscom, w.first_word);
      		  return (system (syscom));
	}
	else {
		cpid=waitpid(0, &wstatus, 0);
	}

	return 0;
}
    
    
    
    
    
    
    
    
    
    
    
    
