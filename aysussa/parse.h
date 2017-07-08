#include "utils.h"
#include "command.h"
#include "completion.h"

#define READ_END 0
#define WRITE_END 1

int parsing (char*);
int background (char*);

typedef struct {
    char* first_word;
    char* second_word;
    char* argument;
} WORDS;

WORDS words;

WORDS handle_line(char*, char*);
void redirection_output(char *, char*, char*);
int redirection_input(WORDS);
int piping(WORDS);

int parsing (char *buffer)
{
    WORDS result;
    int i;
    int found = 0;
    int length = strlen(buffer);
    
    if(strcmp(&buffer[length - 1], "&") == 0)
    {
        buffer[length - 1] = '\0';
        length -= 1;
        background(buffer);
    }
    
    for(i = 0; i < strlen(buffer); i++) {
        
        if(!rl_alphabetic(buffer[i]) && !isspace(buffer[i]))
        {
            if(buffer[i] == '>')
            {
                found = 1;
                result = handle_line(buffer, ">");
                redirection_output(result.first_word, result.second_word, result.argument);
            }
            else if(buffer[i] == '<')
            {
                found = 1;
                result = handle_line(buffer, "<");
                redirection_input(result);
            }
            else if(buffer[i] == '|')
            {
                found = 1;
                result = handle_line(buffer, "|");
                piping(result);
            }
        }
    }
    
    if(found == 0) {
            execute_line(buffer);
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

void redirection_output(char *word1, char *word2, char* argument)
{
    pid_t cpid;
    int rtrnstatus;
    int outpt;
    int save_out;

    outpt = open(word2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (-1 == outpt) {
           perror(word2);
           return;
    }

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
            
            char syscom[1024];
            sprintf(syscom, word1);
            system(syscom);
            
            dup2(save_out, fileno(stdout));
            close(outpt);
            close(save_out);
        } 
                          
        else {
            waitpid(cpid, &rtrnstatus, 0);
        }
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
                execlp(w.first_word, w.first_word, w.second_word, NULL);
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

int background(char *buff)
{
    int pid;
    int status;
    
    pid = fork();
     
    if(pid == 0) {
        
        setpgid(pid, 0);
        execvp(&buff[0], &buff);
        exit(1);
    }
    else
    {
        waitpid(pid, &status, WUNTRACED);
        
        if (WIFEXITED(status))
                 printf("Child's exit code %d %d %d\n", WEXITSTATUS(status), getpid(), getppid());
        else
                 printf("Child did not terminate with exit\n"); 
    }
    
    return 0;
}
