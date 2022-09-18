#include<stdio.h>

#define N 16




int main(){
    int a[N][N];
    int b[N][N];
    int c[N][N];
    int i,j,k;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            c[i][j]=(N*i)+j+1;
            printf("%-3d ",c[i][j]);
        }
        printf("\n");
    }
    return 0;
}