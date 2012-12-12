#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GSMFunctions.h"
#include "HuffmanFunctions.h"

int main (void){
        int i;
        
        //  Læser input fra en unicode fil ind i en unicode streng
        uchar *string = fReadUnicode("test.txt");

        // Variable til konvertering til GSM-7bit
        char *result = (char *)malloc(sizeof(char)*(ustrlen(string) + 1));
        
        // Konverter streng fra Unicode til GSM-7bit
        for(i = 0; i < ustrlen(string); i++)
        {
            result[i] = ucharToGSM(string[i]);
        }
        result[(ustrlen(string))] = 13;
        //printStringBinary(result);
        result = gsmDeflate(result);
    printf("\n");
    printf("Uncompressed:\n");
    printStringBinary(result);    
    /* 
    På dette tidspunkt er 'result' lig med den data 
    mobiltelefonen ville skrive sms'en som. 
    */
        
    // Nu har vi format som computeren forstår
    result = gsmInflate(result); 
    
    printf("\nCompressed:\n");
    huffEncode (result);
    /*
    Huffman Komprimering og dekomprimering ville sidenhen foregå her
    Al kode ovenover her, vil blive kørt FØR komprimering, mens al
    kode nedenunder her vil blive kørt EFTER dekomprimering.
    */
    
    // Tilbage på format som mobilen forstår
    result = gsmDeflate(result); 
        
        
        // Genskab unicode sekvens ud fra GSM-7bit
        result = gsmInflate(result);
        
        int length = strlen(result);
        uchar original[(length + 1)];
        for(i = 0; i < length; i++)
        {
            original[i] = gsmToUchar(result[i]);
        }
        // Terminer unicode streng
        original[length] = unicodeNullTerm();
       
        // Skriv den genskabte streng ind i out.txt
        fWriteUnicode(original, "out.txt");
  return 0;
}
