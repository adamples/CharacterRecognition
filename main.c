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


#define A ((nfloat_t) 1.5)
#define N ((nfloat_t) 0.01)


nfloat_t
f(nfloat_t x)
{
  return 2.0 / (1 + exp(- A * x)) - 1.0;
}


nfloat_t
df(nfloat_t x)
{
  return 2 * A * exp(A * x) / pow(exp(A * x) + 1, 2);
}


int
rand_(int min, int max) {
  return min + (rand() % (max - min + 1));
}


void
print_input_data(nfloat_t *data, int image_size)
{
  int x, y;

  for (y = 0; y < image_size; ++y) {
    for (x = 0; x < image_size; ++x)
      if (data[image_size * y + x] < -0.5)
        printf("  ");
      else if (data[image_size * y + x] < 0.0)
        printf("··");
      else if (data[image_size * y + x] < 0.5)
        printf("hh");
      else
        printf("##");
      //~ printf("%4.1f ", data[image_size * y + x]);

    printf("\n");
  }
}


void
print_output_data(nfloat_t *data, int output_size)
{
  int i = 0;
  int output = 0;

  for (i = output_size - 1; i >= 0; --i) {
    printf("%4.1f ", data[i]);

    if (data[i] >= 0.0)
      output |= (1 << i);
  }

  printf(" (%d)\n", output);
}


void
train(net_t *net, int n, nfloat_t *set, int rows_n, int image_size, int output_size)
{
  int       i = 0, j = 0, r = 0;
  int       input_size = image_size * image_size;
  nfloat_t  *input, *output;
  int       *hash;

  input = (nfloat_t *) malloc(input_size * sizeof(nfloat_t));
  hash = (int *) malloc(rows_n * sizeof(int));

  for (i = 0; i < rows_n; ++i)
    hash[i] = i;

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

    //~ print_input_data(input, image_size);
    //~ print_output_data(output, output_size);
    net_learn(net, N, input, output);
  }
}


void
test(net_t *net, int n, nfloat_t *set, int rows_n, int image_size, int output_size)
{
  int       i = 0, j = 0, r = 0;
  int       input_size = image_size * image_size;
  nfloat_t  *input, *output, *correct_output;
  int       correct_bits = 0;
  int       correct_letters = 0;
  double    result_bits = 0.0;
  double    result_letters = 0.0;
  char      tmp;

  output = (nfloat_t *) malloc(output_size * sizeof(nfloat_t));
  n = rows_n;

  for (i = 0; i < n; ++i) {
    r = i;
    input = set + r * (input_size + output_size);
    correct_output = input + input_size;
    net_run(net, input, output);
    //~ print_input_data(input, image_size);
    //~ print_output_data(output, output_size);

    tmp = 0;

    for (j = 0; j < output_size; ++j)
      if (((output[j] >= 0) ^ (correct_output[j] >= 0)) == 0) {
        ++correct_bits;
        ++tmp;
      }

    if (tmp == output_size)
      ++correct_letters;
  }

  result_bits = (double) correct_bits * 100.0 / (output_size * n);
  result_letters = (double) correct_letters * 100.0 / n;

  printf("%12.9f %12.9f\n", result_bits, result_letters);
}


int main(int argc, char **argv)
{
  const net_desc_t network = {
    .layers_n = 4,
    .neurons_n = (int[]) { 0, 32, 16, 0 },
    .f = f,
    .df = df
  };
  net_t     *net = NULL;
  FILE      *input = NULL;
  int       image_size = 0;
  int       input_size = 0;
  int       output_size = 0;
  int       letters_n = 0;
  int       rows_n = 0;
  int       row_size = 0;
  nfloat_t  *training_set = NULL;
  int       i;

  srand(time(NULL));

  if (argc != 2)
    perror("użycie: train <plik wejściowy>"), exit(-1);

  if (!(input = fopen(argv[1], "rb")))
    perror("nie można wczytać pliku do odczytu"), exit(-2);

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

  network.neurons_n[0] = input_size;
  network.neurons_n[network.layers_n - 1] = output_size;

  net = net_create(&network);
  test(net, 1000, training_set, rows_n, image_size, output_size);

  for (i = 0; i < 1000; ++i) {
    printf("%d ", i);
    train(net, 10000, training_set, rows_n, image_size, output_size);
    test(net, 1000, training_set, rows_n, image_size, output_size);
  }

  free(training_set);
  return 0;
}
