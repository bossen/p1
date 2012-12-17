/*
    STRUCTURES SECTION
*/

typedef struct _uchar {
    char value;
    char extension;
    char extension2;
} uchar;

typedef struct _huffnode {
	char symbol;
	int freq;
    int connectionPoint;
	struct _huffnode *left, *right, *parent;
} HuffNode;

typedef struct _bitstream {
    FILE *file;
    int position;
    char buffer;
} BitStream;

/*
    FUNCTIONS SECTION
*/

// UTF-8 Specific functions
int isByteOrderMarked(FILE *file);
void skipByteOrderMark(FILE *file);

// General library extensions
char *flipString(const char *string);

// General file properties
unsigned long int filesize(char *filename);

// Unicode Read/Write
uchar *fReadUnicode(char *filename);
int fWriteUnicode(char *filename, uchar *string, int bom);

// Unicode properties
unsigned long int fUnicodeChars(FILE *file);
unsigned long int ustrlen(uchar *string);

// Unicode / GSM convert
char ucharToGSM(uchar ch);
uchar gsmToUchar(char ch);
char *fReadUnicodeAsGSM(char *filename);
void fWriteGSMAsUnicode(char *filename, char *data);

// Huffman specific functions
HuffNode *generateTree(int count, char *letters, int *frequencies);
void calculatePaths(HuffNode *rootNode, char *bitstring, FILE *exportTo);
void outputTree(HuffNode *tree, char *filename);
int nodeCompare(const void *n1, const void *n2);
int isLeaf(HuffNode *node);
int isRoot(HuffNode *node);
HuffNode **calculateEntryPoints(HuffNode *tree);
char *getEntryString(HuffNode **points, int point);
HuffNode *treeFromFile(const char *filename);
void huffmanCompress(char *filename, char *message, HuffNode *tree);
char *huffmanDecompress(HuffNode *tree, char *inputfile);
BitStream *establishBitStream(char *filename, char *mode);
void closeBitStream(BitStream *stream);
int readBit(BitStream *stream);
char *getPaddingValue(HuffNode *tree, int minLength);

/*
    UNICODE FUNCTIONS SECTION
*/

// Function that gets the filesize of given file in bytes
unsigned long int filesize(char *filename)
{
    // Variable declarations
    unsigned long int size = 0;
    char ch;
    FILE *file = fopen(filename, "r");
    
    // Catch exception
    if(file == NULL) {
        // If file is non-exisitent
        printf("Called filesize for non-existent file");
        return 0;
    }
    
    // Count bytes in file
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    // Close File pointer
    fclose(file);
    
    return size;
}

// Function that skips a UTF-8 byte order mark
void skipByteOrderMark(FILE *file)
{
    // Move pointer to back of BOM
    fseek(file, 3, SEEK_SET);
}

// Function that determines whether a file is UTF8 byte order marked
int isByteOrderMarked(FILE *file)
{
    unsigned char bom[3];
    // Get current position of file handle
    unsigned long int position = ftell(file);
    int i;
    // Go to file beginning
    rewind(file);
    // Read first 3 bytes
    for(i = 0; i < 3; i++)
        bom[i] = fgetc(file);
    // Reset file position
    fseek (file, position, SEEK_SET);    
    // Check if first bytes were byte order mark
    if(bom[0] == 239 && bom[1] == 187 && bom[2] == 191)
        return 1;
    else
        return 0;
}

// Function that counts characters in a unicode file
unsigned long int fUnicodeChars(FILE *file)
{
    unsigned long int result = 0;
    char ch;
    // Get current position of file handle
    unsigned long int position = ftell(file);
    // Rewind file handle
    rewind(file);
    // Skips byte order mark if it exists
    if(isByteOrderMarked(file))
        skipByteOrderMark(file);
    // Count
    while(!feof(file)) {
        ch = fgetc(file);
        
        if(ch >= 0)
            result++;
        else if(ch  == -30) {
            result++;
            fgetc(file);
            fgetc(file);
        } else {
            fgetc(file);
            result++;
        }
    }
    result--;
    // Reset file position to original
    fseek (file , position, SEEK_SET);    
    return result;
}

