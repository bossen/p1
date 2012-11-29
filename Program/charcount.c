#include <stdio.h>
#include <ctype.h>
#define PRINTBOUNDARY 0
#define MAXCHARS 256

typedef struct {
    unsigned char symbol;
    unsigned long int amount;
} entry;

int entryCompare(const void *e1, const void *e2)
{
    return ((entry *)e2)->amount - ((entry *)e1)->amount;
}

int main(void)
{
    // Variable
    int i, curr, charcount = MAXCHARS - PRINTBOUNDARY;
    entry array[charcount];
    // Initialisering af array
    for(i = 0; i < charcount; i++) {
        array[i].symbol = i + PRINTBOUNDARY;
        array[i].amount = 0;
    }
    do
    {
        curr = getchar();
        array[(curr - PRINTBOUNDARY)].amount++;
    } while (curr != EOF);
    qsort(array, charcount, sizeof(entry), entryCompare);
    FILE *result = fopen("occurrences.txt", "w");
    for(i = 0; i < charcount; i++)
        if(array[i].amount > 0)
            if (!isprint(array[i].symbol) && array[i].symbol != '\n' && array[i].symbol != ' ')
                fprintf(result, "Dec(%2d):%25d\n", array[i].symbol, array[i].amount);
            else if(array[i].symbol == '\n')
                fprintf(result, "%-7s:%25d\n", "Newline", array[i].amount);
            else if(array[i].symbol == ' ')
                fprintf(result, "%-7s:%25d\n", "Space", array[i].amount);
            else
                fprintf(result, "%-7c:%25d\n", array[i].symbol, array[i].amount);
}
