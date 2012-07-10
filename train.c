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


int
rand_(int min, int max) {
  return min + (rand() % (max - min + 1));
}


void
train(net_t *net, nfloat_t *set, int rows_n, int image_size, int output_size, nfloat_t n)
{
  int       i = 0, j = 0, r = 0;
  int       input_size = image_size * image_size;
  nfloat_t  *input, *output;
  int       *hash;

  input = (nfloat_t *) malloc(input_size * sizeof(nfloat_t));
  hash = (int *) malloc(rows_n * sizeof(int));

  for (i = 0; i < rows_n; ++i)
    hash[i] = i;

	/* Knuth shuffle */
  for (i = 0; i < rows_n; ++i) {
    r = rand_(0, rows_n - 1);
    j = hash[i];
    hash[i] = hash[r];
    hash[r] = j;
  }

  for (i = 0; i < rows_n; ++i) {
    r = hash[i];
    //input = set + r * (input_size + output_size);
    memcpy(input, set + r * (input_size + output_size), input_size * sizeof(nfloat_t));

    /* Zakłócenia */
    for (j = 0; j < input_size; ++j) {
      input[j] += ((nfloat_t) rand() / RAND_MAX) * 0.2 + 0.1;
      if (input[j] < -1) input[j] = -1;
      if (input[j] > 1) input[j] = 1;
    }

    for (j = 0; j < rand_(0, 5); ++j)
      input[rand_(0, input_size - 1)] = rand_(0, 1) == 0 ? 1.0 : -1.0;

    output = set + r * (input_size + output_size) + input_size;

    net_learn(net, n, input, output);
  }
}


int main(int argc, char **argv)
{
  FILE      *data_file = NULL;
  FILE      *net_file = NULL;
  net_t     *net = NULL;
  nfloat_t  *training_set = NULL;
  int       image_size = 0;
  int       input_size = 0;
  int       output_size = 0;
  int       letters_n = 0;
  int       rows_n = 0;
  int       row_size = 0;
  nfloat_t	n = 0;

  srand(time(NULL));

  if (argc != 5)
    fprintf(stderr, "użycie: train <plik we. sieci> <plik wy. sieci> <plik danych uczących> <wsp. uczenia>"), exit(-1);

  if (sscanf(argv[4], "%lf", &n) != 1)
		fprintf(stderr, "Nieprawidłowy współczynnik uczenia\n"), exit(-1);

  /* Wczytanie danych uczących */

  if (!(data_file = fopen(argv[3], "rb")))
    perror(argv[3]), exit(-2);

  fread(&image_size, sizeof(image_size), 1, data_file);
  fread(&output_size, sizeof(output_size), 1, data_file);
  fread(&letters_n, sizeof(letters_n), 1, data_file);
  fread(&rows_n, sizeof(rows_n), 1, data_file);

  input_size = image_size * image_size;
  row_size = input_size + output_size;

  training_set = (nfloat_t *) malloc(row_size * rows_n * sizeof(nfloat_t));
  fread(training_set, row_size * sizeof(nfloat_t), rows_n, data_file);

  fclose(data_file);

  /* Koniec wczytywania danych uczących */

  /* Wczytywanie opisu sieci */

  if (!(net_file = fopen(argv[1], "rb")))
    perror(argv[1]), exit(-3);

  net = net_create_from_file(net_file);

  fclose(net_file);

  /* Koniec wczytywania opisu sieci */

  train(net, training_set, rows_n, image_size, output_size, n);

  if (!(net_file = fopen(argv[2], "wb")))
    perror(argv[2]), exit(-4);

  net_write_to_file(net, net_file);

  fclose(net_file);

  free(training_set);
  return 0;
}
