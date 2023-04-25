// Darwuind Euceda
// April 24, 2023
// CDA3103, Spring 2023

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#define BLOCKS 32

// Assignment Description
// In C-language, implement direct-mapped, 2-way, 4-way and fully associative caching
// in a cache of size 32. Apply both least-recently used and random replacement policies 
// if applicable. Compare the performance of each cache implementation in terms of number of hits, 
// number of total accesses and hit rate.

// Struct Definition(s)
typedef struct Info
{
    // This holds a cache of length 32
    unsigned int cache[BLOCKS];

    // This holds a 2D cache
    unsigned int ** twoDCache;

    // This holds the "age" of the data in the 2D cache
    // This will be used to use the LRU policy
    int dataAges[BLOCKS];
    int ** twoDDataAges;

    // Number of hits in the cache used. Will be updated after running the function.
    int hitCount;

    // Number of total accesses
    int totalAccesses;

    // Rate of hits. Will be updated after all numbers have been read in.
    double hitRate;
} Info;

// Function Prototype(s)
void updateDirectMappedCache(Info * cache, unsigned int data);
void updateTwoWayCache(Info * cache, unsigned int data);
void updateFourWayCache(Info * cache, unsigned int data);
void updateFullCache(Info * cache, unsigned int data);
Info * initializeInfo(void);
Info * initializeCache(int row, int col);
void freeAll(Info * info);

int main(void)
{
    // The policy that will be used will be as follows:
    // LRU policy will be used whenever there is a collision
    // Random replacement will be used when LRU policy cannot be used

    // Variable Declaration(s)
    // A direct mapped cache is simply an array of length 32 (32 blocks) acting as a hash table
    Info * directCache = initializeInfo();

    // A 2-way cache is simply a 2D array with 16 rows (16 blocks) and 2 columns
    Info * twoWayCache = initializeCache(16, 2);

    // A 4-way cache is simply a 2D array with 8 rows (8 blocks) and 4 columns
    Info * fourWayCache = initializeCache(8, 4);

    // A fully associative cache is simply an array of length 32
    Info * fullCache = initializeInfo();

    FILE * input = fopen("traces.txt", "r");
    unsigned int data;
    int stop, accesses = 0;


    // Begin reading from the file and using the caches
    while ((stop = fscanf(input, "%x", &data)) != EOF)
    {
        updateDirectMappedCache(directCache, data);
        updateTwoWayCache(twoWayCache, data);
        updateFourWayCache(fourWayCache, data);
        updateFullCache(fullCache, data);
        accesses++;
    }

    // Calculate hit rate for each cache
    directCache->hitRate = ((double)directCache->hitCount / accesses) * 100;
    twoWayCache->hitRate = ((double)twoWayCache->hitCount / accesses) * 100;
    fourWayCache->hitRate = ((double)fourWayCache->hitCount / accesses) * 100;
    fullCache->hitRate = ((double)fullCache->hitCount / accesses) * 100;

    // Print out all of the information
    int hits;
    double rate;

    // Direct cache
    hits = directCache->hitCount;
    rate = directCache->hitRate;
    printf("For the direct-mapped cache: Hits = %d\t\tAccesses = %d\t\tHit Rate = %.2f%%\n", hits, accesses, rate);

    // 2-way cache
    hits = twoWayCache->hitCount;
    rate = twoWayCache->hitRate;
    printf("For the 2-way associative cache: Hits = %d\t\tAccesses = %d\t\tHit Rate = %.2f%%\n", hits, accesses, rate);

    // 4-way cache
    hits = fourWayCache->hitCount;
    rate = fourWayCache->hitRate;
    printf("For the 4-way associative cache: Hits = %d\t\tAccesses = %d\t\tHit Rate = %.2f%%\n", hits, accesses, rate);

    // Full cache
    hits = fullCache->hitCount;
    rate = fullCache->hitRate;
    printf("For the fully associative cache: Hits = %d\t\tAccesses = %d\t\tHit Rate = %.2f%%\n", hits, accesses, rate);
    printf("\n\n\n%d\n%d\n%d\n%d\n\n\n", directCache->totalAccesses, twoWayCache->totalAccesses, fourWayCache->totalAccesses, fullCache->totalAccesses);
    // Free all of allocated memory
    free(directCache);
    freeAll(twoWayCache);
    freeAll(fourWayCache);
    free(fullCache);

    // Close file
    fclose(input);

    return 0;
}

// Function Definition(s)
void updateDirectMappedCache(Info * cache, unsigned int data)
{
    // Calculate the index to where the data should go
    int index = data % BLOCKS;

    // If the value at the index is the same as data, then increase the hit count
    // Otherwise, update the value at that index
    if (cache->cache[index] == data)
        cache->hitCount++;
    else
        cache->cache[index] = data;

    // Increase the access count
    cache->totalAccesses++;
}

