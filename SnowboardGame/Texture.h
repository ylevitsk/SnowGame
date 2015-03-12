/*
 * Author: Evan Peterson
 * Cal Poly, CPE-471, Winter 2014
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#define SNOW_TEX 0
#define BLANK_TEX 1
#define PINE_TEX 2
#define BARK_TEX 3
#define MAX_TEX_INDEX 3

// Data structure for a bitmap image used in texture mapping.
typedef struct Bitmap {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
} Bitmap;

void initTextures();

#endif
