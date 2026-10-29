#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MAX_INTENSITY 255
#define ENCODING_BASE 256

void generateRandomMatrix(int *matrix, int matrixSize){
    int *currentElement = matrix;
    int totalElements = matrixSize * matrixSize; //NxN matrix

    for(int index = 0 ; index < totalElements ;index++){
        *currentElement = rand() % (MAX_INTENSITY +1);
        currentElement++; 
    }
}

void displayMatrix(int *matrix , int matrixSize){
    int *currentElement = matrix;
    int totalElements = matrixSize * matrixSize;
    for(int index = 0 ; index < totalElements ;index++){
        printf("%d ", *currentElement);
        currentElement++;
        
        // if we are at the end of the row
        if((index+1)%matrixSize == 0){
            printf("\n");
        }
    }
}

void swap(int *num1 , int *num2){
    int temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}

void transposeMatrix(int *matrix , int matrixSize){
    for(int row = 0; row < matrixSize;row++){
        for(int column = row+1; column < matrixSize;column++){
            int *upperElement = matrix + row * matrixSize + column;
            int *lowerElement = matrix + column * matrixSize +row;
            swap(upperElement,lowerElement);
        }
    }
}

void reverseMatrix(int *matrix , int matrixSize){
    for(int row = 0; row < matrixSize;row++){
        for(int column = 0; column < matrixSize/2;column++){
            int *upperElement = matrix + row * matrixSize + column;
            int *lowerElement = matrix + row * matrixSize +(matrixSize -1-column);
            swap(upperElement,lowerElement);
        }
    }
}

void rotateMatrixClockwise(int *matrix , int matrixSize){
    transposeMatrix(matrix, matrixSize);
    reverseMatrix(matrix,matrixSize);
}

int calculateNeighbourhoodAverage(int *matrix , int matrixSize, int row , int column){
    int sum = 0;
    int validcount = 0;
    for(int currentrow = row - 1 ;currentrow <= row+1;currentrow++){
        for(int currentcolumn = column -1 ;currentcolumn <= column+1;currentcolumn++){
            if(currentrow >= 0 && currentrow < matrixSize && currentcolumn >= 0 && currentcolumn < matrixSize){
                int *neighbourElement = matrix + currentrow * matrixSize +currentcolumn;
                sum += *neighbourElement % ENCODING_BASE;
                validcount++;
            }
        }
    }
    return sum / validcount;
}

void encodeSmoothendValues(int *matrix , int matrixSize){
    for(int row = 0;row<matrixSize;row++){
        for(int column = 0;column < matrixSize ;column++){
            int *currentElement = matrix + row * matrixSize + column;
            // The remainder gives the original value because it is between 0 and 255
            int originalValue = *currentElement % ENCODING_BASE;
            // Temporarily store both values in the same integer:
            // originalValue occupies the remainder when divided by ENCODING_BASE,
            // while smoothedValue is stored as the quotient.
            int smoothedValue = calculateNeighbourhoodAverage(matrix, matrixSize, row, column);
            *currentElement =originalValue+ smoothedValue * ENCODING_BASE;
        }
    }
}
void decodeSmoothedValues(int *matrix , int matrixSize){
    int *currentElement = matrix;
    int totalElements = matrixSize * matrixSize;
    for(int index = 0 ; index < totalElements ;index++){
        *currentElement = *currentElement / ENCODING_BASE;
        currentElement++; 
    }

}

void smoothMatrix(int *matrix , int matrixSize){
    encodeSmoothendValues(matrix,matrixSize);
    decodeSmoothedValues(matrix,matrixSize);
}

int main(void){
    int matrixSize;
    printf("Enter matrix size: ");
    if(scanf("%d" ,&matrixSize) != 1 || matrixSize < MIN_MATRIX_SIZE || matrixSize > MAX_MATRIX_SIZE){
        printf("Invalid Matix size. It should be between 2 to 10.\n");
        return 1;
    }
    srand((unsigned int)time(NULL));

    int *matrix = malloc((size_t) matrixSize * matrixSize * sizeof(*matrix));
    if(matrix == NULL){
        printf("Memory Allocation failed.");
    }
    generateRandomMatrix(matrix,matrixSize);
    printf("\nOriginal Randomly generated Matrix \n");
    displayMatrix(matrix, matrixSize);

    rotateMatrixClockwise(matrix,matrixSize);
    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    displayMatrix(matrix, matrixSize);

    smoothMatrix(matrix, matrixSize);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, matrixSize);

    free(matrix);

    return 0;
}