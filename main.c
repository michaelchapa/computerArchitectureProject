#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

//Prototypes
void PrintHeader(char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy);
void CalculateValues (char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy);

int main(int argc, char** argv) 
{
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
    
    CalculateValues(argv[2], iCacheSize, iBlockSize
            , iAssociativity, szReplacementPolicy);
    
    return 0;
}

void PrintHeader(char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy)
{
    printf("Cache Simulator CS 3853 Spring 2020 - Group #11\n\n");
    printf("Cmd Line: tcsh\n\n"); // TODO: get shell
    printf("***** Cache Input Parameters *****\n\n");
    printf("Trace File: %s\nCache Size: %d KB\nBlock Size: %d bytes\n"
            , szFileName, iCacheSize, iBlockSize);
    printf("Associativity: %d\nPolicy: %s\n"
            , iAssociativity, szReplacementPolicy);
}

void CalculateValues (char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy)
{
<<<<<<< HEAD
    int iNumBlocks, iOffsetSz, iTagSz, iIndexSz, iNumRows, iOverheadSz;
    int iMemSz, iCost;

    iCacheSize *= 1024; // changes from KB to Bytes
=======
    iCacheSize *= 1024; // Simplifies our calculations, changes from KB to B.
    int iNumBlocks;
    int iOffsetSz;
    int iTagSz;
    int iIndexSz;
    int iNumRows;
    int iOverheadSz;
    int iMemSz;
    int iMemSzKB;
    float fCost;
        
>>>>>>> 08ca38d4f98ba82a7440f72f9bbcd3c8e55f9b21
    iNumBlocks = iCacheSize / iBlockSize;
    iOffsetSz = (int) (log10((double) iBlockSize)) / (log10((double) 2));
    iIndexSz = (int) (log10((double) (iCacheSize / ((2 ^ iOffsetSz)) * iAssociativity))) / (log10((double) 2));
    iTagSz = 32 - (iIndexSz + iOffsetSz);
    iNumRows = iNumBlocks / iAssociativity;
    iOverheadSz = (iTagSz + 1) * (2 ^ (iIndexSz - 3)) * iAssociativity;
<<<<<<< HEAD
    iMemSz = 1234; // TODO
    iCost = iMemSz * 0.05;
=======
    
    iMemSz = iOverheadSz + iCacheSize;
    
    iMemSzKB = iMemSz / 1024;
    
    fCost = iMemSzKB * 0.05;
    
>>>>>>> 08ca38d4f98ba82a7440f72f9bbcd3c8e55f9b21
    printf("***** Cache Calculated Values *****\n");
    printf("Total # Blocks: %d\nTag Size: %d bits\nIndex Size: %d bits\n"
            , iNumBlocks
            , iTagSz
            , iIndexSz);
    printf("Total # Rows: %d\n Overhead Size: %d bytes\n"
            , iNumRows
            , iOverheadSz);
    printf("Implementation Memory Size: %d KB (%d bytes) \nCost: $%f\n"
            , iMemSzKB
            , iMemSz
            , fCost);
}
