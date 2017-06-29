#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include "utils.h"
#include "command.h"
#include "completion.h"

int main (int argc, char** argv)
{
  char *line, *s;
  int pid, cpid;

  initialize_readline ();	/* Bind our completer. */

  for ( ; done == 0; )
    {
      line = readline ("@>: ");

      if (!line)
        break;
      
      s = stripwhite (line);

      if (*s)
        {
            pid = fork();
            if (pid < 0) {
                perror("fork");
                return 0;
            }
            else if (pid != 0) {
                cpid = wait(NULL);
                printf("[CPID %5d]: Terminated\n", cpid);
            }
            else {
                add_history (s);
                //parsing (s);
                execute_line(s);
                printf("[PID %5d]: Terminated\n", pid);
            }
        }

      free (line);
    }
  exit (0);
}

