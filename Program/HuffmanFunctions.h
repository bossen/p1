#define MAXBYTES 35
#define LINES 128

typedef struct _huffnode {
	char symbol;
	int freq;
	struct _huffnode *left, *right;
} HuffNode;

typedef struct _bitstream {
    FILE *file;
    int position;
    char buffer;
} BitStream;


int nodeCompare(const void *n1, const void *n2);
int isLeaf(HuffNode *node);
void outputTree(HuffNode *rootNode, char *bitstring);
HuffNode *generateTree(int count, char *letters, int *frequencies);
char *huffmanDecode(HuffNode *tree, BitStream *stream);
BitStream *establishBitStream(char *filename, char *mode);
void closeBitStream(BitStream *stream);
int readBit(BitStream *stream);
int writeBit(BitStream *stream, int bit);

int readBit(BitStream *stream) {
    int read = 0;
    if(stream->position == 8)
    {
        read = fgetc(stream->file);
        stream->buffer = read;
        stream->position = 0;
    }
    if(read == EOF)
        stream->position = -1;
    if(stream->position == -1)
        return -1;
    
    int res;
    res = ((stream->buffer) >> (7 - (stream->position))) & 1;
    stream->position++;

    return res;
}

int writeBit(BitStream *stream, int bit) 
{
    // Flush buffer into stream
    if(bit == 2) {
        int i = 0;
        while(stream->position <= 7)
        {
            stream->buffer = (stream->buffer << 1);
            if(i < 1 || i > 2)
                setBit(&(stream->buffer), 0);
            else
                clearBit(&(stream->buffer), 0);
            
            stream->position++;
            i++;
        }
        fwrite(&(stream->buffer), sizeof(char), 1, stream->file);
        return 0;
    }
    

    stream->buffer = (stream-> buffer << 1);
    bit == 0 ? clearBit(&(stream->buffer), 0) : setBit(&(stream->buffer), 0);
    stream->position++;
    if(stream->position == 8)
    {
        fwrite(&(stream->buffer), sizeof(char), 1, stream->file);
        stream->position = 0;
    }
    return 1;
}


void closeBitStream(BitStream *stream)
{
    fclose(stream->file);
    free(stream);
}

BitStream *establishBitStream(char *filename, char *mode)
{
    BitStream *stream = (BitStream *)malloc(sizeof(BitStream));
    stream->file = fopen(filename, mode);
    if(mode[0] == 'r')
        stream->position = 8;
    else if(mode[0] == 'w')
        stream->position = 0;
    return stream;
}

HuffNode *generateTree(int count, char *letters, int *frequencies)
{
    HuffNode *rootNode,*priorityQueue[count];
    int i, cnt = count;
	for (i = 0; i < count;i++)
	{
		priorityQueue[i] = (HuffNode *)malloc(sizeof(HuffNode));
		priorityQueue[i]->freq = frequencies[i];
		priorityQueue[i]->symbol = letters[i];
		priorityQueue[i]->left = NULL;
		priorityQueue[i]->right = NULL;
	}
	qsort(priorityQueue,count,sizeof(HuffNode *),nodeCompare);

	printf("\n");
    cnt--;
    
	while(cnt > 0)
	{
		rootNode = (HuffNode *)malloc(sizeof(HuffNode));
		rootNode->symbol = '#';
		rootNode->right = *(priorityQueue + cnt);
		rootNode->left = *(priorityQueue + cnt - 1);
		rootNode->freq = (rootNode->left->freq + rootNode->right->freq);
		cnt--;
		priorityQueue[(cnt)] = rootNode;
		qsort(priorityQueue,(cnt + 1),sizeof(HuffNode *),nodeCompare);
	}
    return rootNode;
}

int nodeCompare(const void *n1, const void *n2)
{
	return ((HuffNode **)n2)[0]->freq - ((HuffNode **)n1)[0]->freq;
}

int isLeaf(HuffNode *node)
{
	if (node->left == NULL && node->right == NULL)
		return 1;
	else
		return 0;
}

char *huffmanDecode(HuffNode *tree, BitStream *stream)
{
    int res, size = 1, ch;
    while((ch = fgetc(stream->file)) != EOF)
        size++;
    rewind(stream->file);
    char *result = malloc(sizeof(char)*size + 1);
    
    HuffNode *conveyor = tree;
    size = 0;
    while((res = readBit(stream)) != -1)
    {
        if(res == 1) {
            conveyor = conveyor->left;
        } 
        if(res == 0){
            conveyor = conveyor->right;      
        }
        if(isLeaf(conveyor)) {
            result[size] = conveyor->symbol;
            size++;
            conveyor = tree;
        }
    }
    result[size] = 13;
    return result;
}

void outputTree(HuffNode *rootNode, char *bitstring)
{
    char lbit[40];
    char rbit[40];
    strcpy(lbit, bitstring);
    strcpy(rbit, bitstring);
    if(isLeaf(rootNode)) {
        printf("%3d:%s\n", rootNode->symbol,bitstring);
    } else {
            strcat(rbit, "0");
            outputTree(rootNode->right, rbit);
            strcat(lbit, "1");
            outputTree(rootNode->left, lbit);
    }
}

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
    //printf("Buffer filled: ");
    for (i=0; i<=7; i++)
      printf("%d", buffer[i]);
    printf(" ");

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
  // Establish bitstream
  BitStream *testout = establishBitStream("Testbinary.bin", "w");
        
        
  
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
      writeBit(testout, huffBits[j] == 49 ? 1 : 0);
    }
  }
  
  //Flush buffer to to file
  writeBit(testout, 2);
  // Close bitstream
  closeBitStream(testout);
}