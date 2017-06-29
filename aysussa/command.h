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
int com_cd(char *);
int com_pwd(char *);
int com_quit(char *);
int com_remove(char *);
int com_user();
int com_make(char *);
int com_touch(char *);
int com_process(char *);
int com_kill(char *);
int com_move(char *);
int com_clear();

int execute_line (char*);

int done;

typedef struct {
  char *name;
  rl_icpfunc_t *func;
  char *doc;
} COMMAND;


//builin
COMMAND commands[] = {
  { "ls", com_list, "List files in DIR" },
//   { "help", com_help, "Display this text" },
  { "cd", com_cd, "Change to directory DIR" },
  { "pwd", com_pwd, "Print the current working directory" },
  { "quit", com_quit, "Quit using Aysussa" },
  { "rm", com_remove, "Remove FILE" },
  { "whoami", com_user, "Who am I?" },
  { "mkdir", com_make, "Make DIR" },
  { "touch", com_touch, "Make FILE" },
  { "ps", com_process, "List all your processes" },
  { "kill", com_kill, "Kill process" },
  { "mv", com_move, "Move file or rename" },
  { "clear", com_clear, "Clear" },
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
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
  register int i;
  int printed = 0;

  for (i = 0; commands[i].name; i++)
    {
      if (!*arg || (strcmp (arg, commands[i].name) == 0))
        {
          printf ("%s\t\t%s.\n", commands[i].name, commands[i].doc);
          printed++;
        }
    }

  if (!printed)
    {
      printf ("No commands match `%s'.  Possibilties are:\n", arg);

      for (i = 0; commands[i].name; i++)
        {
          /* Print in six columns. */
          if (printed == 6)
            {
              printed = 0;
              printf ("\n");
            }

          printf ("%s\t", commands[i].name);
          printed++;
        }

      if (printed)
        printf ("\n");
    }
  return (0);
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

int com_quit(char *arg)
{
  done = 1;
  return (0);
}

int com_remove(char *arg) {
    if(!arg)
        fprintf(stderr, "Missing argument!");
    
    sprintf (syscom, "rm -i %s", arg);
    return (system (syscom));
}

int com_user() {    
    sprintf (syscom, "whoami");
    return (system (syscom));
}

int com_make(char *arg) {
    if(!arg)
        fprintf(stderr, "Missing argument!");
    
    sprintf (syscom, "mkdir %s", arg);
    return (system (syscom));
}

int com_touch(char *arg) {
    if(!arg)
        fprintf(stderr, "Missing argument!");
    
    sprintf (syscom, "touch %s", arg);
    return (system (syscom));
}

int com_process(char *arg) {
    sprintf (syscom, "ps -u %s", arg);
    return (system (syscom));
}

int com_kill(char *arg) {
    int pid = atoi(arg);
    sprintf (syscom, "kill %d", pid);
    return (system (syscom));
}

int com_move(char *arg) {
    sprintf (syscom, "mv %s", arg);
    return (system (syscom));
}

int com_clear() {
    sprintf (syscom, "clear");
    return (system (syscom));
}

