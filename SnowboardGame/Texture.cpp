#include "Texture.h"
#include <iostream>
#include <GLFW/glfw3.h>

static Bitmap *g_textures[MAX_TEX_INDEX + 1];

/* Helper functions to load a bitmap byte-by-byte. */
static unsigned int getint(FILE *fp)
{
   int c, c1, c2, c3;

   /*  get 4 bytes */
   c = getc(fp);
   c1 = getc(fp);
   c2 = getc(fp);
   c3 = getc(fp);

   return ((unsigned int) c) +
      (((unsigned int) c1) << 8) +
      (((unsigned int) c2) << 16) +
      (((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp)
{
   int c, c1;

   /* get 2 bytes*/
   c = getc(fp);
   c1 = getc(fp);

   return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/* Bitmap loader for 24 bit bitmaps with 1 plane only. */
void loadBitmap(char *filename, int texId)
{
   FILE *file;
   unsigned long size;                 /*  size of the image in bytes. */
   unsigned long i;                    /*  standard counter. */
   unsigned short int planes;          /*  number of planes in image (must be 1)  */
   unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
   char temp;                          /*  used to convert bgr to rgb color. */
   Bitmap *bitmap;

   g_textures[texId] = (Bitmap *)malloc(sizeof(Bitmap));
   bitmap = g_textures[texId];

   if ((file = fopen(filename, "rb"))==NULL) {
      std::cout << "File not found: " << filename << std::endl;
      exit(1);
   }

   // Seek through the bmp header, up to the width and height values.
   fseek(file, 18, SEEK_CUR);
   bitmap->sizeX = getint(file);
   bitmap->sizeY = getint(file);

  // Calculate the size of the bitmap (24 bits/3 bytes per pixel).
  size = bitmap->sizeX * bitmap->sizeY * 3;

   // Read the planes.
   planes = getshort(file);
   if (planes != 1) {
      printf("Planes from %s is not 1: %u\n", filename, planes);
      std::cout << "Planes from " << filename << " is not 1: " << planes
       << std::endl;
      exit(1);
   }

   // Read the bpp.
   bpp = getshort(file);
   if (bpp != 24) {
      printf("Bpp from %s is not 24: %u\n", filename, bpp);
      std::cout << "Bits per pixel from " << filename << " is not 24: " << bpp
       << std::endl;
      exit(1);
   }

   // Seek past the rest of the bitmap header.
   fseek(file, 24, SEEK_CUR);

   // Read the data.
   bitmap->data = (char *)malloc(size);
   if (bitmap->data == NULL) {
      std::cout << "Error allocating memory for color-corrected image data"
       << std::endl;
      exit(1);
   }

   if ((i = fread(bitmap->data, size, 1, file)) != 1) {
      std::cout << "Error reading image data from " << filename << std::endl;
      exit(1);
   }

   /*
   // Reverse all of the colors (bgr -> rgb).
   for (i = 0; i < size;i += 3) {
      temp = bitmap->data[i];
      bitmap->data[i] = bitmap->data[i + 2];
      bitmap->data[i + 2] = temp;
   }
   */
   for (i = 0; i < size; i += 3) {
      temp = bitmap->data[i];
      bitmap->data[i] = bitmap->data[i + 1];
      bitmap->data[i + 1] = temp;
   }

   fclose(file);

   // Send the texture to the GPU.
   glBindTexture(GL_TEXTURE_2D, texId);
   glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap->sizeX, bitmap->sizeY,
    0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   // Cheap scaling when image is bigger than texture.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   // Cheap scaling when image is smaller than texture.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void initTextures()
{
   glEnable(GL_TEXTURE_2D);

   loadBitmap((char *)"textures/snow.bmp", SNOW_TEX);
   loadBitmap((char *)"textures/pine.bmp", PINE_TEX);
   loadBitmap((char *)"textures/bark.bmp", BARK_TEX);
   loadBitmap((char *)"textures/blank.bmp", BLANK_TEX);
}
