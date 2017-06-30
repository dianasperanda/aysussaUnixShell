#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include "parse.h"
#include "command.h"
#include "completion.h"

int main (int argc, char** argv)
{
  int pid, cpid;
  int l;
  initialize_readline ();	/* Bind our completer. */

  for ( ; done == 0; )
    {
      readline ("@>: ");

      if (rl_point == rl_end && *rl_line_buffer)
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
                add_history (rl_line_buffer);
                l = parsing();
                if (l == 0)
                    execute_line(rl_line_buffer);
            }
        }        
    }
  exit (0);
}

