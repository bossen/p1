#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HuffmanLibrary.h"

int main(void)
{
    // Variable Declarations
    char selection;
    
    // File paths
    char *treefile = "gsmfreq.txt";
    char *messagefile = "test.txt";
    char *compressionfile = "result.bin";
    char *outputfile = "output.txt";
    
    // Read tree
    printf("Importing tree from frequency index: %s", treefile);
    HuffNode *tree = treeFromFile(treefile);
    printf(" ... [ DONE ]\n");
    
    // User input
    printf("Select mode: 1 for compression, 2 for decompression (0 terminates): ");
    do {
        selection = getchar();
    } while(selection < '0' && selection > '2');
    
    switch(selection)
    {
        case '0':
            printf("Quitting!");
            break;
        case '1':
            // Compression mode
            printf("Reading message from: %s", messagefile);
            char *message = fReadUnicodeAsGSM(messagefile);
            printf(" ... [ DONE ]\n\n");
            
            printf("Compressing message into file: %s", compressionfile);
            huffmanCompress(compressionfile, message, tree);
            printf(" ... [ DONE ]\n");
            break;
        case '2':
            // Decompression mode
            printf("Decompressing data from %s", compressionfile);
            char *result = huffmanDecompress(tree, compressionfile);
            printf(" ... [ DONE ]\n\n");
            
            printf("Writing original message to file: %s", outputfile);
            fWriteGSMAsUnicode(outputfile, result);
            printf(" ... [ DONE ]\n");
            break;
    }
}
