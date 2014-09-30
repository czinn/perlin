#include <stdio.h>
#include <math.h>

double rawnoise(int n) {
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double noise1d(int x, int octave, int seed) {
    return rawnoise(x * 1619 + octave * 3463 + seed * 13397);
}

double noise2d(int x, int y, int octave, int seed) {
    return rawnoise(x * 1619 + y * 31337 + octave * 3463 + seed * 13397);
}

double noise3d(int x, int y, int z, int octave, int seed) {
    return rawnoise(x * 1919 + y * 31337 + z * 7669 + octave * 3463 + seed * 13397);
}

double interpolate(double a, double b, double x) {
    double f = (1 - cos(x * 3.141593)) * 0.5;

    return a * (1 - f) + b * f;
}

double smooth1d(double x, int octave, int seed) {
    int intx = (int)x;
    double fracx = x - intx;

    double v1 = noise1d(intx, octave, seed);
    double v2 = noise1d(intx + 1, octave, seed);

    return interpolate(v1, v2, fracx);
}

double smooth2d(double x, double y, int octave, int seed) {
    int intx = (int)x;
    double fracx = x - intx;
    int inty = (int)y;
    double fracy = y - inty;
    
    double v1 = noise2d(intx, inty, octave, seed);
    double v2 = noise2d(intx + 1, inty, octave, seed);
    double v3 = noise2d(intx, inty + 1, octave, seed);
    double v4 = noise2d(intx + 1, inty + 1, octave, seed);
    
    double i1 = interpolate(v1, v2, fracx);
    double i2 = interpolate(v3, v4, fracx);
    
    return interpolate(i1, i2, fracy);
}

double smooth3d(double x, double y, double z, int octave, int seed) {
    int intx = (int)x;
    double fracx = x - intx;
    int inty = (int)y;
    double fracy = y - inty;
    int intz = (int)z;
    double fracz = z - intz;


    double v1 = noise3d(intx, inty, intz, octave, seed);
    double v2 = noise3d(intx + 1, inty, intz, octave, seed);
    double v3 = noise3d(intx, inty + 1, intz, octave, seed);
    double v4 = noise3d(intx + 1, inty + 1, intz, octave, seed);
    double v5 = noise3d(intx, inty, intz + 1, octave, seed);
    double v6 = noise3d(intx + 1, inty, intz + 1, octave, seed);
    double v7 = noise3d(intx, inty + 1, intz + 1, octave, seed);
    double v8 = noise3d(intx + 1, inty + 1, intz + 1, octave, seed);

    double i1 = interpolate(v1, v2, fracx);
    double i2 = interpolate(v3, v4, fracx);
    double i3 = interpolate(v5, v6, fracx);
    double i4 = interpolate(v7, v8, fracx);
    
    double j1 = interpolate(i1, i2, fracy);
    double j2 = interpolate(i3, i4, fracy);

    return interpolate(j1, j2, fracz);
}

double pnoise1d(double x, double persistence, int octaves, int seed) {
   double total = 0.0;
   double frequency = 1.0;
   double amplitude = 1.0;
   int i = 0;
   
   for(i = 0; i < octaves; i++) {
       total += smooth1d(x * frequency, i, seed) * amplitude;
       frequency /= 2;
       amplitude *= persistence;
   } 

   return total;
}

double pnoise2d(double x, double y, double persistence, int octaves, int seed) {
   double total = 0.0;
   double frequency = 1.0;
   double amplitude = 1.0;
   int i = 0;
   
   for(i = 0; i < octaves; i++) {
       total += smooth2d(x * frequency, y * frequency, i, seed) * amplitude;
       frequency /= 2;
       amplitude *= persistence;
   } 

   return total;
}

double pnoise3d(double x, double y, double z, double persistence, int octaves, int seed) {
   double total = 0.0;
   double frequency = 1.0;
   double amplitude = 1.0;
   int i = 0;
   
   for(i = 0; i < octaves; i++) {
       total += smooth3d(x * frequency, y * frequency, z * frequency, i, seed) * amplitude;
       frequency /= 2;
       amplitude *= persistence;
   } 

   return total;
}

void pbatch1d(double *grid, double x, int xs, double step, double persistence, int octaves, int seed) {
    int i = 0, j = 0;

    double xEnd = x + xs * step;

    int octaveStart = 0;
    double minFrequency = pow(0.5, octaves - 1);
    int maxOctaveX = ceil(xEnd / minFrequency) - floor(x / minFrequency) + 1;
    double octaveData[maxOctaveX];

    double frequency = 1.0;
    double amplitude = 1.0;

    int intx;
    double fracx;

    for(i = 0; i < octaves; i++) {
        // Initialize the octaveData
        octaveStart = floor(x / frequency);
        for(j = 0; octaveStart + j <= ceil(xEnd / frequency); j++) {
            octaveData[j] = noise1d(octaveStart + j, i, seed);
        }
        // Interpolate and add to the things
        for(j = 0; j < xs; j++) {
            if(i == 0)
                grid[j] = 0;
            intx = (int)((x + j * step) * frequency);
            fracx = (x + j * step) * frequency - intx;
            grid[j] += interpolate(octaveData[intx - octaveStart], octaveData[intx - octaveStart + 1], fracx) * amplitude;
        }

        frequency /= 2;
        amplitude *= persistence;
    }
}

void pbatch2d(double **grid, double x, double y, int xs, int ys, double step, double persistence, int octaves, int seed) {
    int i = 0, j = 0, k = 0;

    double xEnd = x + xs * step;
    double yEnd = y + ys * step;

    int octaveStartX = 0, octaveStartY = 0;
    double minFrequency = pow(0.5, octaves - 1);
    int maxOctaveX = ceil(xEnd / minFrequency) - floor(x / minFrequency) + 1;
    int maxOctaveY = ceil(yEnd / minFrequency) - floor(y / minFrequency) + 1;
    double octaveData[maxOctaveY][maxOctaveX];

    double frequency = 1.0;
    double amplitude = 1.0;

    int intx, inty;
    double fracx, fracy;

    for(i = 0; i < octaves; i++) {
        // Initialize the octaveData
        octaveStartX = floor(x / frequency);
        octaveStartY = floor(y / frequency);
        for(j = 0; octaveStartY + j <= ceil(yEnd / frequency); j++) {
            for(k = 0; octaveStartX + k <= ceil(xEnd / frequency); k++) {
                octaveData[j][k] = noise2d(octaveStartX + k, octaveStartY + j, i, seed);
            }
        }
        // Interpolate and add to the things
        for(j = 0; j < ys; j++) {
            for(k = 0; k < xs; k++) {
                if(i == 0)
                    grid[j][k] = 0;
                intx = (int)((x + k * step) * frequency);
                fracx = (x + k * step) * frequency - intx;
                inty = (int)((y + j * step) * frequency);
                fracy = (y + j * step) * frequency - intx;

                grid[j][k] += interpolate(\
                    interpolate(octaveData[inty - octaveStartY][intx - octaveStartX], octaveData[inty - octaveStartY][intx - octaveStartX + 1], fracx),\
                    interpolate(octaveData[inty - octaveStartY + 1][intx - octaveStartX], octaveData[inty - octaveStartY + 1][intx - octaveStartX + 1], fracx),\
                    fracy) * amplitude;
            }
        }

        frequency /= 2;
        amplitude *= persistence;
    }
}
