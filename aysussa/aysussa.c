#include "parse.h"

int main (int argc, char** argv)
{
  char *line, *s;
  initialize_readline ();

  for ( ; done == 0; )
  {
      line = readline ("@>: ");
      
      if (!line) {
        break;
      }
      
      s = stripwhite(line);
      
      if (*s)
      {
            add_history(s);
            parsing(s);
      }
      
      free(line);
      
    }
  
  return 0;

}

