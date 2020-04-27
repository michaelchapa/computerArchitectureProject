#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Block {
    int iValid;
    unsigned int uiTag;
    int iTime;
} Block;

typedef struct PartsOfAdd {
    int itagSz;
    int iIndexSz;
    int iBlockSize;
    int iAssociativity;
    int iRepCase;
    int rrCount;
    int iHitCount;
    int iConflict;
    int iCompulsory;
    int iNumBlocks;
    int iOverheadSz;
    int iImpMemSz;
    float fCost;
    float fEIPCount;
    float fOverlaps;
} PartsOfAdd;

typedef struct Address {
    unsigned int uiOffset;
    unsigned int uiIndex;
    unsigned int uiTag;
} Address;

//Globals
int iHitCount = 0;
int iCompulsory = 0;
int iConflict = 0;

//Prototypes
void PrintHeader(char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy);
void ProcessFile(FILE *pInFile, PartsOfAdd* parts, Block** cacheM);
double LogBaseTwo(int i);
int CalculateValues (char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy, PartsOfAdd* parts);
Block** initCache();
void printCache(Block** cacheM, int iAssoc, int iMaxIndex);
void accessCache(Block** cacheM, PartsOfAdd* parts, Address address, int iOverlaps, int iTime);
unsigned int ConvertStringToInt(char* szHex);
int countOverlaps(int iBlockSize, unsigned int offset, int numBytesAccessed);
void CalculateSimResults(PartsOfAdd* parts);

int main(int argc, char** argv) {

    // Check if valid command line args
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
    Block **cacheM, tempBlock;
    int iMaxIndex;
        
    // Check if valid trace file
    if(pFile == NULL)
    {
        fprintf(stderr, "Trace File %s does not exist.\n", argv[2]);
        exit(1);
    }
    
    // check replacement algorithm
    if (strcmp(szReplacementPolicy, "RR") == 0)
        szReplacementPolicy = "Round Robin";
    else if (strcmp(szReplacementPolicy, "RND") == 0)
        szReplacementPolicy = "Random";
    else if (strcmp(szReplacementPolicy, "LRU") == 0)
        szReplacementPolicy = "Least Recently Used";
    else {
        fprintf(stderr, "Invalid Replacement policy: %s\n"
            , szReplacementPolicy);
        exit(1);
    }

    PrintHeader(argv[2], iCacheSize, iBlockSize
            , iAssociativity, szReplacementPolicy);
    
    struct PartsOfAdd parts;
    
    iMaxIndex = CalculateValues(argv[2], iCacheSize, iBlockSize
            , iAssociativity, szReplacementPolicy, &parts);
    
    cacheM = initCache(iAssociativity, iMaxIndex);
    
    ProcessFile(pFile, &parts, cacheM);
    
    CalculateSimResults(&parts);
    
    
    
    
    
    
    free(cacheM);

    
    
    //TODO: Start function, params = iBlockSize, uiAddress, iIndexSz, iTagSz
    
    
    
    return (EXIT_SUCCESS);
}

int CalculateValues (char* szFileName, int iCacheSize, int iBlockSize
                , int iAssociativity, char* szReplacementPolicy, PartsOfAdd* parts)
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
    
    parts->iIndexSz = iIndexSz;
    parts->itagSz = iTagSz;
    parts->iBlockSize = iBlockSize;
    parts->iAssociativity = iAssociativity;
    parts->rrCount = 0;
    parts->iNumBlocks = iNumBlocks;
    parts->iOverheadSz = iOverheadSz;
    parts->iImpMemSz = iMemSzKB;
    parts->fCost = fCost;
    parts->fEIPCount = 0.0;
    parts->fOverlaps = 0.0;
    
    if (strcmp(szReplacementPolicy, "Round Robin") == 0)
        parts->iRepCase = 1;
    else if (strcmp(szReplacementPolicy, "Random") == 0)
        parts->iRepCase = 2;
    else if (strcmp(szReplacementPolicy, "Least Recently Used") == 0)
        parts->iRepCase = 3;
    
    printf("\n***** Cache Calculated Values *****\n\n");
    printf("%-31s %d\n", "Total # Blocks:", iNumBlocks);
    printf("%-31s %d bits\n", "Tag Size:", iTagSz);
    printf("%-31s %d bits\n", "Index Size:", iIndexSz);
    printf("%-31s %d\n", "Total # Rows:", iNumRows);
    printf("%-31s %d bytes\n", "Overhead Size:", iOverheadSz);
    printf("%-31s %d KB (%d bytes)\n", "Implementation Memory Size:", iMemSzKB, iMemSz);
    printf("%-31s $%.2f\n\n", "Cost:", fCost);
    
    return iNumRows;
}

