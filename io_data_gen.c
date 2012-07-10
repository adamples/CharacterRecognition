#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include <wand/MagickWand.h>
#include "net.h"
#include "img_utils.h"


#define FONT_SIZE 200


/**
 * Generator danych treningowych
 *
 * Parametry:
 *  - rozmiar obrazu (px),
 *  - lista czcionek,
 *  - lista liter.
 */

void
generate_input(nfloat_t *input_data, int image_size, const char* family, int font_weight, int font_style, const char *letter)
{
  double      font_size = FONT_SIZE;
  double      width, height;
  double      origin_x, origin_y;

  char        fname[1024];

  MagickWand  *wand;
  DrawingWand *draw;
  PixelWand   *bgcolor;
  PixelWand   *fgcolor;
  double      *metrics;

  wand = NewMagickWand();
  draw = NewDrawingWand();

  /* Create colors */
  bgcolor = NewPixelWand();
  PixelSetHSL(bgcolor, 0, 0, 1);
  fgcolor = NewPixelWand();
  PixelSetHSL(fgcolor, 0, 0, 0);

  /* Get text metrics (with temporary image) */
  MagickNewImage(wand, 1, 1, bgcolor);
  DrawSetFontFamily(draw, family);
  DrawSetFontSize(draw, font_size);
  DrawSetFontStyle(draw, font_style);
  DrawSetFontWeight(draw, font_weight);
  DrawSetFontStretch(draw, NormalStretch);
  metrics = MagickQueryFontMetrics(wand, draw, letter);

  width = metrics[9] - metrics[7];
  height = metrics[10] - metrics[8];
  origin_x = - metrics[7];
  origin_y = height - metrics[12] + metrics[8];

  free(metrics);

  width = floor(width + 0.5) + 2;
  height = floor(height + 0.5) + 2;
  origin_x = floor(origin_x + 0.5) + 1;
  origin_y = floor(origin_y + 0.5) + 1;

  MagickNewImage(wand, width, height, bgcolor);

  DrawSetFillColor(draw, fgcolor);
  DrawAnnotation(draw, origin_x, origin_y, (const unsigned char *) letter);
  MagickDrawImage(wand, draw);
  MagickResizeImage(wand, image_size, image_size, LanczosFilter, 1.0);

  get_input_data(wand, input_data);

#ifdef DEBUG
  sprintf(fname, "debug/%s_s%d_w%d_%s.png", family, font_style, font_weight, letter);
  MagickWriteImage(wand, fname);
#endif

  DestroyPixelWand(bgcolor);
  DestroyPixelWand(fgcolor);
  DestroyDrawingWand(draw);
  DestroyMagickWand(wand);
}


void generate_output_coded(nfloat_t *output_data, int output_size, int letter_index)
{
  int  i = 0;

  for (i = 0; i < output_size; ++i)
    if ((letter_index & (1 << i)) == (1 << i))
      output_data[i] = 1.0;
    else
      output_data[i] = -1.0;
}


void generate_output_straight(nfloat_t *output_data, int output_size, int letter_index)
{
  int  i = 0;

  for (i = 0; i < output_size; ++i)
    if (i == letter_index)
      output_data[i] = 1.0;
    else
      output_data[i] = -1.0;
}


void
generate(FILE *file, int image_size, int output_size, const char *font, int font_weight, int font_style, const char *letter, int letter_index)
{
  int       input_size = image_size * image_size;
  nfloat_t  *input = NULL;
  nfloat_t  *output = NULL;

  printf("%s", letter);
  fflush(stdout);

  input = (nfloat_t *) calloc(input_size, sizeof(nfloat_t));
  output = (nfloat_t *) calloc(output_size, sizeof(nfloat_t));

  generate_input(input, image_size, font, font_weight, font_style, letter);
  generate_output_coded(output, output_size, letter_index);

  fwrite(input, sizeof(nfloat_t), input_size, file);
  fwrite(output, sizeof(nfloat_t), output_size, file);

  free(input);
  free(output);
}


int
main(int argc, char **argv)
{
  FILE  *input = NULL;
  FILE  *output = NULL;
  int   image_size = 0;
  int   letters_n = 0;
  char  *letters_line = NULL;
  char  **letters = NULL;
  int   fonts_n = 0;
  char  **fonts = NULL;
  int   output_size = 0;
  int   rows_n = 0;
  int   i, j;

  if (argc != 3)
    perror("użycie: tsg <plik wejściowy> <plik wyjściowy>"), exit(-1);

  if (!(input = fopen(argv[1], "r")))
    perror("nie można wczytać pliku do odczytu"), exit(-2);

  if (!(output = fopen(argv[2], "wb")))
    perror("nie można wczytać pliku do zapisu"), exit(-3);

  fscanf(input, "%d\n", &image_size);

  fscanf(input, "%d\n", &letters_n);
  fscanf(input, "%m[^\n]\n", &letters_line);
  letters = (char **) malloc(letters_n * sizeof(char *));

  for (i = 0; i < letters_n; ++i) {
    char *tmp = g_utf8_find_next_char(letters_line, NULL);
    letters[i] = (char *) malloc(tmp - letters_line) + 1;
    strncpy(letters[i], letters_line, tmp - letters_line);
    letters_line = tmp;
  }

  output_size = ceil(log(letters_n / 2) / log(2));

  fscanf(input, "%d\n", &fonts_n);
  fonts = (char **) malloc(fonts_n * sizeof(char *));

  for (i = 0; i < fonts_n; ++i)
    fscanf(input, "%m[^\n]\n", &(fonts[i]));

  rows_n = letters_n * fonts_n;

  /* Trochę metadanych */
  fwrite(&image_size, sizeof(image_size), 1, output);
  fwrite(&output_size, sizeof(output_size), 1, output);
  fwrite(&letters_n, sizeof(letters_n), 1, output);
  fwrite(&rows_n, sizeof(rows_n), 1, output);

  MagickWandGenesis();

  /* Właściwe dane treningowe */
  for (i = 0; i < fonts_n; ++i) {

    printf("Generating for %s Regular\n", fonts[i]);
    for (j = 0; j < letters_n; ++j)
      generate(output, image_size, output_size, fonts[i], 300, NormalStyle, letters[j], j % 34);
    printf("\n");

    printf("Generating for %s Bold\n", fonts[i]);
    for (j = 0; j < letters_n; ++j)
      generate(output, image_size, output_size, fonts[i], 600, NormalStyle, letters[j], j % 34);
    printf("\n");

    printf("Generating for %s Italic\n", fonts[i]);
    for (j = 0; j < letters_n; ++j)
      generate(output, image_size, output_size, fonts[i], 300, ItalicStyle, letters[j], j % 34);
    printf("\n");

    printf("Generating for %s Bold Italic\n", fonts[i]);
    for (j = 0; j < letters_n; ++j)
      generate(output, image_size, output_size, fonts[i], 600, ItalicStyle, letters[j], j % 34);
    printf("\n");
  }

  fclose(input);
  fclose(output);

  return 0;
}
