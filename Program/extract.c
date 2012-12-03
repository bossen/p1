#include <stdio.h>
#include <string.h>

void fetchBody(char *filename);
int main(int argc, char **argv)
{
  int i;
  for(i = 1; i < argc; i++)
    fetchBody(argv[i]);
}

void fetchBody(char *filename)
{
  int i = 0;
  char buffer[2000], ch;
  char *bodyStart, *bodyEnd, *pos;
  FILE *data = fopen(filename, "r");
  while(!feof(data)) {
    fgets(buffer, 2000, data);
    if(strstr(buffer, "body=") != NULL) {
    bodyStart = strstr(buffer, "body=") + 6;
    bodyEnd = strstr(bodyStart, "\"");
    bodyEnd[0] = '\0';
    // Udskifter specialtegn
    while (strstr(bodyStart, "&#")) {
     pos = strstr(bodyStart, "&#");
      sscanf(pos, "&#%d", (int*)&ch);
     pos[0] = ch;
     strcpy(pos + 1, pos + 4);
    }
    // Udskriver body på standard output
    printf("%s", bodyStart);
    i++;
    }
  }
  fclose(data);
}