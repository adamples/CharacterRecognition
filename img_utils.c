#include "img_utils.h"

void
normalize(MagickWand *wand, int image_size, double threshold)
{
  PixelIterator       *iterator = NULL;
  PixelWand           **pixels = NULL;
  MagickPixelPacket   pixel;

  size_t  width, height;
  size_t  x, y;
  size_t  min_x, max_x, min_y, max_y;
  int     th = 65535 * (1 - threshold);

  width = MagickGetImageWidth(wand);
  height = MagickGetImageHeight(wand);

  min_x = width - 1;
  max_x = 0;
  min_y = height - 1;
  max_y = 0;

  MagickGaussianBlurImage(wand, 1, 1);
  MagickModulateImage(wand, 100, 0, 100);
  MagickAutoLevelImage(wand);
  iterator = NewPixelIterator(wand);

  for (y = 0; y < height; ++y) {
    pixels = PixelGetNextIteratorRow(iterator, &width);

    for (x = 0; x < width; ++x) {
      PixelGetMagickColor(pixels[x], &pixel);

      if (pixel.red < th) {
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
      }
    }
  }

  if (max_x - min_x + 1 > 0 && max_y - min_y + 1 > 0)
    MagickCropImage(wand, max_x - min_x + 1, max_y - min_y + 1, min_x, min_y);

  MagickResizeImage(wand, image_size, image_size, LanczosFilter, 1.0);
}


void
get_input_data(MagickWand *wand, nfloat_t *data)
{
  PixelIterator       *iterator = NULL;
  PixelWand           **pixels = NULL;
  MagickPixelPacket   pixel;

  size_t  width = MagickGetImageWidth(wand);
  size_t  height = MagickGetImageHeight(wand);

  iterator = NewPixelIterator(wand);

  for (size_t y = 0; y < height; ++y) {
    pixels = PixelGetNextIteratorRow(iterator, &width);

    for (size_t x = 0; x < width; ++x) {
      PixelGetMagickColor(pixels[x], &pixel);
      *data = 1.0 - ((nfloat_t) pixel.red / 65535.0) * 2.0;
      ++data;
    }
  }

  DestroyPixelIterator(iterator);
}
