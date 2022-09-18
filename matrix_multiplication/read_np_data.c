#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void print_matrix(int r, int n, float (*arr)[n]) {
    printf("The size of the matrix is: %d and the print rows are: %d\n", n, r);
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < r; j++) {
            // printf("%-16.8f ",*(arr + (i*n) + j));
            printf("%-16.8f ", arr[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]){
    if(argc < 4){
        printf("Usage: ./read_np_data <n> <r> <filename>\n");
        return 0;
    }
    int n = atoi(argv[1]);
    int r = atoi(argv[2]);
    char *filename = argv[3];
    // Need to dynamically allocate memory for the array
    printf("Allocating %ld bytes for the array", n*n*sizeof(float));
    float (*a)[n] = malloc(n*n*sizeof(float));
    //memset(a, 5, n*n*sizeof(float));
    printf("The address of the allocated memory is: %p\n", a);
    // float (*a)[n][n] = malloc(sizeof *a);
#ifndef DEBUG
    float (*b)[n] = malloc(n*n*sizeof(float));
    float (*c)[n] = malloc(n*n*sizeof(float));/
    // NULL is (void *)0
    if( a == NULL || b == NULL || c == NULL){
        printf("Memory allocation failed!\n");
        return 0;
    }
#else
    if( a == NULL){
        printf("Memory allocation failed!\n");
        return 0;
    }
#endif
    // memset array a to value 5
    printf("Hello what is this\n");
    FILE *f = fopen(filename, "rb");
    fread(a, 1, sizeof(float)*n*n, f); // Changing the second parameter has no impact on the output - WHAT?
    printf("Hello what is this\n");
#ifndef DEBUG
    fread(b, 1, sizeof(float)*n*n, f); // Changing the second parameter has no impact on the output - WHAT?
    fread(c, 1, sizeof(float)*n*n, f); // Changing the second parameter has no impact on the output - WHAT?S
#endif
    printf("This is matrix A:\n");
    printf("The value of r is: %d and the value of n is: %d\n", r, n);
    print_matrix(r, n, a);
#ifndef DEBUG
    printf("This is matrix B:\n");
    print_matrix(r, n, b);
    printf("This is matrix C:\n");
    print_matrix(r, n, c);
#endif
    // deallocate memeory
    free(a);
#ifndef DEBUG
    free(b);
    free(c);
#endif
    fclose(f);
    return 0;
}