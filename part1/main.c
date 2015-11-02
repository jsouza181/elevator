#include <stdio.h>
#include <stdlib.h>

int main()
{
  FILE *fp = fopen("out.txt", "w+");
  if (fp != NULL) {
    fprintf(fp, "Testing file output.\n");
    fclose(fp);
  }
  wait(1);
  wait(1);
  return 0;
}
