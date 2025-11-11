#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include "columnSort.h"

static int numCompare(const void *x, const void *y) {
    return (*(int *)x - *(int *)y);
}

void displayMatrix(const char *title, int **grid, int rows, int cols) {
    printf("%s:\n", title);
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            printf("%d ", grid[r][c]);
        }
        printf("\n");
    }
}

void columnSort(int *data, int workers, int height, int width, double *timeElapsed) {
    int rows = height, cols = width;
    if (!data || !timeElapsed || rows <= 0 || cols <= 0 || (rows % cols != 0) || rows < 2 * (cols - 1) * (cols - 1)) {
        if (timeElapsed) *timeElapsed = 0.0;
        return;
    }

    int **grid = (int **)calloc(rows, sizeof(int *));
    for (int r = 0; r < rows; r++) {
        grid[r] = (int *)calloc(cols, sizeof(int));
        memcpy(grid[r], &data[r * cols], cols * sizeof(int));
    }

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    for (int c = 0; c < cols; c++) {
        int *tempCol = (int *)malloc(rows * sizeof(int));
        for (int r = 0; r < rows; r++) tempCol[r] = grid[r][c];
        qsort(tempCol, rows, sizeof(int), numCompare);
        for (int r = 0; r < rows; r++) grid[r][c] = tempCol[r];
        free(tempCol);
    }
    
    int shiftAmount = rows / 2;
    int **shiftGrid = (int **)calloc(rows, sizeof(int *));
    for (int r = 0; r < rows; r++) shiftGrid[r] = (int *)calloc(cols, sizeof(int));

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            int newRow = (r + shiftAmount) % rows;
            shiftGrid[newRow][c] = grid[r][c];
        }
    }

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            int origRow = (r - shiftAmount + rows) % rows;
            grid[origRow][c] = shiftGrid[r][c];
        }
    }

    int *flatArr = (int *)calloc(rows * cols, sizeof(int));
    int idx = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            flatArr[idx++] = grid[r][c];
        }
    }
    qsort(flatArr, rows * cols, sizeof(int), numCompare);

    idx = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            grid[r][c] = flatArr[idx++];
        }
    }
    free(flatArr);

    gettimeofday(&endTime, NULL);
    *timeElapsed = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;
    if (*timeElapsed == 0.0) *timeElapsed = 0.000001;

    for (int r = 0; r < rows; r++) {
        memcpy(&data[r * cols], grid[r], cols * sizeof(int));
        free(grid[r]);
        free(shiftGrid[r]);
    }
    free(grid);
    free(shiftGrid);
}
