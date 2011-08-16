/*
     File: Texture.h
 Abstract: 
  Version: 1.2
 
 
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <CoreGraphics/CGImage.h>

#include "Imaging.h"


void loadTextureFromFile(const char *name, Image *img, RendererInfo *renderer);

// 
void loadTextureFromCGImage(CGImageRef inputCGImage,Image *img, RendererInfo *renderer);


#endif /* TEXTURE_H */