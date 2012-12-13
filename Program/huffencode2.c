#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GSMFunctions.h"
#include "HuffmanFunctions.h"
#define SENTINEL -1

int main (void){
    int i;
    char letters[200];
    int frequencies[200];
    int count = 0;
    int ch;
    int freq;
    int menuInput;
    FILE *freqfile = fopen("gsmfreq.txt", "r");
    while(!feof(freqfile)) {
        fscanf(freqfile, " %d:%d", &ch, &freq);
        letters[count] = ch;
        frequencies[count] = freq;
        count++;
    }    
    char bitstring[20] = {'\0'};
   
    //int i;
   

   /* Midlertidig løsning 
    *
    *
    *
    */ 
            //  Læser input fra en unicode fil ind i en unicode streng
            uchar *string = fReadUnicode("test.txt");
            printf("Read from test.txt\n");

            // Variable til konvertering til GSM-7bit
            char *result = (char *)malloc(sizeof(char)*(ustrlen(string) + 1));

    
	HuffNode *tree = generateTree(count, letters, frequencies);

    /* Menu display */
    do {
        printf("Vaelg komprimering (1) eller dekomprimering (2) tilstand>");
        scanf("%d", &menuInput);

    } while (menuInput < 1 || menuInput > 2 && menuInput != SENTINEL);
	
    /* Menu actions */
    switch (menuInput) {

        case 1:
           {
  
            BitStream *testout = establishBitStream("Testbinary.bin", "w");
            for ( i = 0; i < 10; i++)
                writeBit(testout, 0);
            writeBit(testout, 2);
            closeBitStream(testout);
           
           
            // Konverter streng fra Unicode til GSM-7bit
            for(i = 0; i < ustrlen(string); i++)
            {
                result[i] = ucharToGSM(string[i]);
            }
            result[(ustrlen(string))] = 13;
            result = gsmDeflate(result);
            printf("Original message is %d bytes long\n", gsmstrlen(result));

           /* 
           På dette tidspunkt er 'result' lig med den data 
           mobiltelefonen ville skrive sms'en som. 
           */
           // Nu har vi format som computeren forstår
           result = gsmInflate(result); 
           huffEncode(result);
           printf("Compressed into file testbinary.bin\n");

           break;
           }



           /*
           Huffman Komprimering og dekomprimering ville sidenhen foregå her
           Al kode ovenover her, vil blive kørt FØR komprimering, mens al
           kode nedenunder her vil blive kørt EFTER dekomprimering.
           */


        case 2:
           {


           BitStream *stream = establishBitStream("Testbinary.bin", "r");
           result = huffmanDecode(tree, stream);
           closeBitStream(stream);
           printf("Decompressed testbinary.bin\n");
           // Tilbage på format som mobilen forstår
           //result = gsmDeflate(result); 
          
              
           // Genskab unicode sekvens ud fra GSM-7bit
           //result = gsmInflate(result);
          
           int length = gsmstrlen(result) + 1;
           uchar original[(length + 1)];
           for(i = 0; i < length; i++)
           {
               original[i] = gsmToUchar(result[i]);
           }
           // Terminer unicode streng
           original[length] = unicodeNullTerm();
           // Skriv den genskabte streng ind i out.txt
           fWriteUnicode(original, "out.txt");
           printf("Original message written to out.txt\n");

           break;
           }

        case SENTINEL:
           printf("Quitting!");
    }

  return 0;
}
