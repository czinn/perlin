#include <stdio.h>
#include <time.h>
//#include <unistd.h>
#include "perlin.h"

//0123456789
// `'"+*0&#@
char d[11] = " `'\"+*0&#@";

int main() {
    int i = 0, j = 0, c = 0, z = 0;
    double n;
    clock_t stall;

    while(1) {
        stall = clock();
        for(i = 0; i < 50; i++) {
            for(j = 0; j < 50; j++) {
                n = pnoise3d(j * 0.1, i * 0.1, z * 0.1, 0.7, 5, 12124);
                c = (int)((n + 1) * 4);
                if(c < 0)
                    c = 0;
                if(c > 9)
                    c = 9;
                printf("%c", d[c]);
            }
            printf("\n");
        }
        printf("\n\n");
        z++;
        
        while(clock() - stall < 100000) {

        }
    }

    return 0;
}
