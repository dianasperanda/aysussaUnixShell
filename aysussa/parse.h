#include "utils.h"
#include "command.h"
#include "completion.h"

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
int piping(char *, char *, char *, char*);
int handle_line_for_pipe(char *buffer);

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
                handle_line_for_pipe(buffer);
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

int handle_line_for_pipe(char *buffer) {
    
    char *token;
    char first_pipe[20] = {'\0'};
    char second_pipe[20] = {'\0'};
    char first_word[20] = {'\0'};
    char second_word[20] = {'\0'};
    
    token = strtok(buffer, "|");
    strcpy(first_word, token);
    while( token != NULL ) {   
        token = strtok(NULL, "|");
            if(token)
                strcpy(second_word, token);
    }
    
    token = strtok(first_word, " ");
    token = strtok(NULL, " ");
    if(token)
        strcpy(first_pipe, token);
    
    token = strtok(second_word, " ");
    token = strtok(NULL, " ");
    if(token)
        strcpy(second_pipe, token);
    
    remove_spaces(first_word);
    if(strcmp(first_pipe, " ") != 0)
        remove_spaces(first_pipe);

    remove_spaces(second_word);
    if(strcmp(second_pipe, " ") != 0)
        remove_spaces(second_pipe);
    
    piping(first_word, second_word, first_pipe, second_pipe);
    
    return 0;
}

void redirection_output(char *first_word, char *second_word, char* argument)
{
    int status;
    int outpt;
    int save_out;

    outpt = open(second_word, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (-1 == outpt) {
           perror(second_word);
           return;
    }

    save_out = dup(STDOUT_FILENO);
    if (-1 == dup2(outpt, STDOUT_FILENO)) {
                perror("Cannot redirect stdout");
                return;
    }   
              
    char syscom[1024];
    sprintf(syscom, first_word);
    system(syscom);
            
    dup2(save_out, STDOUT_FILENO);
    close(outpt);
    close(save_out);
            
    waitpid(0, &status, 0);
            
    if (WIFEXITED(status))
          printf("Child's exit code %d %d %d\n", WEXITSTATUS(status), getpid(), getppid());
    else
          printf("Child did not terminate with exit\n");
}

int redirection_input(WORDS w)
{
        int status;
        int intput;
        int save_in;
        
        intput = open(w.second_word, O_RDONLY, 0);
        if (-1 == intput) {
           perror(w.second_word);
           _exit(0);
        }
        
        save_in = dup(STDIN_FILENO);
        if (-1 == dup2(intput, STDIN_FILENO)) {
                perror("Cannot redirect stdin");
                _exit(0);
        }   
              
        char syscom[1024];
        sprintf(syscom, w.first_word);
        system(syscom);
            
        dup2(save_in, STDIN_FILENO);
        close(intput);
        close(save_in);
            
        waitpid(0, &status, 0);
                
        if (WIFEXITED(status))
               printf("Child's exit code %d %d %d\n", WEXITSTATUS(status), getpid(), getppid());
        else
               printf("Child did not terminate with exit\n"); 
        
        return 0;
}

int piping(char *first_word, char *second_word, char *first_pipe, char *second_pipe)
{
    int pipefd[2];
    pid_t pid, pid2;
    int status;

    if (pipe(pipefd) == -1) {
        
        perror("pipe");
        _exit(0);
    }
        
    pid = fork();
    
    if (pid == -1) {
        
        perror("fork");
        _exit(0);
    }
    if (pid == 0) {               

        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("Firt child dup2");
            _exit(0);
        }

        close(pipefd[0]);
        
        if (pipefd[1] != STDOUT_FILENO)
            close(pipefd[1]);
        
        if(strcmp(first_pipe, "\0") == 0)
            execlp(first_word, first_word, NULL);
        else
            execlp(first_word, first_word, first_pipe, NULL);
        
    }    
    close(pipefd[1]);

    pid2 = fork();
    
    if (pid2 == -1) {
        perror("fork");
        kill(pid, SIGTERM);
        _exit(0);
    }
    if(pid2 == 0) {
        if(dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("2nd child dup2");
            kill(pid, SIGTERM);
            _exit(0);
        }
        
        if (pipefd[0] != STDIN_FILENO)
            close(pipefd[0]);
            
        if(strcmp(second_pipe, "\0") == 0)
            execlp(second_word, second_word, NULL);
        else
            execlp(second_word, second_word, second_pipe, NULL);
    }
    
    close(pipefd[0]);
    
    waitpid(pid, NULL, 0);
    waitpid(pid2, &status, 0);
    
    if (WIFEXITED(status))
            printf("Child's exit code %d %d %d\n", WEXITSTATUS(status), getpid(), getppid());
    else
            printf("Child did not terminate with exit\n"); 
    
    return 0;
}

int background(char *buff)
{
    int pid;
    int status;
    
    pid = fork();
    
    if(pid < 0) perror("fork");
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
