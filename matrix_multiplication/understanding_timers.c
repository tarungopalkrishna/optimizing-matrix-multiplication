#include<time.h>
#include<stdio.h>
#include<unistd.h>



int main(){

#ifdef CLOCK
    clock_t start = clock();
    sleep(5);
    clock_t end = clock();
    printf("start: %ld, end: %ld, elapsed: %f\n", start, end, (double)(end - start) / CLOCKS_PER_SEC);
#endif
#ifdef TIME
    time_t start = time(NULL);
    sleep(5);
    time_t end = time(NULL);
    printf("start: %ld, end: %ld, elapsed: %f\n", start, end, (double)(end - start));
#endif
    return 0;
}
