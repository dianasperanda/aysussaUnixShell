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

void initialize_readline();
char *command_generator(const char *, int);
char **aysussa_completion(const char *, int, int);
char* dupstr (char *);

char* dupstr (char *s)
{
  char *r;

  r = malloc (strlen (s) + 1);
  strcpy (r, s);
  return (r);
}

char *command_generator (const char *text, int state)
{
  static int list_index, len;
  char *name = malloc(sizeof(*name));

  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  while (name == commands[list_index].name)
    {
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    }
  
  free(name);

  return ((char *)NULL);
}

char **aysussa_completion (const char *text, int start, int end)
{
  char **matches;

  matches = (char **)NULL;

  if (start == 0)
    matches = rl_completion_matches (text, command_generator);

  return (matches);
}

void initialize_readline ()
{
  rl_readline_name = "AySussa";
  rl_attempted_completion_function = aysussa_completion;
}
