#include<stdio.h>


#define BLAS_SYMBOL_PREFIX 
#define BLAS_SYMBOL_SUFFIX


#define BLAS_FUNC_CONCAT(name,prefix,suffix,suffix2) prefix ## name ## suffix ## suffix2
#define BLAS_FUNC_EXPAND(name,prefix,suffix,suffix2) BLAS_FUNC_CONCAT(name,prefix,suffix,suffix2)
#define CBLAS_FUNC(name) BLAS_FUNC_EXPAND(name,BLAS_SYMBOL_PREFIX,,BLAS_SYMBOL_SUFFIX)
// #define BLAS_FUNC(name) BLAS_FUNC_EXPAND(name,BLAS_SYMBOL_PREFIX,BLAS_FORTRAN_SUFFIX,BLAS_SYMBOL_SUFFIX)




void lib_testingwhatthisis(){
    printf("Hello this is within the function lib_testingwhatthisis\n");
}

void _testing(){
    printf("Hello this is within the function _testing\n");
}

int main(){
    // BLAS_FUNC_CONCAT(_testing, lib, what, thisis)();
    // BLAS_FUNC_EXPAND(_testing, lib, what, thisis)();
    CBLAS_FUNC(_testing)();
    return 0;
}
