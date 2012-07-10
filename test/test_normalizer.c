#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "net.h"
#include "img_utils.h"

int
main(void)
{
  const char  *font_face = "Georgia";
  const char  *letter = "ą";
  double      font_size = 400;
  double      width, height;
  double      origin_x, origin_y;

  MagickWand  *wand;
  DrawingWand *draw;
  PixelWand   *bgcolor;
  PixelWand   *fgcolor;
  double      *metrics;

  MagickWandGenesis();
  wand = NewMagickWand();
  draw = NewDrawingWand();

  /* Create colors */
  bgcolor = NewPixelWand();
  PixelSetHSL(bgcolor, 0, 0, 1);
  fgcolor = NewPixelWand();
  PixelSetHSL(fgcolor, 0, 0, 0);

  /* Get text metrics (with temporary image) */
  MagickNewImage(wand, 1, 1, bgcolor);
  DrawSetFontFamily(draw, font_face);
  DrawSetFontSize(draw, font_size);
  DrawSetFontStyle(draw, NormalStyle);
  DrawSetFontWeight(draw, 300);
  DrawSetFontStretch(draw, NormalStretch);
  metrics = MagickQueryFontMetrics(wand, draw, letter);

  for (int i = 0; i < 13; ++i)
    printf("metrics[%d] = %0.1lf\n", i, metrics[i]);

  width = metrics[9] - metrics[7];
  height = metrics[10] - metrics[8];
  origin_x = - metrics[7];
  origin_y = height - metrics[12] + metrics[8];

  printf("image size: %0.1lfx%0.1lf\n", width, height);
  printf("origin: (%0.1lf, %0.1lf)\n", origin_x, origin_y);
  width = floor(width + 0.5) + 2;
  height = floor(height + 0.5) + 2;
  origin_x = floor(origin_x + 0.5) + 1;
  origin_y = floor(origin_y + 0.5) + 1;

  MagickNewImage(wand, width, height, bgcolor);

  DrawSetFillColor(draw, fgcolor);
  DrawAnnotation(draw, origin_x, origin_y, (const unsigned char *) letter);
  MagickDrawImage(wand, draw);

  MagickWriteImage(wand, "debug/wand.png");
  normalize(wand, 12, 0.5);
  MagickWriteImage(wand, "debug/normalized.png");
}
