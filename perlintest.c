#include <stdio.h>
#include <time.h>
#include "perlin.h"

int main() {
    int i = 0, j = 0, c = 0;
    double n;
    double g[50][50];
    clock_t t1, t2, t3;

    t1 = clock();

    for(i = 0; i < 50; i++) {
        for(j = 0; j < 50; j++) {
            n = pnoise2d(j * 0.1, i * 0.1, 0.5, 5, 12124);
            c = (int)((n + 1) * 4);
            printf("%d ", c);
        }
        printf("\n");
    }
    printf("\n\n");

    t2 = clock();

    pbatch2d(g, 0.0, 0.0, 50, 50, 0.1, 0.5, 5, 12124);
    for(i = 0; i < 50; i++) {
        for(j = 0; j < 50; j++) {
            c = (int)((g[i][j] + 1) * 4);
            printf("%d ", c);
        }
        printf("\n");
    }
    printf("\n\n");

    t3 = clock();

    float diff1 = (((float)t2 - (float)t1) / 1000000.0F) * 1000;
    float diff2 = (((float)t3 - (float)t2) / 1000000.0F) * 1000;
    printf("Normal method: %f\n", diff1);
    printf("Batch method: %f\n", diff2);
    printf("Efficiency: %f\n", diff2 / diff1);

    return 0;
}
