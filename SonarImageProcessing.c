#include <stdio.h>
#include <stdlib.h>

// Constant Declaration
#define MAX_SIZE 10
#define MIN_SIZE 2
#define MAX_VALUE 255

// Function ProtoTypes
int readMatrixSize();
void generateMatrix(int *matrix, int size);
void displayMatrix(int *mat, int size);
void rotateMatrix(int *mat, int size);
int averageAround(int *matrix, int size, int *previous, int *current, int row, int column);
void swapValues(int *first, int *second);
void transposeMatrix(int *mat, int size);
void copyRow(int *source, int *destination,int size);
void smoothMatrix(int *mat, int size);
void reverseRows(int *mat, int size);

// Safely reads matrix size between MIN_SIZE and MAX_SIZE
int readMatrixSize(){
    int size, result, ch;

    while(1){
        printf("Enter Matrix Size (%d-%d): ", MIN_SIZE, MAX_SIZE);
        result = scanf("%d", &size);

        if(result == EOF) exit(1);
        if(result == 1 && size >= MIN_SIZE && size <= MAX_SIZE) return size;

        printf("Invalid Input.");
        while(((ch = getchar()) != '\n') && ch != EOF);
        if(ch == EOF) exit(1);
    }
}

// Generate Random Matrix
void generateMatrix(int *matrix, int size){
    int *block;
    
    for(block = matrix ; block < matrix + size * size; block++){
        *block = rand() % (MAX_VALUE + 1);
    }
}

// Displays N * N Matrix
void displayMatrix(int *mat, int size){
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size ; j++){
            printf("%4d", *(mat + i * size + j));
        }
        printf("\n");
    }
}

// Swaps Integer using pointer
void swapValues(int *first, int *second){
    int temp = *first;
    *first = *second;
    *second = temp;
}

// Transposes matrix along the diagonal
void transposeMatrix(int *mat, int size){
    for(int i=0 ; i<size ; i++){
        for(int j=i+1 ; j<size ; j++){
            swapValues(mat + i * size + j, mat + j * size + i);
        }
    }
}

// Reverse each Row Horizontally
void reverseRows(int *mat, int size){
    for(int i=0 ; i<size ; i++){
        for(int j=0 ; j<size/2 ; j++){
            swapValues(mat + i * size + j, mat + i * size + (size - 1 - j));
        }
    }
}

// Rotates matrix 90 degrees clockwise
void rotateMatrix(int *mat, int size){
    transposeMatrix(mat, size);
    reverseRows(mat, size);
}

// Copies row of size elements 
void copyRow(int *source, int *destination,int size){
    for(int i=0 ; i<size ; i++){
        *(destination + i) = *(source + i);
    }
}

// Applies a 3x3 box smoothing filter
void smoothMatrix(int *mat, int size){
    int previous[MAX_SIZE] = {0};
    int current[MAX_SIZE] = {0};
    
    for(int i=0 ; i<size ; i++){
        copyRow(mat + i * size, current, size);

        for(int j=0 ; j<size ; j++){
            *(mat + i * size + j) = averageAround(mat, size, previous, current , i, j);
        }
        copyRow(current, previous, size);
    }
}

// Calculates average of 3x3 neighbourhood for a cell
int averageAround(int *matrix, int size, int *previous, int *current, int row, int column){
    int sum = 0, count = 0;
    
    for(int i=row - 1; i<=row + 1; i++){
        for(int j=column - 1; j<= column + 1; j++){
            if(i >= 0 && i < size && j>=0 && j < size){
                if(i < row){
                    sum += *(previous + j);
                } else if(i == row){
                    sum += *(current + j);
                } else{
                    sum += *(matrix + i *size + j);
                }
                count++;
            }
        }
    }
    return sum / count;
}

int main(){
    int matrix[MAX_SIZE * MAX_SIZE];
    int size = readMatrixSize();

    generateMatrix(matrix, size);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, size);

    rotateMatrix(matrix, size);
    printf("\nMatrix after 90 degrees Clockwise Rotation:\n");
    displayMatrix(matrix, size);
    smoothMatrix(matrix, size);

    printf("\nMatrix after Applying 3x3 Smoothing Filtering:\n");
    displayMatrix(matrix, size);

    return 0;
}