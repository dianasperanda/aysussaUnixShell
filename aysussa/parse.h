#include "utils.h"
#include "command.h"
#include "completion.h"

#define READ_END 0
#define WRITE_END 1

int parsing (char*);


typedef struct {
    char* first_word;
    char* second_word;
    char* argument;
} WORDS;

WORDS words;

WORDS handle_line(char*, char*);
int redirection_output(WORDS);
int redirection_input(WORDS);
int piping(WORDS);

int parsing (char *buffer)
{
    WORDS result;
    int i;
    
    for(i = 0; i < strlen(buffer); i++) {
        
        if(!rl_alphabetic(buffer[i]) && !isspace(buffer[i]))
        {
            if(buffer[i] == '>')
            {
                result = handle_line(buffer, ">");
                redirection_output(result);
            }
            else if(buffer[i] == '<')
            {
                result = handle_line(buffer, "<");
                redirection_input(result);
            }
            else if(buffer[i] == '|')
            {
                result = handle_line(buffer, "|");
                piping(result);
            }
        }
        else {
            execute_line(buffer);
            return 0;
        }
    }
    
    return 0;
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

int redirection_output(WORDS w)
{
	int pid, cpid, wstatus, fd;

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
        
        //nadodati još za cpid
	}

	return 0;
}

int redirection_input(WORDS w)
{
    pid_t cpid;
    int rtrnstatus;

    cpid = fork();

    if (cpid < 0) {
           perror("fork failed");
           return -1;
    }
    else if (cpid == 0) {
        static char syscom[1024];
        sprintf (syscom, w.first_word);
        return (system (syscom));
    }
    else {
        
         waitpid(cpid, &rtrnstatus, 0);
 
         if (WIFEXITED(rtrnstatus))
                 printf("Child's exit code %d\n", WEXITSTATUS(rtrnstatus));
         else
                 printf("Child did not terminate with exit\n"); 
    }
    
    return 0;
}

int piping(WORDS w)
{
    int pipefd[2];
    int status;
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }
        
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {               

        if (dup2(pipefd[WRITE_END], STDOUT_FILENO) == -1) {
            perror("dup2");
            return -1;
        }

        close(pipefd[READ_END]);
        
        if (pipefd[WRITE_END] != STDOUT_FILENO)
            close(pipefd[WRITE_END]);
        
        static char syscom[1024];
        sprintf (syscom, w.first_word);
        return (system (syscom));
        
    }    
    
    close(pipefd[WRITE_END]);
    waitpid(pid, NULL, 0);
    
    return 0;
}
    
    
    