// Function that determines a Unicode string's length
unsigned long int ustrlen(uchar *string)
{
    int i;
    // Seek for terminator
    for(i = 0; string[i].value != 0; i++);
    return i;
}

// Function that reads a unicode text file
uchar *fReadUnicode(char *filename)
{
    // Establish file handle
    FILE *file = fopen(filename, "r");
    // Variable declarations
    unsigned long int length = fUnicodeChars(file), i;
    uchar *contents = (uchar *)malloc(sizeof(uchar) * length + 1);
    // Exception handling
    if(contents == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    char ch;
    // Jump to start of file
    rewind(file);
    // Check (and skip) BOM
    if(isByteOrderMarked(file))
        skipByteOrderMark(file);
    
    // Read file to end
    for(i = 0; i < length; i++){
        ch = fgetc(file);
        // Parse bytes to unicode char
        
        if(ch == -30) {
            contents[i].value = ch;
            ch = fgetc(file);
            contents[i].extension = ch;
            ch = fgetc(file);
            contents[i].extension2 = ch;
        } else if(ch < 0 && ch != -30) {
            contents[i].value = ch;
            ch = fgetc(file);
            contents[i].extension = ch;
        } else 
            contents[i].value = ch;
    }
    // Terminate string
    contents[length].value = 0;
    // Close file handle
    fclose(file);
    
    return contents;
}

// Function that writes a Unicode character array to file
int fWriteUnicode(char *filename, uchar *string, int bom)
{
    // Establish file handle
    FILE *file = fopen(filename, "w");
    // Exception handling
    if(file == NULL) {
        printf("Unable to establish handle to file. Writing omitted");
        return 0;
    }
    
    // Writes byte order mark if requested
    if(bom) {
        char bom[3] = {239, 187, 191};
        fwrite(bom, sizeof(char), 3, file);
    }
    
    // Determine the amount of chars to write
    unsigned long int length = ustrlen(string), i;
    // Buffer for writing
    char current[3];
    
    // Write string to file
    for(i = 0; i < length; i++){
        if(string[i].value >= 0) {
            current[0] = string[i].value;
            fwrite(current, sizeof(char), 1, file);
        } else if(string[i].value == -30) {
            current[0] = string[i].value;
            current[1] = string[i].extension;
            current[2] = string[i].extension2;
            fwrite(current, sizeof(char), 3, file);
        } else {
            current[0] = string[i].value;
            current[1] = string[i].extension;
            fwrite(current, sizeof(char), 2, file);
        }
    }
    // Close file handle
    fclose(file);
    return 1;
}

/*
    CONVERTION FUNCTIONS SECTION
*/

// Function that converts a GSM 7-bit character to a Unicode character
uchar gsmToUchar(char ch)
{
    uchar result;
        // Remapping
        switch (ch)
        {
            case -64:
                result.value = 124;
                break;
            case -101:
                result.value = -30;
                result.extension = -126;
                result.extension2 = -84;
                break;
            case -60:
                result.value = 91;
                break;
            case -61:
                result.value = 126;
                break;
            case -62:
                result.value = 93;
                break;
            case -47:
                result.value = 92;
                break;
            case -40:
                result.value = 123;
                break;
            case -41:
                result.value = 125;
                break;
            case -20:
                result.value = 94;
                break;
            case 0:
                result.value = 64;
                break;
            case 2:
                result.value = 36;
                break;
            case 17:
                result.value = 95;
                break;
            case 16:
                result.value = -50;
                result.extension = -108;
                break;
            case 18:
                result.value = -50;
                result.extension = -90;
                break;
            case 19:
                result.value = -50;
                result.extension = -109;
                break;
            case 20:
                result.value = -50;
                result.extension = -101;
                break;
            case 21:
                result.value = -50;
                result.extension = -87;
                break;
            case 22:
                result.value = -50;
                result.extension = -96;
                break;
            case 23:
                result.value = -50;
                result.extension = -88;
                break;
            case 24:
                result.value = -50;
                result.extension = -93;
                break;
            case 25:
                result.value = -50;
                result.extension = -104;
            case 26:
                result.value = -50;
                result.extension = -98;
                break;
            case 1:
                result.value = -62;
                result.extension = -93;
                break;
            case 3:
                result.value = -62;
                result.extension = -91;
                break;
            case 36:
                result.value = -62;
                result.extension = -92;
                break;
            case 64:
                result.value = -62;
                result.extension = -95;
                break;
            case 95:
                result.value = -62;
                result.extension = -89;
                break;
            case 96:
                result.value = -62;
                result.extension = -65;
                break;
            case 4:
                result.value = -61;
                result.extension = -88;
                break;
            case 5:
                result.value = -61;
                result.extension = -87;
                break;
            case 6:
                result.value = -61;
                result.extension = -71;
                break;
            case 7:
                result.value = -61;
                result.extension = -84;
                break;
            case 8:
                result.value = -61;
                result.extension = -78;
                break;
            case 9:
                result.value = -61;
                result.extension = -121;
                break;
            case 11:
                result.value = -61;
                result.extension = -104;
                break;
            case 12:
                result.value = -61;
                result.extension = -72;
                break;
            case 14:
                result.value = -61;
                result.extension = -123;
                break;
            case 15:
                result.value = -61;
                result.extension = -91;
                break;
            case 28:
                result.value = -61;
                result.extension = -122;
                break;
            case 29:
                result.value = -61;
                result.extension = -90;
                break;
            case 30:
                result.value = -61;
                result.extension = -97;
                break;
            case 31:
                result.value = -61;
                result.extension = -119;
                break;
            case 91:
                result.value = -61;
                result.extension = -124;
                break;
            case 92:
                result.value = -61;
                result.extension = -106;
                break;
            case 93:
                result.value = -61;
                result.extension = -111;
                break;
            case 94:
                result.value = -61;
                result.extension = -100;
                break;
            case 123:
                result.value = -61;
                result.extension = -92;
                break;
            case 124:
                result.value = -61;
                result.extension = -74;
                break;
            case 125:
                result.value = -61;
                result.extension = -79;
                break;
            case 126:
                result.value = -61;
                result.extension = -68;
                break;
            case 127:
                result.value = -61;
                result.extension = -96;
                break;
            default:
                result.value = ch;
                break;
        }
        return result;
}

// Function that converts a Unicode character to a GSM 7-bit character
char ucharToGSM(uchar ch)
{
    // Remapping
    
    // Handle Euro-sign
    if((ch.value == -30) && (ch.extension == -126) && (ch.extension2 == -84))
        return -101;
    if(ch.value >= 0)
    {
        switch (ch.value)
        {
            case 91:
                return -60;
            case 126:
                return -61;
            case 93:
                return -62;
            case 92:
                return -47;
            case 94:
                return -20;
            case 124:
                return -64;
            case 123:
                return -40;
            case 125:
                return -41;
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
                    default:
                        printf("Read non GSM symbol, skipping\n");
                        return 32;
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
                    default:
                        printf("Read non GSM symbol, skipping\n");
                        return 32;
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
                default:
                    printf("%d Read non GSM symbol, skipping\n", ch.value);
                    return 32;
        }
    }
}

// Function that reads a Unicode file into GSM 7-bit representation
char *fReadUnicodeAsGSM(char *filename)
{
    // Read file in unicode
    uchar *string = fReadUnicode(filename); 
    int length = ustrlen(string), i;
    char *gsmstring = (char *)malloc(sizeof(char) * length + 1);
    // Exception handling
    if(gsmstring == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    // Convert to GSM 7-bit
    for(i = 0; i < length; i++)
        gsmstring[i] = ucharToGSM(string[i]);
    // Terminate string with GSM 7-bit Carriage return
    gsmstring[length] = -127;
    return gsmstring;
}

// Function that writes a GSM 7-bit string as a Unicode file
void fWriteGSMAsUnicode(char *filename, char *data)
{
    int length = gsmstrlen(data), i;
    uchar *original = (uchar *)malloc(sizeof(uchar) * length);
    // Exception handling
    if(original == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    // Convert to Unicode
    for(i = 0; i < length; i++) {
        original[i] = gsmToUchar(data[i]);
    }
    // Terminate string
    original[i].value = 0;
    // Write to file
    fWriteUnicode(filename, original, 1);
}
/*
    GSM FUNCTIONS SECTION
*/

// Function that determines a GSM 7-bit string's length
int gsmstrlen(char *string)
{
    int i = 0;
    // Seek for terminator
    while(string[i] != -127)
        i++;
    return i;
}

/*
    HUFFMAN FUNCTIONS SECTION
*/

// Function that indexes a tree for faster reverse lookup
HuffNode **calculateEntryPoints(HuffNode *tree)
{
    // Variable declaration
    unsigned long int treesize = 0, i, j;
    HuffNode *conveyor;
    
    // Write tree codevalues into temp file
    outputTree(tree, "temp.hufftree");
    
    // Establish filehandle to tempfile
    FILE *treedata = fopen("temp.hufftree", "r");
    
    // Count number of tree leaves
    while(!feof(treedata)) {
        fscanf(treedata, "%*d : %*d ");
        treesize++;
    }
    
    // Dynamic memory allocation for reverse lookup table
    HuffNode **entryPoints = (HuffNode **)malloc(sizeof(HuffNode *) * 255 + 1);
    // Exception handling
    if(entryPoints == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    // Rewind filepointer
    rewind(treedata);
    
    // Iterate through codevalues
    for(i = 0; i < treesize; i++)
    {
        int node;
        char binVal[100];
        // Get symbol and huffman code
        fscanf(treedata, " %d:%s ", &node, binVal);
        // Move conveyor to root
        conveyor = tree;
        // Initialize j
        j = 0;
        // Check if codevalue is valid
        while(!isLeaf(conveyor)) {
            if(j > strlen(binVal)) {
                printf("Cannot calculate entry points: tree mismatch!");
                fclose(treedata);
                return NULL;
            }
            // Traverse to leaf
            if(binVal[j] == 48)
                conveyor = conveyor->right;
            else
                conveyor = conveyor->left;
            j++;
        }
        // Add index to codepoint
        entryPoints[((unsigned char)node)] = conveyor;
    }
    // Close file handle
    fclose(treedata);
    return entryPoints;
}

// Function that gets huffman code from tree index
char *getEntryString(HuffNode **points, int point)
{
    // Set conveyor to requested node
    HuffNode *conveyor = points[point];
    
    // Allocate memory for huffman code
    char *entry = (char *)malloc(sizeof(char) * 40);
    // Exception handling
    if(entry == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    entry[0] = '\0';
    
    // Travel upwards tree path to root
    while(!isRoot(conveyor)){
        strcat(entry, conveyor->connectionPoint == 0 ? "0" : "1");
        conveyor = conveyor->parent;
    }
    // Flip codevalue
    entry = flipString(entry);
    return entry;
}

char *getPaddingValue(HuffNode *tree, int minLength)
{
    outputTree(tree, "temp.hufftree");
    
    FILE *treefile = fopen("temp.hufftree", "r");
    char currbin[100] = {'\0'};
    int node;
   
    while(!feof(treefile)) {
        fscanf(treefile, " %d:%s", &node, currbin);
        if(strlen(currbin) >= minLength)
            break;
    }
    fclose(treefile);
    if(currbin[0] == '\0') {
        printf("ERROR! Could not find a branch value for padding");
        return NULL;
    }
    // Trim and malloc value to return
    char *result = (char *)malloc(sizeof(char) * strlen(currbin) + 1);
    // Exception handling
    if(result == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    strcpy(result, currbin);
    return result;
}

// Function that compresses a GSM 7-bit string using indexed Huffman tree
void huffmanCompress(char *filename, char *message, HuffNode *tree)
{
    int length = gsmstrlen(message), i, j, position = 0, byteIndex = 0,
        estSize = length * 1.5; // Estimates the maximum final message size
    char buffer, result[estSize];
    // Index tree
    HuffNode **entries = calculateEntryPoints(tree);
    // Fetch a valid padding value from tree
    char *padvalue = getPaddingValue(tree, 8);
    // Iterate through message
    for(i = 0; i < length; i++) {
        // Fetch current symbols codevalue
        char *binval = getEntryString(entries, (unsigned char)message[i]);
        for(j = 0; j < strlen(binval); j++) {
            // Write codevalue to buffer
            byteIndex++;
            buffer = buffer << 1;
            buffer += binval[j] == 48 ? 0 : 1;
            if(byteIndex == 8){
                // Write buffer to array
                result[position] = buffer;
                position++;
                byteIndex = 0;
            }
        }
    }
    // Pad buffer towards branch until full
    i = 0;
    while(byteIndex > 0) {
        buffer = buffer << 1;
        buffer += (padvalue[i] == 48 ? 0 : 1);
        byteIndex++;
        i++;
        if(byteIndex == 8)
            break;
    }
    // Add final buffer to array
    result[position] = buffer;
    position++;
    
    // Write compressed binary to file
    FILE *file = fopen(filename, "wb");
    fwrite(result, sizeof(char), position, file);
    fclose(file);

}

// Function that decompresses a binary file into a GSM 7-bit string
char *huffmanDecompress(HuffNode *tree, char *inputfile)
{
    HuffNode *conveyor = tree;
    int read, size = 0, position = 0, 
        // Estimate maximum filesize
        estSize = filesize(inputfile) * 4;
    char result[estSize];
    
    // Establish a bitstream to file
    BitStream *stream = establishBitStream(inputfile, "rb");
    // Read file bitwise and iterate through tree
    while((read = readBit(stream)) != EOF){
        if(read == 0)
            conveyor = conveyor->right;
        else
            conveyor = conveyor->left;
        if(isLeaf(conveyor)) {
            // If node is leaf, add to output stream
            result[size] = conveyor->symbol;
            size++;
            conveyor = tree;
        }
    }
    // Close bitstream
    closeBitStream(stream);
    // Trim memory to exact size
    char *final = (char *)malloc(sizeof(char) * size + 1);
    // Exception handling
    if(final == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    for(position = 0; position < size; position++)
        final[position] = result[position];
    final[position] = -127;
    
    return final;
}

// Function that calculates all possible paths in Huffman tree
void calculatePaths(HuffNode *rootNode, char *bitstring, FILE *exportTo)
{
    char lbit[40];
    char rbit[40];
    // Combine current path
    strcpy(lbit, bitstring);
    strcpy(rbit, bitstring);
    
    // Recursively traverse tree and output node path
    if(isLeaf(rootNode)) {
        fprintf(exportTo, "%d:%s\n", rootNode->symbol,bitstring);
    } else {
            strcat(rbit, "0");
            calculatePaths(rootNode->right, rbit, exportTo);
            strcat(lbit, "1");
            calculatePaths(rootNode->left, lbit, exportTo);
    }
}

// Function that writes a Huffman tree to a file
void outputTree(HuffNode *tree, char *filename)
{
    char bitstring[100] = {'\0'};
    // Get file handle for writing
    FILE *file = fopen(filename, "w");
    // Output all possible paths to file
    calculatePaths(tree, bitstring , file);
    // Close file handle
    fclose(file);
}

// Function that generates a Huffman tree
HuffNode *generateTree(int count, char *letters, int *frequencies)
{
    HuffNode *rootNode,*priorityQueue[count];
    int i, cnt = count;
    
    // Initialize priority queue
	for (i = 0; i < count;i++)
	{
		priorityQueue[i] = (HuffNode *)malloc(sizeof(HuffNode));
        // Exception handling
        if(priorityQueue[i] == NULL) {
            printf("Out of Memory! - Quitting");
            exit(EXIT_FAILURE);
        }
		priorityQueue[i]->freq = frequencies[i];
		priorityQueue[i]->symbol = letters[i];
		priorityQueue[i]->left = NULL;
		priorityQueue[i]->right = NULL;
	}
    
    // Sort priority queue by frequency (descending)
	qsort(priorityQueue,count,sizeof(HuffNode *),nodeCompare);
    cnt--;
    
    // Merge nodes until only a single branch is left in priority queue
	while(cnt > 0)
	{
		rootNode = (HuffNode *)malloc(sizeof(HuffNode));
        // Exception handling
        if(rootNode == NULL) {
            printf("Out of Memory! - Quitting");
            exit(EXIT_FAILURE);
        }
		rootNode->right = *(priorityQueue + cnt);
        rootNode->right->connectionPoint = 0;
		rootNode->left = *(priorityQueue + cnt - 1);
        rootNode->left->connectionPoint = 1;
		rootNode->freq = (rootNode->left->freq + rootNode->right->freq);
        rootNode->left->parent = rootNode;
        rootNode->right->parent = rootNode;
		cnt--;
        // Replace old nodes with new branch
		priorityQueue[(cnt)] = rootNode;
        // Resort priority queue
		qsort(priorityQueue,(cnt + 1),sizeof(HuffNode *),nodeCompare);
	}
    // Remove parent from root node
    rootNode->parent = NULL;
    return rootNode;
}

// Function that compares two tree nodes by frequency
int nodeCompare(const void *n1, const void *n2)
{
	return ((HuffNode **)n2)[0]->freq - ((HuffNode **)n1)[0]->freq;
}

// Function that determines whether a node is a leaf
int isLeaf(HuffNode *node)
{
	if (node->left == NULL && node->right == NULL)
		return 1;
	else
		return 0;
}

// Function that determines if a node is tree root
int isRoot(HuffNode *node)
{
    return (node->parent == NULL ? 1 : 0);
}

// Function that generates a Huffman tree from frequency file
HuffNode *treeFromFile(const char *filename)
{
    int lines = 0, i;
    char buffer[200];
    int currents, currentc;
    // Get file handle to frequency file
    FILE *tree = fopen(filename, "r");
    // Exception handling
    if(tree == NULL) {
        printf(" Tree file not found!");
        return 0;
    }
    // Count number of entries in frequency file
    while(!feof(tree))
    {
        fgets(buffer, 200, tree);
        lines++;
    }
    // Rewind file pointer
    rewind(tree);
    
    // Memory allocation
    char *symbols = (char *)malloc(sizeof(char) * lines);
    int *frequencies = (int *)malloc(sizeof(int) * lines);
    // Exception handling
    if(symbols == NULL || frequencies == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    
    // Read frequencies and symbols from file
    for(i = 0; i < lines; i++)
    {   
        fscanf(tree, " %[^\n]s ", buffer);
        sscanf(buffer, " %d:%d ", &currents, &currentc);
        symbols[i] = currents;
        frequencies[i] = currentc;
    }

    // Generate and return tree
    return generateTree(lines, symbols, frequencies);
}

// Function that closes a bitstream
void closeBitStream(BitStream *stream)
{
    fclose(stream->file);
    free(stream);
}

// Function that establishes a bitstream
BitStream *establishBitStream(char *filename, char *mode)
{
    BitStream *stream = (BitStream *)malloc(sizeof(BitStream));
    // Exception handling
    if(stream == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    stream->file = fopen(filename, mode);
    if(mode[0] == 'r')
        stream->position = 8;
    else if(mode[0] == 'w')
        stream->position = 0;
    return stream;
}

// Function that reads a bit from bitstream
int readBit(BitStream *stream) 
{
    int read = 0;
    // If buffer is empty
    if(stream->position == 8)
    {
        read = fgetc(stream->file);
        stream->buffer = read;
        stream->position = 0;
    }
    // If file is at end
    if(read == EOF) {
        stream->position = -1;
        return -1;
    }   
    
    // Return current binary value
    int res;
    res = ((stream->buffer) >> (7 - (stream->position))) & 1;
    stream->position++;

    return res;
}

/*
    GENERAL LIBRARY EXTENSIONS
*/

// Function that flips a string
char *flipString(const char *string)
{   
    int length = strlen(string);
    char *flipped = (char *)malloc(sizeof(char) * length + 1);
    // Exception handling
    if(flipped == NULL) {
        printf("Out of Memory! - Quitting");
        exit(EXIT_FAILURE);
    }
    int i;
    for(i = 0;i < length; i++)
    {
        flipped[i] = string[(length - 1 - i)];
    }
    flipped[length] = '\0';
    return flipped;
}
