#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void)
{
    /*
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
