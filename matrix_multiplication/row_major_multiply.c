#include "common.h"


// TODO: change the how the timer works, its broken

void matmul(){
    #ifdef DEBUG
    printf("Initial matrices:\n");
    print_matrix();
#endif
    uint64_t start = nanos();
    for(int i =0;i<N;i++){
        for(int k=0;k<N;k++){
            for(int j=0;j<N;j++){
                c[i][j] += a[i][k] * b[k][j];
            }
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
    return 0;
}
