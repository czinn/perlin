#include <stdio.h>
#include <time.h>
#include "perlin.h"

int main() {
    int i = 0, c = 0;
    double n;
    double row[10000];
    clock_t t1, t2, t3;

    t1 = clock();

    for(i = 0; i < 10000; i++) {
        n = pnoise1d(i * 0.1, 0.5, 5, 12124);
        c = (int)((n + 1) * 4);
        if(i < 50)
            printf("%d ", c);
    }
    printf("\n");

    t2 = clock();

    pbatch1d(row, 0.0, 10000, 0.1, 0.5, 5, 12124);
    for(i = 0; i < 10000; i++) {
        c = (int)((row[i] + 1) * 4);
        if(i < 50)
            printf("%d ", c);
    }
    printf("\n");

    t3 = clock();

    float diff1 = (((float)t2 - (float)t1) / 1000000.0F) * 1000;
    float diff2 = (((float)t3 - (float)t2) / 1000000.0F) * 1000;
    printf("Normal method: %f\n", diff1);
    printf("Batch method: %f\n", diff2);
    printf("Efficiency: %f\n", diff2 / diff1);

    return 0;
}
