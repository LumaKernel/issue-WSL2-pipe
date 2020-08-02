#include <stdio.h>
#include <ctype.h>

int main() {

  char line[10000];
  int i;

  while(fgets(line, sizeof(line), stdin) != NULL) {
    for (i = 0; line[i]; i++) {
      printf("%02x", line[i]);
      if (i % 2 == 0) printf(" ");
    }
    printf("    |");
    for (i = 0; line[i]; i++) {
      if (iscntrl(line[i])) {
        printf(".");
      } else {
        printf("%c", line[i]);
      }
    }
    printf("|\n");
    fflush(stdout);
  }

  printf("FOUND EOF!!\n");

  return 0;
}
