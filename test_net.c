#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <cairo/cairo.h>
#include "net.h"


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


nfloat_t
test(net_t *net, nfloat_t *set, int rows_n, int image_size, int output_size)
{
  int       i = 0, j = 0;
  int       input_size = image_size * image_size;
  nfloat_t  *input, *output, *expected_output;
  nfloat_t  error = 0;
  nfloat_t  other_error = 0;

  output = (nfloat_t *) malloc(output_size * sizeof(nfloat_t));

  for (i = 0; i < rows_n; ++i) {
    input = set + i * (input_size + output_size);
    expected_output = set + i * (input_size + output_size) + input_size;
    net_run(net, input, output);

    for (j = 0; j < output_size; ++j) {
      error += pow(expected_output[j] - output[j], 2);
      if ((expected_output[j] > 0) ^ (output[j] > 0))
        ++other_error;
    }

    //print_output_data(expected_output, output_size);
    //print_output_data(output, output_size);
    //printf("\n");
  }

  error /= rows_n * output_size;
  other_error = other_error / rows_n;

  return error;
}


int main(int argc, char **argv)
{
  FILE      *data_file = NULL;
  FILE      *net_file = NULL;
  net_t     *net = NULL;
  nfloat_t  *test_set = NULL;
  int       image_size = 0;
  int       input_size = 0;
  int       output_size = 0;
  int       letters_n = 0;
  int       rows_n = 0;
  int       row_size = 0;
  nfloat_t  error = 0.0;

  srand(time(NULL));

  if (argc != 3)
    fprintf(stderr, "użycie: test_net <plik we. sieci> <plik danych uczących>"), exit(-1);

  /* Wczytanie danych uczących */

  if (!(data_file = fopen(argv[2], "rb")))
    perror(argv[2]), exit(-2);

  fread(&image_size, sizeof(image_size), 1, data_file);
  fread(&output_size, sizeof(output_size), 1, data_file);
  fread(&letters_n, sizeof(letters_n), 1, data_file);
  fread(&rows_n, sizeof(rows_n), 1, data_file);

  input_size = image_size * image_size;
  row_size = input_size + output_size;

  test_set = (nfloat_t *) malloc(row_size * rows_n * sizeof(nfloat_t));
  fread(test_set, row_size * sizeof(nfloat_t), rows_n, data_file);

  fclose(data_file);

  /* Koniec wczytywania danych uczących */

  /* Wczytywanie opisu sieci */

  if (!(net_file = fopen(argv[1], "rb")))
    perror(argv[1]), exit(-3);

  net = net_create_from_file(net_file);

  fclose(net_file);

  /* Koniec wczytywania opisu sieci */

  error = test(net, test_set, rows_n, image_size, output_size);
  printf("%0.9lf\n", error);

  free(test_set);
  return 0;
}
