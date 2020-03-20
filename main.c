#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc != 11){ // Check if enough command line arguments 
        fprintf(stderr, "USAGE: -f <trace file name> -s <cache size in KB>"
                        "-b <block size> -a <associativity> "
                        "-r <replacement policy\n");
        exit(1);
    }
    
    FILE *pFile = fopen(argv[2], "r+"); // Opens file to read
    int iCacheSize = atoi(argv[4]), iBlockSize = atoi(argv[6]);
    int iAssociativity = atoi(argv[8]);
    char *szReplacementPolicy = argv[10];
    
    if(pFile == NULL){ // Check if trace file exists
        fprintf(stderr, "Trace File %s does not exist.\n", argv[2]);
        exit(1);
    }
    
    PrintHeader(argv[2], iCacheSize, iBlockSize
            , iAssociativity, szReplacementPolicy);
    
    return 0;
}

void PrintHeader(char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy){
    printf("Cache Simulator CS 3853 Spring 2020 - Group #11\n\n");
    printf("Cmd Line: tcsh\n"); // TODO: get shell
    printf("Trace File: %s\nCache Size: %d KB\nBlock Size: %d bytes\n"
            , szFileName, iCacheSize, iBlockSize);
    printf("Associativity: %d\nPolicy: %s\n"
            , iAssociativity, szReplacementPolicy);
}
