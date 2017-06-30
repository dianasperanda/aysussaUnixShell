#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pwd.h>
#include <wait.h>
#include "utils.h"

int parsing ();

typedef struct {
    char* first_word;
    char* second_word;
    char* argument;
} WORDS;

WORDS words;

WORDS handle_line(char*, char*);
void redirection_output(WORDS);


int parsing ()
{
    WORDS result;
    int i = 0;
    int found = 0;
    
    for(i = 0; i < strlen(rl_line_buffer); i++) {
        
        if(!rl_alphabetic(rl_line_buffer[i]) && !isspace(rl_line_buffer[i]))
        {
            if(rl_line_buffer[i] == '>')
            {
                result = handle_line(rl_line_buffer, ">");
                found = 1;
                redirection_output(result);
            }
            else if(rl_line_buffer[i] == '<')
            {
                result = handle_line(rl_line_buffer, "<");
                found = 1;
            }
            else if(rl_line_buffer[i] == '|')
            {
                result = handle_line(rl_line_buffer, "|");
                found = 1;
            }
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

void redirection_output(WORDS w)
{
    pid_t cpid;
    int rtrnstatus;
    int outpt;
    int save_out;

    outpt = open(w.second_word, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (-1 == outpt) {
           perror(w.second_word);
           return;
    }

    /* Save standard output */
    save_out = dup(fileno(stdout));
   
    if (-1 == dup2(outpt, fileno(stdout))) {
           perror("Cannot redirect stdout");
           return;
     }   

        cpid=fork();

        if (cpid < 0 ) {
                perror("Fork failed");
                return;
        }           
        else if (cpid == 0) {
        		execlp(w.first_word, w.second_word, w.argument, NULL);
        } 
                          
        else {
                /* Wait for child process to finish */
                waitpid(cpid, &rtrnstatus, 0);

                /* Restore standard output */
                dup2(save_out, fileno(stdout));
                close(save_out);
                close(outpt);

                if (WIFEXITED(rtrnstatus))
                        printf("Child's exit code %d\n", WEXITSTATUS(rtrnstatus));
                else
                        printf("Child did not terminate with exit\n");          
        }   
}
    
    
    
    
    
    
    
    
    
    
    
    
