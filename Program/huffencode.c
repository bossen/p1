#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GSMFunctions.h"
#define MAXBYTES 35
#define LINES 128

struct chHuff {
  int character;
  char bvalue[MAXBYTES];
};
typedef struct chHuff chHuff;


void getTree (FILE *fp_tree, chHuff *huffCharacters){
  int i;
 
 for (i=0; i < LINES; i++){
   fscanf(fp_tree, " %d:%s", &huffCharacters[i].character, &huffCharacters[i].bvalue);
//   printf("Scanned: %d:%s\n", huffCharacters[i].character, huffCharacters[i].bvalue);
 }

}


void bitBuffer (int newbit){
  static int buffer[8], current=0, i;
  
  if (newbit == 2){
    //fill buffer with 0, if current > 0
    if (current != 0)
      while (current != 8)
	buffer[current++]=0;
  }
  else{
    buffer[current]=newbit;
    current++;
  }
  
  if (current == 8){
    //write buffer somewhere
    printf("Buffer filled: ");
    for (i=0; i<=7; i++)
      printf("%d", buffer[i]);
    printf("\n");

    current = 0;
  }
}



char *huffCharEncode(int data, chHuff *huffCharacters){
  int i;

  for (i=0;i<LINES+1;i++){
    if (data == huffCharacters[i].character)
      break;
  }
  if (i == LINES){
    printf("Error!\n");
    exit(EXIT_FAILURE);
  }

  return huffCharacters[i].bvalue;
}


void huffEncode (unsigned char *inputst){
  int i, j;
  //Open file for read
  FILE *fp_tree = fopen("GSM7b.tree", "r");

  //Create struct array
  chHuff *huffCharacters;
  huffCharacters = (chHuff *)malloc(sizeof(chHuff)*LINES-1);
  if (huffCharacters == NULL){
    printf("Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  //Read tree from file
  getTree (fp_tree, huffCharacters);
  fclose(fp_tree);

  //Loop to go through all characters in "inputst"
  for (i=0 ; i < gsmstrlen(inputst); i++){
    //printf("%d", inputst[i]);
    
    //Get bits for a character
    char *huffBits = huffCharEncode(inputst[i], huffCharacters);
//    printf("%d = %s\n", inputst[i], huffBits);
    
    //Send all bits, to buffer
    for (j = 0 ; j < strlen(huffBits); j++){
      //printf("%d", dataout[j] == 49 ? 1 : 0);
      bitBuffer(huffBits[j] == 49 ? 1 : 0);
    }
  }
  
  //Force bitBuffer to write halffull buffer
  bitBuffer(2);
}


int main (void){
  unsigned char gsm7str[4] = {3,5,9,13};

  huffEncode (gsm7str);
  return 0;
}
