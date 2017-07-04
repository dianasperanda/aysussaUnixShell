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

int com_list(char *);
int com_help(char *);
int com_cat(char *);
int com_cd(char *);
int com_pwd(char *);
int com_quit();
int com_remove(char *);
int com_user();
int com_make(char *);
int com_touch(char *);
int com_process(char *);
int com_kill(char *);
int com_move(char *);
int com_clear();
int com_exec(char *);
int com_umask(char *);
int com_exit();

int execute_line (char*);

int done;

typedef struct {
  char *name;
  rl_icpfunc_t *func;
} COMMAND;


COMMAND commands[] = {
  { "ls", com_list },
  { "help", com_help },
  { "cat", com_cat },
  { "cd", com_cd },
  { "pwd", com_pwd },
  { "quit", com_quit },
  { "rm", com_remove },
  { "whoami", com_user },
  { "mkdir", com_make },
  { "touch", com_touch },
  { "ps", com_process },
  { "kill", com_kill },
  { "mv", com_move },
  { "clear", com_clear },
  { "exec", com_exec },
  { "umask", com_umask },
  { "exit", com_exit },
  { (char *)NULL, (rl_icpfunc_t *)NULL }
};

COMMAND *find_command ();

int execute_line (char *line)
{ 
  register int i;
  COMMAND *command;
  char *word;

  /* Isolate the command word. */
  i = 0;
  while (line[i] && whitespace (line[i]))
    i++;
  word = line + i;

  while (line[i] && !whitespace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  command = find_command (word);

  if (!command)
    {
      fprintf (stderr, "%s: No such command for AySussa.\n", word);
      return (-1);
    }

  /* Get argument to command, if any. */
  while (whitespace (line[i]))
    i++;

  word = line + i;

  /* Call the function. */
  return ((*(command->func)) (word));
}

COMMAND *find_command (char *name)
{
  register int i;

  for (i = 0; commands[i].name; i++)
    if (strcmp (name, commands[i].name) == 0)
      return (&commands[i]);

  return ((COMMAND *)NULL);
}

static char syscom[1024];

int com_list(char *arg) {
  if (!arg)
    arg = "";

  sprintf (syscom, "ls -FClg %s", arg);
  return (system (syscom));
}

int com_help(char *arg)
{
    if (!arg)
        arg = "";

    sprintf (syscom, "help -dms %s", arg);
    return (system (syscom));
}

int com_cat(char *arg) 
{    
    if (!arg)
        perror("cat");
    
    sprintf (syscom, "cat %s", arg);
    return (system (syscom));
}

int com_cd(char *arg)
{
  if (chdir (arg) == -1)
    {
      perror (arg);
      return 1;
    }

  com_pwd ("");
  return (0);
}

int com_pwd(char *ignore)
{
  char dir[1024], *s;

  s = getcwd (dir, sizeof(dir) - 1);
  if (s == 0)
    {
      printf ("Error getting pwd: %s\n", dir);
      return 1;
    }

  printf ("Current directory is %s\n", dir);
  return 0;
}

int com_quit()
{
  done = 1;
  return (0);
}

int com_remove(char *arg) {
    
    if(!arg)
    {
        perror("rm")
        return -1;
    }
    
    sprintf (syscom, "rm %s", arg);
    
    sprintf (syscom, "rm -i %s", arg);
    return (system (syscom));
}

int com_user() {
    
    sprintf (syscom, "whoami");
    return (system (syscom));
}

int com_make(char *arg) {
    
    if(!arg)
    {
        perror("mkdir")
        return -1;
    }
    
    sprintf (syscom, "mkdir %s", arg);
    return (system (syscom));
}

int com_touch(char *arg) {
    
    if(!arg)
    {
        perror("touch")
        return -1;
    }
    
    sprintf (syscom, "touch %s", arg);
    return (system (syscom));
}

int com_process(char *arg) {
    
    sprintf (syscom, "ps -u %s", arg);
    return (system (syscom));
}

int com_kill(char *arg) {
    
    if(!arg)
    {
        perror("kill")
        return -1;
    }
    
    int pid = atoi(arg);
    
    sprintf (syscom, "kill %d", pid);
    return (system (syscom));
}

int com_move(char *arg) {
    
    if(!arg)
    {
        perror("mv")
        return -1;
    }
    
    sprintf (syscom, "mv %s", arg);
    return (system (syscom));
}

int com_clear() {
    
    sprintf (syscom, "clear");
    return (system (syscom));
}

int com_exec(char *arg) {
    
  sprintf (syscom, "exec -cla %s", arg);
  return (system (syscom));
}

int com_umask(char *arg) {
    
    if(!arg)
        arg = "";
    
    sprintf (syscom, "umask -pS %s", arg);
    return (system (syscom));
}

int com_exit() {
    
    int rtrnstatus;
    
    waitpid(getpid(), &rtrnstatus, 0);
    
    if (WIFEXITED(rtrnstatus))
        printf("Closing...\n");
    
    return com_quit();
}
