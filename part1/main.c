#include <stdio.h>
#include <stdlib.h>

int main()
{
  FILE *fp = fopen("test.txt", "w+");

  fprintf(fp, "Testing file output.\n");

  wait(1);
  wait(1);

  fclose(fp);

  return 0;
}
