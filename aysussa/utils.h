#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<strings.h>
#include<time.h>
#include<sys/types.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pwd.h>
#include <readline/readline.h>
#include <readline/history.h>

char* stripwhite (char *);

char *stripwhite (char *string)
{
  register char *s, *t;

  for (s = string; whitespace (*s); s++)
    ;
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t))
    t--;
  *++t = '\0';

  return s;
}
