#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <cairo/cairo.h>
#include <glib.h>
#include "net.h"


  /**
   * Generator danych treningowych
   *
   * Parametry:
   *  - rozmiar obrazu (px),
   *  - lista czcionek,
   *  - lista liter.
   */

/*
void
generate_input(nfloat_t *input_data, int image_size, const char* family, const char *letter)
{
  cairo_surface_t       *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, image_size, image_size);
  cairo_t               *cr = cairo_create(surface);
  cairo_text_extents_t  extents;
  uint32_t              *data = NULL;
  int                   stride = 0;
  int                   i;
  char                  fname[1024];

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
  cairo_select_font_face(cr, family, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, image_size * 10);
  cairo_text_extents(cr, letter, &extents);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_scale(cr, (nfloat_t) (image_size - 1) / extents.width, (nfloat_t) (image_size - 1) / extents.height);
  cairo_translate(cr, -extents.x_bearing, -extents.y_bearing);
  cairo_text_extents(cr, letter, &extents);
  cairo_show_text(cr, letter);
  sprintf(fname, "debug/%s_%s.png", family, letter);
  cairo_surface_write_to_png(surface, fname);
  cairo_surface_flush(surface);
  data = (uint32_t *) cairo_image_surface_get_data(surface);
  stride = cairo_image_surface_get_stride(surface);
  assert(stride == image_size * 4);

  for (i = 0; i < image_size * image_size; ++i, ++data)
    input_data[i] =  - (nfloat_t) *((unsigned char *) data) / 127.5 + 1.0;

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}*/


void
generate_input(nfloat_t *input_data, int image_size, const char* family, const char *letter)
{
  cairo_surface_t       *tsurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, image_size, image_size);
  cairo_t               *tcr = cairo_create(tsurface);
  cairo_surface_t       *ssurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, image_size * 10, image_size * 10);
  cairo_t               *scr = cairo_create(ssurface);
  cairo_text_extents_t  extents;
  uint32_t              *data = NULL;
  int                   stride = 0;
  int                   i;
  char                  fname[1024];

  cairo_select_font_face(scr, family, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(scr, image_size * 10);
  cairo_text_extents(scr, letter, &extents);
  cairo_destroy(scr);
  cairo_surface_destroy(ssurface);
  ssurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, extents.width, extents.height);
  scr = cairo_create(ssurface);
  cairo_select_font_face(scr, family, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(scr, image_size * 10);
  cairo_set_source_rgb(scr, 1.0, 1.0, 1.0);
  cairo_paint(scr);
  cairo_set_source_rgb(scr, 0.0, 0.0, 0.0);
  cairo_move_to(scr, -extents.x_bearing, -extents.y_bearing);
  cairo_show_text(scr, letter);
  sprintf(fname, "debug/%s_%s.png", family, letter);
  cairo_surface_write_to_png(ssurface, fname);

  cairo_scale(tcr, (double) image_size / extents.width, (double) image_size / extents.height);
  cairo_set_source_surface(tcr, ssurface, 0, 0);
  cairo_pattern_set_filter(cairo_get_source(tcr), CAIRO_FILTER_BEST);
  cairo_paint(tcr);
  cairo_surface_write_to_png(tsurface, fname);

  /*cairo_surface_flush(surface);
  data = (uint32_t *) cairo_image_surface_get_data(surface);
  stride = cairo_image_surface_get_stride(surface);
  assert(stride == image_size * 4);

  for (i = 0; i < image_size * image_size; ++i, ++data)
    input_data[i] =  - (nfloat_t) *((unsigned char *) data) / 127.5 + 1.0;
*/
  cairo_destroy(scr);
  cairo_surface_destroy(ssurface);
  cairo_destroy(tcr);
  cairo_surface_destroy(tsurface);
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
generate(FILE *file, int image_size, int output_size, const char *font, const char *letter, int letter_index)
{
  int       input_size = image_size * image_size;
  nfloat_t  *input = NULL;
  nfloat_t  *output = NULL;

  printf("Generating for font = '%s'; char = '%s'\n", font, letter);

  input = (nfloat_t *) malloc(input_size * sizeof(nfloat_t));
  output = (nfloat_t *) malloc(output_size * sizeof(nfloat_t));

  generate_input(input, image_size, font, letter);
  generate_output_coded(output, output_size, letter_index);

  fwrite(input, sizeof(nfloat_t), input_size, file);
  fwrite(output, sizeof(nfloat_t), output_size, file);
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

  output_size = ceil(log(letters_n) / log(2));
  //output_size = letters_n;

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

  /* Właściwe dane treningowe */
  for (i = 0; i < fonts_n; ++i)
    for (j = 0; j < letters_n; ++j)
      generate(output, image_size, output_size, fonts[i], letters[j], j);

  fclose(input);
  fclose(output);

  return 0;
}
