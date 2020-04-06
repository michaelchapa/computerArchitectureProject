#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//Prototypes
void PrintHeader(char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy);
void ProcessFile(FILE *pInFile);
double LogBaseTwo(int i);
void CalculateValues (char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy);

int main(int argc, char** argv) 
{
    // Check if enough command line args
    if(argc != 11)
    {
        fprintf(stderr, "USAGE: -f <trace file name> -s <cache size in KB>"
                        "-b <block size> -a <associativity> "
                        "-r <replacement policy\n");
        exit(1);
    }
    
    FILE *pFile = fopen(argv[2], "r+"); // Opens file to read
    int iCacheSize = atoi(argv[4]), iBlockSize = atoi(argv[6]);
    int iAssociativity = atoi(argv[8]);
    char *szReplacementPolicy = argv[10];
    
    // Check if valid trace file
    if(pFile == NULL)
    {
        fprintf(stderr, "Trace File %s does not exist.\n", argv[2]);
        exit(1);
    }
    
    if (strcmp(szReplacementPolicy, "RR") == 0)
        szReplacementPolicy = "Round Robin";
    else if (strcmp(szReplacementPolicy, "RND") == 0)
        szReplacementPolicy = "Random";
    else if (strcmp(szReplacementPolicy, "LRU") == 0)
        szReplacementPolicy = "Least Recently Used";
    // TODO: Add else for <no match>

    ProcessFile(pFile);
    PrintHeader(argv[2], iCacheSize, iBlockSize
            , iAssociativity, szReplacementPolicy);
    CalculateValues(argv[2], iCacheSize, iBlockSize
            , iAssociativity, szReplacementPolicy);
    
    return 0;
}

void ProcessFile(FILE *pInFile)
{
    char szInputBuffer[241], szCommand[11], szRest[230];
    char szAddress[11], szLength[11];

    while(fgets(szInputBuffer, 240, pInFile) != NULL)
    {
        // skip line feeds
        if(szInputBuffer[0] == '\n')
            continue;

        // get command and rest of line
        sscanf(szInputBuffer, "30s %[^\n]", szCommand, szRest);
        
        if(strcmp(szCommand, "EIP") == 0)
            printf("%s %s\n", szComand, szRest);
    }

    fclose(pInFile);
}

void PrintHeader(char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy)
{
    printf("Cache Simulator CS 3853 Spring 2020 - Team 05\n\n");
    printf("Trace File: %s\n\n", szFileName);
    printf("***** Cache Input Parameters *****\n\n");
    printf("%-31s %d KB\n", "Cache Size:", iCacheSize);
    printf("%-31s %d bytes\n", "Block Size:", iBlockSize);
    printf("%-31s %d\n", "Associativity:", iAssociativity);
    printf("%-31s %s\n", "Replacement Policy:", szReplacementPolicy);
}

double LogBaseTwo (int i)
{
    return (log10((double) i))/(log10((double) 2));
}

void CalculateValues (char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy)
{
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

    iNumBlocks = iCacheSize / iBlockSize;

    iOffsetSz = (int) LogBaseTwo(iBlockSize);
    
    int a = (int) pow(2.0, (double) iOffsetSz);
    int b = a * iAssociativity;
    int c = iCacheSize / b;

    iIndexSz = (int) LogBaseTwo(c);

    iTagSz = 32 - (iIndexSz + iOffsetSz);

    iNumRows = iNumBlocks / iAssociativity;

    iOverheadSz = (iTagSz + 1) * (int) pow(2.0, (double) (iIndexSz - 3)) * iAssociativity;

    iMemSz = iOverheadSz + iCacheSize;
    
    iMemSzKB = iMemSz / 1024;
    
    fCost = iMemSzKB * 0.05;
    
    printf("\n***** Cache Calculated Values *****\n\n");
    printf("%-31s %d\n", "Total # Blocks:", iNumBlocks);
    printf("%-31s %d bits\n", "Tag Size:", iTagSz);
    printf("%-31s %d bits\n", "Index Size:", iIndexSz);
    printf("%-31s %d\n", "Total # Rows:", iNumRows);
    printf("%-31s %d bytes\n", "Overhead Size:", iOverheadSz);
    printf("%-31s %d KB (%d bytes)\n", "Implementation Memory Size:", iMemSzKB, iMemSz);
    printf("%-31s $%.2f\n\n", "Cost:", fCost);
}
