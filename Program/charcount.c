#include <stdio.h>
#define PRINTBOUNDARY 32
#define MAXCHARS 256
int main(void)
{
    // Variable
    int i, curr, charcount = MAXCHARS - PRINTBOUNDARY;
    unsigned long int array[charcount];
    // Initialisering af array
    for(i = 0; i < charcount; i++)
        array[i] = 0;
    
    do
    {
        curr = getchar();
        array[(curr - PRINTBOUNDARY)]++;
    } while (curr != EOF);
    
    FILE *result = fopen("occurrences.txt", "w");
    for(i = 0; i < charcount; i++)
        fprintf(result, "%c : %d\n", i + PRINTBOUNDARY, array[i]);
}
