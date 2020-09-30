/********** test.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
  int i;

  int pid = getpid();
  printf("ESF: PROC #%d - running TEST program\n", pid);

  printf("ESF: argc = %d\n", argc);
  for (i=0; i<argc; i++)
    printf("ESF: argv[%d] = %s\n", i, argv[i]);
  printf("ESF: exit\n");
}