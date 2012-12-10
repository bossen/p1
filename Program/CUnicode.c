#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _uBOM {
    unsigned char value[3];
} ByteOrderMark;

typedef struct _utf8char {
    unsigned char value;
    unsigned char extension;
} utf8Char;

ByteOrderMark getByteOrderMark(FILE *file);
void printUtf8Char(utf8Char);
void setFileEncoding(FILE *file, char *encoding);
utf8Char *readUnicodeFile(FILE *file);
int utf8StrLen(utf8Char *string);
void writeUnicodeFile(utf8Char *string, char *filename, char *encoding);

int main(void)
{
    FILE *utf8file = fopen("test.txt", "r");
    ByteOrderMark bm;
    bm = getByteOrderMark(utf8file);
    utf8Char *result = readUnicodeFile(utf8file);
    int i;
    for(i = 0; i < utf8StrLen(result); i++)
        printUtf8Char(result[i]);
    fclose(utf8file);
    writeUnicodeFile(result, "res.txt", "utf8");
    printf("\nCopied!");
}

void setFileEncoding(FILE *file, char *encoding)
{
	unsigned char data[10];
	// Skriver UTF8 Byte Order Mark binært
	if(!strcmp(encoding, "utf8") || 
	   !strcmp(encoding, "utf-8") || 
	   !strcmp(encoding, "UTF8") || 
	   !strcmp(encoding, "UTF-8"))
	{
		data[0] = 239;
		data[1] = 187;
		data[2] = 191;
		fwrite(data,sizeof(char),3,file);
	}
	// Skriver UTF16 small endian Byte Order Mark binært
	if(!strcmp(encoding, "utf16") || 
	   !strcmp(encoding, "utf-16") || 
	   !strcmp(encoding, "UTF16") || 
	   !strcmp(encoding, "UTF-16"))
	{
		data[0] = 255;
		data[1] = 254;
		fwrite(data,sizeof(char),2,file);
	}
	// Skriver UTF16 big endian Byte Order Mark binært
	if(!strcmp(encoding, "utf16BE") || 
	   !strcmp(encoding, "utf-16BE") || 
	   !strcmp(encoding, "UTF16BE") || 
	   !strcmp(encoding, "UTF-16BE"))
	{
		data[0] = 254;
		data[1] = 255;
		fwrite(data,sizeof(char),2,file);
	}
}
void writeUnicodeFile(utf8Char *string, char *filename, char *encoding)
{
    FILE *resfile = fopen(filename, "w");
    int datalength = utf8StrLen(string), i;
	unsigned char *data = (unsigned char *) malloc(sizeof(unsigned char)*datalength);
	setFileEncoding(resfile, "utf8");

    for ( i= 0; i < (utf8StrLen(string)); i++) 
    {
        fputc(string[i].value, resfile);
		if(string[i].value > 127)
			fputc(string[i].extension, resfile);
     }
    fclose(resfile);
}

ByteOrderMark getByteOrderMark(FILE *file)
{
    ByteOrderMark bom;
    rewind(file);
    int i;
    for (i = 0; i < 3; i++)
        bom.value[i] = fgetc(file);
    return bom;
}

void printUtf8Char(utf8Char ch)
{
	if(ch.value > 127)
		printf("%c%c", ch.value, ch.extension);
	else
		printf("%c", ch.value);
}

int utf8StrLen(utf8Char *string)
{
	int i = 0;
	while(string[i].value != 194 && string[i].extension != 156)
		i++;
	return i;
}

utf8Char *readUnicodeFile(FILE *file)
{
    int charCount = 0, i;
    char ch;
	rewind(file);
    getByteOrderMark(file);
    while ((ch = fgetc(file)) != EOF)
        charCount++;
	utf8Char *content = (utf8Char *)malloc(sizeof(utf8Char)*(charCount + 1));
    rewind(file);
    getByteOrderMark(file);
    for(i = 0; i < charCount; i++)
    {
        ch = fgetc(file);
        content[i].value = ch;
		if(ch < 0)
		{
			ch = fgetc(file);
			content[i].extension = ch;
		}
    }
    content[charCount].value = 194;
    content[charCount].extension =  156;
    return content;
}





