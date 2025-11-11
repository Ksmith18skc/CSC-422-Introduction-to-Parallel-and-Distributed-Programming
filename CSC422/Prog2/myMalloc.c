// CSc 422
// Program 2 code for sequential myMalloc (small blocks *only*)
// You must add support for large blocks and then concurrency (coarse- and fine-grain)

#include <stdlib.h>
#include <stdio.h>
#include "myMalloc-helper.h"
#include <pthread.h>

// total amount of memory to allocate, and size of each small chunk
#define SIZE_TOTAL 276672
#define SIZE_SMALL 64
#define MAX_ThreadS 8

pthread_key_t threadKey;
pthread_mutex_t overflowLockSmall = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t overflowLockLarge = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;
int allocMode = 0;
// allocMode(0) = sequential
// allocMode(1) = fine grain
// allocMode(2) = coarse grain

// maintain lists of free blocks and allocated blocks
typedef struct memoryManager
{
    chunk *freeSmall;
    chunk *allocSmall;
    chunk *allocLarge;
    chunk *freeLarge;
} memManager;

memManager *mMan;
memManager *threadMemManagers[MAX_ThreadS];
memManager *overflowManager;

int threadCount = 0;
pthread_mutex_t idLock = PTHREAD_MUTEX_INITIALIZER;

int myInit(int numThreads, int flag)
{
    allocMode = flag;

    if (flag == 2) // fine
    {
        pthread_key_create(&threadKey, NULL);

        void *overflowMem = malloc(SIZE_TOTAL);
        if (overflowMem == NULL)
            return -1;

        overflowManager = (memManager *)malloc(sizeof(memManager));
        overflowManager->freeSmall = createList();
        overflowManager->allocSmall = createList();
        overflowManager->freeLarge = createList();
        overflowManager->allocLarge = createList();

        int numSmall = (SIZE_TOTAL / 2) / (SIZE_SMALL + sizeof(chunk));
        int numLarge = (SIZE_TOTAL / 2) / (1024 + sizeof(chunk));

        setUpChunks(overflowManager->freeSmall, overflowMem, numSmall, SIZE_SMALL);
        setUpChunks(overflowManager->freeLarge,
                    (void *)((char *)overflowMem + SIZE_TOTAL / 2), numLarge, 1024);
    }
    else // seq || coarse
    {
        void *mem = malloc(SIZE_TOTAL);
        if (mem == NULL)
            return -1;

        mMan = (memManager *)malloc(sizeof(memManager));
        mMan->freeSmall = createList();
        mMan->allocSmall = createList();
        mMan->freeLarge = createList();
        mMan->allocLarge = createList();

        int numSmall = (SIZE_TOTAL / 2) / (SIZE_SMALL + sizeof(chunk));
        int numLarge = (SIZE_TOTAL / 2) / (1024 + sizeof(chunk));

        setUpChunks(mMan->freeSmall, mem, numSmall, SIZE_SMALL);
        setUpChunks(mMan->freeLarge, (void *)((char *)mem + SIZE_TOTAL / 2), numLarge, 1024);
    }

    return 0;
}

void initThreadAllocator()
{
    pthread_mutex_lock(&idLock);
    int id = threadCount++;
    pthread_mutex_unlock(&idLock);

    void *mem = malloc(SIZE_TOTAL);
    memManager *localMan = (memManager *)malloc(sizeof(memManager));
    localMan->freeSmall = createList();
    localMan->allocSmall = createList();
    localMan->freeLarge = createList();
    localMan->allocLarge = createList();

    int numSmall = (SIZE_TOTAL / 2) / (SIZE_SMALL + sizeof(chunk));
    int numLarge = (SIZE_TOTAL / 2) / (1024 + sizeof(chunk));

    setUpChunks(localMan->freeSmall, mem, numSmall, SIZE_SMALL);
    setUpChunks(localMan->freeLarge,
                (void *)((char *)mem + SIZE_TOTAL / 2), numLarge, 1024);

    threadMemManagers[id] = localMan;
    pthread_setspecific(threadKey, localMan);
}

// myMalloc just needs to get the next chunk and return a pointer to its data
// note the pointer arithmetic that makes sure to skip over our metadata and
// return the user a pointer to the data
void *myMalloc(int size)
{

    if (size > 1024)
    {
        return NULL;
    }

    chunk *toAlloc;

    if (allocMode == 2)
    {
        memManager *localMan = pthread_getspecific(threadKey);
        if (localMan == NULL)
        {
            initThreadAllocator();
            localMan = pthread_getspecific(threadKey);
        }

        chunk *list = (size <= 64) ? localMan->freeSmall : localMan->freeLarge;
        chunk *allocList = (size <= 64) ? localMan->allocSmall : localMan->allocLarge;

        if (!isEmptyList(list))
        {
            toAlloc = getChunk(list, allocList);
        }
        else
        {
            // Use overflow
            pthread_mutex_t *lock = (size <= 64) ? &overflowLockSmall : &overflowLockLarge;
            memManager *overflow = overflowManager;

            pthread_mutex_lock(lock);
            toAlloc = getChunk((size <= 64) ? overflow->freeSmall : overflow->freeLarge,
                               (size <= 64) ? overflow->allocSmall : overflow->allocLarge);
            pthread_mutex_unlock(lock);
        }

        return ((void *)((char *)toAlloc + sizeof(chunk)));
    }

    // Fallback to single/coarse mode
    if (allocMode == 1)
        pthread_mutex_lock(&globalLock);

    if (size <= 64)
        toAlloc = getChunk(mMan->freeSmall, mMan->allocSmall);
    else
        toAlloc = getChunk(mMan->freeLarge, mMan->allocLarge);

    if (allocMode == 1)
        pthread_mutex_unlock(&globalLock);

    return ((void *)((char *)toAlloc + sizeof(chunk)));
}

void myFree(void *ptr)
{
    chunk *toFree = (chunk *)((char *)ptr - sizeof(chunk));
    int isSmall = toFree->allocSize <= 64;

    if (allocMode == 2)
    {
        memManager *localMan = pthread_getspecific(threadKey);
        if (localMan == NULL)
        {
            return; // fixed
        }

        chunk *allocList = isSmall ? localMan->allocSmall : localMan->allocLarge;
        chunk *freeList = isSmall ? localMan->freeSmall : localMan->freeLarge;

        // returning to local list first
        if (toFree->prev && toFree->next)
        {
            returnChunk(freeList, allocList, toFree);
        }
        else
        {
            // Overflow check
            pthread_mutex_t *lock = isSmall ? &overflowLockSmall : &overflowLockLarge;
            memManager *overflow = overflowManager;

            pthread_mutex_lock(lock);
            returnChunk(isSmall ? overflow->freeSmall : overflow->freeLarge,
                        isSmall ? overflow->allocSmall : overflow->allocLarge,
                        toFree);
            pthread_mutex_unlock(lock);
        }

        return;
    }

    if (allocMode == 1)
        pthread_mutex_lock(&globalLock);

    if (isSmall)
        returnChunk(mMan->freeSmall, mMan->allocSmall, toFree);
    else
        returnChunk(mMan->freeLarge, mMan->allocLarge, toFree);

    if (allocMode == 1)
        pthread_mutex_unlock(&globalLock);
}
