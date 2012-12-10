#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _uchar {
    char value;
    char extension;
} uchar;
// Print af binær værdi
void printStringBinary(char *string);
void printByteBinary(char *byte);
void printIntBinary(int value);

// GSM Funktioner
char *gsmDeflate(char *bytes); // Konverterer 8 bit til 7 bit
char *gsmInflate(char *bytes); // Konverterer 7 bit til 8 bit

// Bit-manipulation
int getBit(char *byte, int position);
void setBit(char *byte, int position);
void clearBit(char *byte, int position);
int toggleBit(char *byte, int position);

// Booleans
int bitIsSet(char *byte, int position);

// Unicode og GSM streng konverterings funktioner
int ustrlen(uchar *string);
void printuchar(uchar ch);
char ucharToGSM(uchar ch);
uchar *fReadUnicode(char *filename);

int main(void)
{
    int i;
    //  Læser input fra en unicode fil
    uchar *string = fReadUnicode("test.txt");
    // Skal få lavet en printfunktion til dette
    for(i = 0; i < ustrlen(string) ;i++)
    {
        // Konverterer og printer unicode tegn som GSM-7bit tegn
        // !!! Udskriver den decimale værdi, og ikke tegnets symbol.
        printf("%d\n", ucharToGSM(string[i]));
    }
    
    /* Test af inflate og deflate
    char *test = "abcdefgabcdehfg";
    printStringBinary(test);
    printf("\n\n");
    char *random = gsmDeflate(test);  
    printStringBinary(random);
    printf("\nInflated:\n");    
    char *inflated = gsmInflate(random); 
    printStringBinary(inflated);
    */
}

uchar *fReadUnicode(char *filename)
{
    FILE *file = fopen(filename, "r");
    char ch;
    int count = 0, i, j = 0;
    // Skip BOM
    fgetc(file);
    fgetc(file);
    fgetc(file);
    
    while((ch = fgetc(file)) != EOF) {
        if(ch < 0)
            j++;
        else
            count++;
    }
    count = count + (j/2);
    
    uchar *string = (uchar *)malloc(sizeof(uchar) * count + 1);
    string[count].value =  -66;
    string[count].extension =  -28;
    
    rewind(file);
    // Skip BOM
    fgetc(file);
    fgetc(file);
    fgetc(file);
        
    for(i = 0; (ch = fgetc(file)) != EOF; i++)
    {
        if(ch < 0) {
            string[i].value = ch;
            ch = fgetc(file);
            string[i].extension = ch;
        } else {
            string[i].value = ch;
        }
    }
    return string;
}

char ucharToGSM(uchar ch)
{
    if(ch.value >= 0)
    {
        switch (ch.value)
        {
            case 64:
                return 0;
            case 36:
                return 2;
            case 95:
                return 17;
            default:
                return ch.value;
        }
    }
    if(ch.value < 0)
        {
        switch (ch.value)
        {
            case -50:
                switch(ch.extension)
                {
                    case -108:
                        return 16;
                    case -90:
                        return 18;
                    case -109:
                        return 19;
                    case -101:
                        return 20;
                    case -87:
                        return 21;
                    case -96:
                        return 22;
                    case -88:
                        return 23;
                    case -93:
                        return 24;
                    case -104:
                        return 25;
                    case -98:
                        return 26;
                }
                
            case -62:
                switch(ch.extension)
                {
                    case -93:
                        return 1;
                    case -91:
                        return 3;
                    case -92:
                        return 36;
                    case -95:
                        return 64;
                    case -89:
                        return 95;
                    case -65:
                        return 96;
                }
            case -61:
                switch(ch.extension)
                {
                    case -88:
                            return 4;
                    case -87:
                            return 5;
                    case -71:
                            return 6;
                    case -84:
                            return 7;
                    case -78:
                            return 8;
                    case -121:
                            return 9;
                    case -104:
                            return 11;
                    case -72:
                            return 12;
                    case -123:
                            return 14;
                    case -91:
                            return 15;
                    case -122:
                            return 28;
                    case -90:
                            return 29;
                    case -97:
                            return 30;
                    case -119:
                            return 31;
                    case -124:
                            return 91;
                    case -106:
                            return 92;
                    case -111:
                            return 93;
                    case -100:
                            return 94;
                    
                    case -92:
                            return 123;
                    case -74:
                            return 124;
                    case -79:
                            return 125;
                    case -68:
                            return 126;
                    case -96:
                            return 127;
                }
        }
    }
}