unsigned int ConvertStringToInt(char* szAddress) {
    const char *hexString = szAddress;
    return (unsigned int) strtol(hexString, NULL, 16);
}

double LogBaseTwo (int i)
{
    return (log10((double) i))/(log10((double) 2));
}

void constructAddress(int iBlockSize, unsigned int uiAddress
                , int iTagSz, int iIndexSz, Address* address)
{
    unsigned int mask;
    unsigned int indexMask;
    unsigned int uiOffset;
    unsigned int uiTemp;
    
    switch (iBlockSize)
    {
        case 4:
            mask = 0x00000003;
            address->uiOffset = uiAddress & mask;
            address->uiIndex = uiAddress >> 2;
            indexMask = (unsigned int) (pow(2.0, (double) iIndexSz) - 1.0);
            uiTemp = address->uiIndex;
            address->uiIndex = uiTemp & indexMask;
            address->uiTag = uiAddress >> iIndexSz;
            break;
            
        case 8:
            mask = 0x00000007;
            address->uiOffset = uiAddress & mask;
            address->uiIndex = uiAddress >> 3;
            indexMask = (unsigned int) (pow(2.0, (double) iIndexSz) - 1.0);
            uiTemp = address->uiIndex;
            address->uiIndex = uiTemp & indexMask;
            address->uiTag = uiAddress >> iIndexSz;
            break;
            
        case 16:
            mask = 0x0000000F;
            address->uiOffset = uiAddress & mask;
            address->uiIndex = uiAddress >> 4;
            indexMask = (unsigned int) (pow(2.0, (double) iIndexSz) - 1.0);
            uiTemp = address->uiIndex;
            address->uiIndex = uiTemp & indexMask;
            address->uiTag = uiAddress >> iIndexSz;
            break;
            
        case 32:
            mask = 0x0000001F;
            address->uiOffset = uiAddress & mask;
            address->uiIndex = uiAddress >> 5;
            indexMask = (unsigned int) (pow(2.0, (double) iIndexSz) - 1.0);
            uiTemp = address->uiIndex;
            address->uiIndex = uiTemp & indexMask;
            address->uiTag = uiAddress >> iIndexSz;
            break;
            
        case 64:
            mask = 0x0000003F;
            address->uiOffset = uiAddress & mask;
            address->uiIndex = uiAddress >> 6;
            indexMask = (unsigned int) (pow(2.0, (double) iIndexSz) - 1.0);
            uiTemp = address->uiIndex;
            address->uiIndex = uiTemp & indexMask;
            address->uiTag = uiAddress >> iIndexSz;
            break;
    }
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

void ProcessFile(FILE *pInFile, PartsOfAdd* parts, Block** cacheM) 
{
    char szInputBuffer[241], szCommand[11], szRest[230], szRest2[230], szRest4[230], szCommand2[11];
    char szAddress[11], szLength[11], dstM[9], srcM[9], garbage1[9], garbage2[9];
    int iAccessCount = 0;
    int iCount = 0;
    unsigned int uiAddress;
    struct Address address;
    int iOverlaps;

    while(fgets(szInputBuffer, 240, pInFile) != NULL) 
    {
        // skip line feeds
        if(szInputBuffer[0] == '\n')
            continue;

        // get command and rest of line
        sscanf(szInputBuffer, "%30s %[^\n]", szCommand, szRest);
        
        if(strcmp(szCommand, "EIP") == 0){ // Instruction line
            sscanf(szRest, "%*1s%2s%*2s %s %[^\n]", szLength, szAddress, szRest2);
//            int spaces = atoi(szLength);
//            int calc = (spaces);
//            char szRest3[calc];
//            sscanf(szRest2, "%*s %s %[^\n]",calc, szRest3, szCommand2, szRest4);
//            //char* token = strtok(szRest2, " ");
//            printf("%s\n", szCommand2);
            uiAddress = ConvertStringToInt(szAddress);
            constructAddress(parts->iBlockSize, uiAddress, parts->itagSz, parts->iIndexSz, &address);
            // call accessCache;
            parts->fEIPCount += 1.0;
            iOverlaps = countOverlaps(parts->iBlockSize, address.uiOffset, atoi(szLength));
            //printf("%s: %d\n", "Overlaps", iOverlaps);
            accessCache(cacheM, parts, address, iOverlaps, iCount);
			
        }
        else if(strcmp(szCommand, "dstM:") == 0){ // Address line
            sscanf(szRest, "%s %s %s %s %[^\n]"
                , dstM, garbage1, garbage2, srcM, szRest2);
            if (!(strcmp(dstM, "00000000") == 0))
            {
                uiAddress = ConvertStringToInt(dstM);
                constructAddress(parts->iBlockSize, uiAddress, parts->itagSz, parts->iIndexSz, &address);
                // call accessCache;
                iOverlaps = countOverlaps(parts->iBlockSize, address.uiOffset, 4);
                //printf("%s: %d\n", "Overlaps", iOverlaps);
                accessCache(cacheM, parts, address, iOverlaps, iCount);
            }
            if (!(strcmp(srcM, "00000000") == 0))
            {
                uiAddress = ConvertStringToInt(srcM);
                constructAddress(parts->iBlockSize, uiAddress, parts->itagSz, parts->iIndexSz, &address);
                // call accessCache;
                iOverlaps = countOverlaps(parts->iBlockSize, address.uiOffset, 4);
                //printf("%s: %d\n", "Overlaps", iOverlaps);
                accessCache(cacheM, parts, address, iOverlaps, iCount);
            }
        }
        iCount++;
    }
    parts->iCompulsory = iCompulsory;
    parts->iConflict = iConflict;
    parts->iHitCount = iHitCount;
    //printf("%s: %d\n%s: %d\n%s: %d\n", "iHitCount", iHitCount, "iCompulsory", iCompulsory, "iConflict", iConflict);
    //printf("%s: %d\n", "Access Count", iHitCount + iCompulsory + iConflict);
    

    //printf("%d\n", iCount); //TODO Remove this later

    fclose(pInFile);
}

Block** initCache(int iAssoc, int iMaxIndex){
    int i, j;
    Block tempBlock;
    Block **cacheM2 = (Block **) malloc(sizeof(Block) * iMaxIndex); // allocates indexes
    tempBlock.iValid = 0;
    tempBlock.uiTag = 0;

    for(i = 0; i < iMaxIndex; i++){
        cacheM2[i] = (Block *) malloc(sizeof(Block) * iAssoc); // allocates set
        for(j = 0; j < iAssoc; j++){
            *(*(cacheM2 + i) + j) = tempBlock;
        }
    }
    return cacheM2;
}

int countOverlaps(int iBlockSize, unsigned int offset, int numBytesAccessed)
{
    int overlap = 1;
    int accessLength = (int) offset + numBytesAccessed;
    // printf("%s : %u, %d\n", "Offset, numBytes", offset, numBytesAccessed);
    int leftover = accessLength % iBlockSize;

    while (accessLength != leftover)
    {
        accessLength -= iBlockSize;
        overlap += 1;
    }

    return overlap; 

}

void accessCache(Block** cacheM, PartsOfAdd* parts, Address address, int iOverlaps, int iTime)
{
    Block tempBlock;
    int iRow = (int) address.uiIndex;
    int i, j;
    int jAssoc = parts->iAssociativity; // i = indexRow, 
    for(i = iRow; i < (iOverlaps + iRow); i++)
    {
        for (j = 0; j < jAssoc; j++)
        {
            tempBlock = *(*(cacheM + iRow) + j);
            if (tempBlock.iValid == 1)
            {
                // Valid Bit is 1, begin checking Tag
                if (tempBlock.uiTag == address.uiTag)
                {
                    // Tag exists and matches, this is a hit
                    iHitCount += 1;
                    return;
                }
                else
                {
                    // Valid Bit is 1, but Tag does not match
                    // Check next block in row
                    continue;
                }
            }
            else
            {
                // Valid Bit is 0, write data
                iCompulsory += 1;
                tempBlock.uiTag = address.uiTag;
                tempBlock.iValid = 1;
                tempBlock.iTime = iTime;
                *(*(cacheM + i) + j) = tempBlock;
                return;
                //TODO: add LRU functionality
            }
        }
        int rnd, k, lruBlock;
        int lruReplace = iTime;
        iConflict += 1;
        switch (parts->iRepCase)
        {
            case 1:
                if (parts->rrCount = parts->iAssociativity)
                {
                    parts->rrCount = 1;
                    tempBlock.uiTag = address.uiTag;
                    tempBlock.iValid = 1;
                    tempBlock.iTime = iTime;
                    *(*(cacheM + i) + 0) = tempBlock;
                }
                else
                {
                    tempBlock.uiTag = address.uiTag;
                    tempBlock.iValid = 1;
                    tempBlock.iTime = iTime;
                    *(*(cacheM + i) + parts->rrCount) = tempBlock;
                    parts->rrCount += 1;
                }
                break;
            case 2:
                rnd = rand();
                int rndReplace = rnd % parts->iAssociativity;
                tempBlock.uiTag = address.uiTag;
                tempBlock.iValid = 1;
                tempBlock.iTime = iTime;
                *(*(cacheM + i) + rndReplace) = tempBlock;
                break;
            case 3:
                for (k = 0; k < parts->iAssociativity; k++)
                {
                    if (tempBlock.iTime < lruReplace)
                    {
                        lruReplace = tempBlock.iTime;
                        lruBlock = k;
                    }
                }
                tempBlock.uiTag = address.uiTag;
                tempBlock.iValid = 1;
                tempBlock.iTime = iTime;
                *(*(cacheM + i) + lruBlock) = tempBlock;
                break;
        }
    }  
}

void CalculateSimResults(PartsOfAdd* parts)
{
    int iMissCount = parts->iConflict + parts->iCompulsory;
    float fAccessCount = parts->iHitCount + iMissCount;
    float fHitRate = (float) (iHitCount / fAccessCount) * 100.0;
    float fMissRate = 100.0 - fHitRate;
    int top = parts->iNumBlocks - parts->iCompulsory;
    int bottom = parts->iBlockSize * parts->iOverheadSz;
    int temp = bottom / 1024;
    float fUnusedKB = top / temp;
    float fUnusedPercent = (fUnusedKB / (float) parts->iImpMemSz) * 100.0;
    float fWaste = (fUnusedPercent / 100.0) * parts->fCost;
    int iUnusedBlocks = (fUnusedPercent / 100.0) * parts->iNumBlocks;
    double test = ceil(parts->iBlockSize/4);
    float fCPI = ((float) parts->iHitCount + (3.0 * (float) iMissCount * (float) test)) / parts->fEIPCount;
    
    printf("***** Cache Simulation Results *****\n\n");
    printf("%-31s %.0f\n", "Total Cache Accesses:", fAccessCount); //TODO: calcTotalCacheAccesses
    printf("%-31s %d\n", "Cache Hits:", parts->iHitCount); //TODO: calcCacheHits
    printf("%-31s %d\n", "Cache Misses:", iMissCount); //TODO: calcCacheMisses
    printf("%-31s %d\n", "--- Compulsory Misses:", iCompulsory); //TODO: calcCompulseMisses
    printf("%-31s %d\n", "--- Conflict Misses:", iConflict); //TODO: calcConflictMisses
    
    printf("\n***** ***** Cache Hit & Miss Rate ***** *****\n\n");
    printf("%-31s %.4f%s\n", "Hit Rate:", fHitRate,"%"); //TODO calcHitRate
    printf("%-31s %.4f%s\n", "Miss Rate:", fMissRate, "%"); //TODO 1 - HitRate
    printf("%-31s %.2f %s\n\n","CPI:", fCPI, "Cycles/Instruction"); //TODO: calcCyclesPerInstruct
    printf("%-31s %.2f KB / %.2f KB %s %.2f%s $%.2f\n", "Unused Cache Space:", fUnusedKB, (float) parts->iImpMemSz, "=", fUnusedPercent, "% Waste:", fWaste);
    printf("%-31s %d / %d\n", "Unused Cache Blocks:", iUnusedBlocks, parts->iNumBlocks);
}