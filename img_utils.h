#ifndef _IMG_UTILS_H_
#define _IMG_UTILS_H_

#include <wand/MagickWand.h>
#include "net.h"


  void normalize(MagickWand *wand, int image_size, double threshold);
  void get_input_data(MagickWand *wand, nfloat_t *data);


#endif//_IMG_UTILS_H_