void updateTwoWayCache(Info * cache, unsigned int data)
{
    // Calculate index
    int index = data % (BLOCKS / 2);
    int emptySpot = -1; // Saves the index of an empty spot
    int oldestSpot = -1; // Saves the index of the oldest value

    // Search for an empty spot while comparing values along the way
    // If a number is the same as data, increase hit count and exit loop
    // If a match is not found, place in empty spot
    // If there is no empty spot, place data in oldest spot
    // NOTE: This also implements random replacement since in the case that there
    // might be similar ages, the program will select the recent oldest spot
    for (int i = 0; i < sizeof(cache->twoDCache[index]); i++)
    {
        // Check if the data at this position is a match
        if (cache->twoDCache[index][i] == data)
        {
            cache->hitCount++;
            cache->twoDDataAges[index][i] = 0;
            break;
        }

        // Check if this is an empty position
        if (cache->twoDCache[index][i] == -1)
        {
            emptySpot = i;
            break;
        }

        // If the program reaches here, then there is no match or empty spot,
        // so increment the age at this position
        cache->twoDDataAges[index][i]++;

        // Update the oldest position
        if (cache->twoDDataAges[index][i] > oldestSpot)
            oldestSpot = i;
    }

    // If there is an empty spot, place data there
    if (emptySpot != -1)
    {
        cache->twoDCache[index][emptySpot] = data;
        cache->twoDDataAges[index][emptySpot] = 0;
    }
    else
    {
        // If the program reaches here, then put data in the oldest spot
        cache->twoDCache[index][oldestSpot] = data;
        cache->twoDDataAges[index][oldestSpot] = 0;
    }

    // Increase access count
    cache->totalAccesses++;
}

void updateFourWayCache(Info * cache, unsigned int data)
{
    // Calculate index
    int index = data % (BLOCKS / 4);
    int emptySpot = -1; // Saves the index of an empty spot
    int oldestSpot = -1; // Saves the index of the oldest value

    // Search for an empty spot while comparing values along the way
    // If a number is the same as data, increase hit count and exit loop
    // If a match is not found, place in empty spot
    // If there is no empty spot, place data in oldest spot
    // NOTE: This also implements random replacement since in the case that there
    // might be similar ages, the program will select the recent oldest spot
    for (int i = 0; i < sizeof(cache->twoDCache[index]); i++)
    {
        // Check if the data at this position is a match
        if (cache->twoDCache[index][i] == data)
        {
            cache->hitCount++;
            cache->twoDDataAges[index][i] = 0;
            break;
        }

        // Check if this is an empty position
        if (cache->twoDCache[index][i] == -1)
        {
            emptySpot = i;
            break;
        }

        // If the program reaches here, then there is no match or empty spot,
        // so increment the age at this position
        cache->twoDDataAges[index][i]++;

        // Update the oldest position
        if (cache->twoDDataAges[index][i] > oldestSpot)
            oldestSpot = i;
    }

    // If there is an empty spot, place data there
    if (emptySpot != -1)
    {
        cache->twoDCache[index][emptySpot] = data;
        cache->twoDDataAges[index][emptySpot] = 0;
    }
    else
    {
        // If the program reaches here, then put data in the oldest spot
        cache->twoDCache[index][oldestSpot] = data;
        cache->twoDDataAges[index][oldestSpot] = 0;
    }

    // Increase access count
    cache->totalAccesses++;
}

void updateFullCache(Info * cache, unsigned int data)
{
    int emptySpot = -1; // Saves the index of an empty spot
    int oldestSpot = -1; // Saves the index of the oldest value

    // Search for an empty spot while comparing values along the way
    // If a number is the same as data, increase hit count and exit loop
    // If a match is not found, place in empty spot
    // If there is no empty spot, place data in oldest spot
    // NOTE: This also implements random replacement since in the case that there
    // might be similar ages, the program will select the recent oldest spot
    for (int i = 0; i < sizeof(cache->cache); i++)
    {
        // Check if the data at this position is a match
        if (cache->cache[i] == data)
        {
            cache->hitCount++;
            cache->dataAges[i] = 0;
            break;
        }

        // Check if this is an empty position
        if (cache->cache[i] == -1)
        {
            emptySpot = i;
            break;
        }

        // If the program reaches here, then there is no match or empty spot,
        // so increment the age at this position
        cache->dataAges[i]++;

        // Update the oldest position
        if (cache->dataAges[i] > oldestSpot)
            oldestSpot = i;
    }

    // If there is an empty spot, place data there
    if (emptySpot != -1)
    {
        cache->cache[emptySpot] = data;
        cache->dataAges[emptySpot] = 0;
    }
    else
    {
        // If the program reaches here, then put data in the oldest spot
        cache->cache[oldestSpot] = data;
        cache->cache[oldestSpot] = 0;
    }

    // Increase access count
    cache->totalAccesses++;
}

Info * initializeInfo(void)
{
    Info * info = (Info *)malloc(sizeof(Info));

     // Initialization of struct
    for (int i = 0; i < BLOCKS; i++)
    {
        info->cache[i] = -1;
        info->dataAges[i] = 0;
    }

    info->hitCount = 0;
    info->totalAccesses = 0;

    return info;
}

Info * initializeCache(int row, int col)
{
    Info * info = initializeInfo();

    // Initialization of struct
    info->twoDCache = (unsigned int **)malloc(row * sizeof(unsigned int *));
    info->twoDDataAges = (int **)malloc(row * sizeof(int *));
    for (int i = 0; i < row; i++)
    {
        info->twoDCache[i] = (unsigned int *)malloc(col * sizeof(unsigned int));
        info->twoDDataAges[i] = (int *)malloc(col * sizeof(int));

        for (int j = 0; j < col; j++)
        {
            info->twoDDataAges[i][j] = 0;
            info->twoDCache[i][j] = -1;
        }
    }

    return info;
}

void freeAll(Info * info)
{
    for (int i = 0; i < sizeof(info->twoDCache); i++)
    {
       free(info->twoDCache[i]);
       free(info->twoDDataAges[i]);
    }

    free(info->twoDCache);
    free(info->twoDDataAges);
    free(info);
}