#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "net.h"


void
print_input_data(nfloat_t *data, int image_size)
{
  int x, y;

  for (y = 0; y < image_size; ++y) {
    printf("   | ");

    for (x = 0; x < image_size; ++x)
      if (data[image_size * y + x] < -0.5)
        printf("..");
      else if (data[image_size * y + x] < 0.0)
        printf("oo");
      else if (data[image_size * y + x] < 0.5)
        printf("HH");
      else
        printf("##");
      //~ printf("%4.1f ", data[image_size * y + x]);

    printf(" |\n");
  }
}


void
print_output_data(nfloat_t *data, int data_size)
{
  int i;

  printf("   | ");

  for (i = data_size - 1; i >= 0; --i) {
    if (data[i] > 0)
      printf("1");
    else
      printf("0");
  }

  printf(" |\n");
}


int
main(int argc, char **argv)
{
  FILE      *input = NULL;
  int       image_size = 0;
  int       input_size = 0;
  int       output_size = 0;
  int       letters_n = 0;
  int       rows_n = 0;
  int       row_size = 0;
  nfloat_t  *training_set = NULL;
  int       i;

  if (argc != 2 && argc != 3)
    perror("użycie: io_data_inspect <plik danych uczących>"), exit(-1);

  if (!(input = fopen(argv[1], "rb")))
    perror(argv[1]), exit(-2);

  fread(&image_size, sizeof(image_size), 1, input);
  fread(&output_size, sizeof(output_size), 1, input);
  fread(&letters_n, sizeof(letters_n), 1, input);
  fread(&rows_n, sizeof(rows_n), 1, input);

  printf("image_size = %d\n", image_size);
  printf("output_size = %d\n", output_size);
  printf("letters_n = %d\n", letters_n);
  printf("rows_n = %d\n", rows_n);

  input_size = image_size * image_size;
  row_size = input_size + output_size;

  training_set = (nfloat_t *) malloc(row_size * rows_n * sizeof(nfloat_t));
  fread(training_set, row_size * sizeof(nfloat_t), rows_n, input);

  for (i = 0; i < rows_n; ++i) {
    printf("\n%d:\n\n", i);
    print_input_data(training_set + i * row_size, image_size);
    print_output_data(training_set + i * row_size + input_size, output_size);
  }

  free(training_set);
  return 0;
}
