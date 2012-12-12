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
void huffmanDecode(HuffNode *tree, BitStream *stream);
BitStream *establishBitStream(char *filename);
int readBit(BitStream *stream);

/*
int main(void)
{
    char letters[200];
    int frequencies[200];
    int count = 0;
    int ch;
    int freq;
    FILE *freqfile = fopen("gsmfreq.txt", "r");
    while(!feof(freqfile)) {
        fscanf(freqfile, " %d:%d", &ch, &freq);
        letters[count] = ch;
        frequencies[count] = freq;
        count++;
    }    
	//char letters[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};
	//int frequencies[] = {5,7,10,15,20,45,55,24,15,1,44,55,12,13,4,2,15,66,25,1,4,5};	
    char bitstring[20] = {'\0'};
    
    //int i;
    //BitStream *stream = establishBitStream("test.txt");
    
	HuffNode *tree = generateTree(count, letters, frequencies);
	outputTree(tree, bitstring);
    
    //printf("\nDecoding:");
    //huffmanDecode(tree, stream);
}
*/

int readBit(BitStream *stream) {
    
    if(stream->position == -1)
        return -1;
    if(stream->position == 8)
    {
        stream->buffer = fgetc(stream->file);
        stream->position = 0;
    }
    int res;
    
    res = ((stream->buffer) >> (7 - (stream->position))) & 1;
    stream->position++;
    if(feof(stream->file))
        stream->position = -1;
    
    return res;
}

BitStream *establishBitStream(char *filename) 
{
    BitStream *stream = (BitStream *)malloc(sizeof(BitStream));
    stream->file = fopen(filename, "r");
    stream->position = 8;
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

void huffmanDecode(HuffNode *tree, BitStream *stream)
{
    int i, res;
    HuffNode *conveyor = tree;

    for(i = 0; (res = readBit(stream)) != -1 ;i++)
    {
        if(res == 1) {
            conveyor = conveyor->left;
        } 
        if(res == 0){
            conveyor = conveyor->right;      
        }
        if(isLeaf(conveyor)) {
            printf("%c", conveyor->symbol);
            conveyor = tree;
        }
    }
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