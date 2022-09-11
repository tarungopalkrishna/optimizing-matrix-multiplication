#include "common.h"

// TODO: change the how the timer works, its broken

void matmul(){
#ifdef DEBUG
    printf("Initial matrices:\n");
    print_matrix();
#endif
    uint64_t start = nanos();
    for(int i =0;i<N;i++){
        for(int j=0;j<N;j++){
            float acc = 0;
            for(int k=0;k<N;k++){
                acc += a[i][k] * b[k][j];
            }
            c[i][j] = acc;
        }
    }
    uint64_t end = nanos();
    get_tflops(start, end, (char *)"Mutiplication:");
#ifdef DEBUG
    printf("Multiplied matrices:\n");
    print_matrix();
#endif
}

int main(){
    init_matrix();
    matmul();
}