void printuchar(uchar ch)
{
    if(ch.value < 0) {
        printf("%d%d ", ch.value, ch.extension);
    } else {
        printf("%d ", ch.value);
    }
}

int ustrlen(uchar *string)
{
    int i = 0;
    while (string[i].value !=-66 && string[i].extension != -28) 
        i++;
    return i;
}
int bitIsSet(char *byte, int position)
{
    return getBit(byte, position);
}

int getBit(char *byte, int position)
{
    return ((*byte) >> position) & 1;
}

void setBit(char *byte, int position)
{
    *byte |= 1 << position;
}

void clearBit(char *byte, int position)
{
    *byte &= ~(1 << position);
}

int toggleBit(char *byte, int position)
{
    *byte ^= 1 << position;
}

char *gsmDeflate(char *bytes)
{
    int i, temp, j;
    int length = strlen(bytes), endLength = length - (length / 8);
    char *tempBytes = (char *)malloc(sizeof(char) * (length + 1));
    for(i = 0; i < length; i++)
    {
        tempBytes[i] = bytes[i];
        tempBytes[i] = tempBytes[i] << 1;
    }
    for(j = 0; j < length; j++)
    {
        for(i = length; i > j; i--)
        {
            if(bitIsSet(tempBytes + i, 7))
                setBit(tempBytes + i - 1, 0);
            tempBytes[i] = tempBytes[i] << 1;
        }
    }
    tempBytes[endLength] = '\0';
    
    // Escape med CR
    if(!((length % 7) - (length / 8)))
    {
        setBit(tempBytes + (endLength - 1),0);
        setBit(tempBytes + (endLength - 1),2);
        setBit(tempBytes + (endLength - 1),3);
    }
    return tempBytes;
}

char *gsmInflate(char *bytes)
{
    int i, temp, j;
    int length = strlen(bytes), endLength = length + (length / 7);
    char *tempBytes = (char *)malloc(sizeof(char)*endLength + 1);
    for(i = 0; i < length; i++)
    {
        tempBytes[i] = bytes[i];
    }
    
    tempBytes[(endLength - 1)] = tempBytes[(endLength - 1)] >> 1;
    for(j = 0; j < endLength; j++)
    {
        for(i = endLength; i > j; i--)
        {
            if(bitIsSet(tempBytes + i - 1,0))
                setBit(tempBytes + i,7);
            else
                clearBit(tempBytes + i,7);
            tempBytes[(i - 1)] = tempBytes[(i - 1)] >> 1;
            clearBit(tempBytes+(i - 1),7);
        }
    }
    // Hvis strengen er escapet med CR
    if(tempBytes[(endLength - 1)] == 13)
        endLength--;
        
    tempBytes[endLength] = '\0';
    return tempBytes;
}

void printByteBinary(char *byte)
{
    int bit_index;
    for (bit_index = 7; bit_index >= 0; --bit_index)
    {
        int bit = (*byte>> bit_index) & 1;
        printf("%d", bit);
    }
}

void printIntBinary(int value)
{
    int bit_index;
    for (bit_index = 7; bit_index >= 0; --bit_index)
    {
        int bit = (value >> bit_index) & 1;
        printf("%d", bit);
    }
}

void printStringBinary(char *string)
{
    int i;
    char *temp = (char *)malloc(sizeof(char)*strlen(string) + 1);    
    strcpy(temp, string);
    for(i = 0; i < strlen(string); i++)
    {
        int bit_index;
        for (bit_index = 7; bit_index >= 0; --bit_index)
        {
            int bit = (temp[i] >> bit_index) & 1;
            printf("%d", bit);
        }
        printf(" ");
    }
    free(temp);
}
