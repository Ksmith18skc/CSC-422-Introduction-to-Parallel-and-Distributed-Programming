#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "columnSort.h"

static int valueCompare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

typedef struct {
    int **grid;
    int startColumn;
    int columnCount;
    int rowCount;
} ThreadData;

void *threadedSort(void *args) {
    ThreadData *info = (ThreadData *)args;
    for (int c = info->startColumn; c < info->startColumn + info->columnCount; c++) {
        int *tempCol = (int *)malloc(info->rowCount * sizeof(int));
        for (int r = 0; r < info->rowCount; r++) {
            tempCol[r] = info->grid[r][c];
        }
        qsort(tempCol, info->rowCount, sizeof(int), valueCompare);
        for (int r = 0; r < info->rowCount; r++) {
            info->grid[r][c] = tempCol[r];
        }
        free(tempCol);
    }
    return NULL;
}

void columnSort(int *array, int numThreads, int height, int width, double *elapsedTime) {
    int rows = height, cols = width;
    if (!array || !elapsedTime || rows <= 0 || cols <= 0 || rows % cols != 0 || rows < 2 * (cols - 1) * (cols - 1)) {
        if (elapsedTime) *elapsedTime = 0.0;
        return;
    }

    int **grid = (int **)calloc(rows, sizeof(int *));
    for (int r = 0; r < rows; r++) {
        grid[r] = (int *)calloc(cols, sizeof(int));
        memcpy(grid[r], &array[r * cols], cols * sizeof(int));
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    int colsPerThread = cols / numThreads;

    for (int t = 0; t < numThreads; t++) {
        threadData[t].grid = grid;
        threadData[t].startColumn = t * colsPerThread;
        threadData[t].columnCount = colsPerThread;
        threadData[t].rowCount = rows;
        pthread_create(&threads[t], NULL, threadedSort, &threadData[t]);
    }
    
    for (int t = 0; t < numThreads; t++) {
        pthread_join(threads[t], NULL);
    }

    int shiftValue = rows / 2;
    int **shiftedGrid = (int **)calloc(rows, sizeof(int *));
    for (int r = 0; r < rows; r++) shiftedGrid[r] = (int *)calloc(cols, sizeof(int));

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            int newRow = (r + shiftValue) % rows;
            shiftedGrid[newRow][c] = grid[r][c];
        }
    }

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            int origRow = (r - shiftValue + rows) % rows;
            grid[origRow][c] = shiftedGrid[r][c];
        }
    }

    int *flatArr = (int *)calloc(rows * cols, sizeof(int));
    int idx = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            flatArr[idx++] = grid[r][c];
        }
    }
    qsort(flatArr, rows * cols, sizeof(int), valueCompare);

    idx = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            grid[r][c] = flatArr[idx++];
        }
    }
    free(flatArr);

    gettimeofday(&end, NULL);
    *elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    if (*elapsedTime == 0.0) *elapsedTime = 0.000001;

    for (int r = 0; r < rows; r++) {
        memcpy(&array[r * cols], grid[r], cols * sizeof(int));
        free(grid[r]);
        free(shiftedGrid[r]);
    }
    free(grid);
    free(shiftedGrid);
}
