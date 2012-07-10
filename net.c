#include "net.h"
#include "net_private.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>


nfloat_t net_f(nfloat_t a, nfloat_t x)
{
  return 2.0 / (1 + exp(- a * x)) - 1.0;
}


nfloat_t net_df(nfloat_t a, nfloat_t x)
{
  return 2 * a * exp(a * x) / pow(exp(a * x) + 1, 2);
}


net_t*
net_allocate(const net_desc_t *net_desc)
{
  net_t *net = NULL;
  int   m = 0, j = 0;

  net = (net_t *) malloc(sizeof(net_t));
  net->layers_n = net_desc->layers_n;
  net->neurons_n = net_desc->neurons_n;
  net->a = net_desc->a;

  /* Alokacja pamięci dla wag, wymaga obliczenia liczby wag */
  int w_n = 0;

  for (m = 1; m < net_desc->layers_n; ++m)
    w_n += net_desc->neurons_n[m] * (net_desc->neurons_n[m - 1] + 1);

  nfloat_t *w = (nfloat_t *) malloc(sizeof(nfloat_t) * w_n);

  /* Tablica wag -- warstwy */
  net->w = (nfloat_t ***) malloc(sizeof(nfloat_t **) * net_desc->layers_n);

  /* Tablica wag -- neurony */
  for (m = 1; m < net_desc->layers_n; ++m) {
    net->w[m] = (nfloat_t **) malloc(sizeof(nfloat_t *) * net_desc->neurons_n[m]);

    for (j = 0; j < net_desc->neurons_n[m]; ++j) {
      net->w[m][j] = w;
      w += net_desc->neurons_n[m - 1] + 1;
    }
  }

  /* Alokacja pamięci dla wyjść */

  int y_n = 0;

  for (m = 0; m < net_desc->layers_n; ++m)
    y_n += net_desc->neurons_n[m];

  net->phi = (nfloat_t **) malloc(sizeof(nfloat_t *) * net_desc->layers_n);
  net->phi[0] = (nfloat_t *) malloc(sizeof(nfloat_t) * y_n);
  net->delta = (nfloat_t **) malloc(sizeof(nfloat_t *) * net_desc->layers_n);
  net->delta[0] = (nfloat_t *) malloc(sizeof(nfloat_t) * y_n);
  net->y = (nfloat_t **) malloc(sizeof(nfloat_t *) * net_desc->layers_n);
  net->y[0] = (nfloat_t *) malloc(sizeof(nfloat_t) * y_n);

  for (m = 1; m < net_desc->layers_n; ++m) {
    net->y[m] = net->y[m - 1] + net_desc->neurons_n[m - 1];
    net->phi[m] = net->phi[m - 1] + net_desc->neurons_n[m - 1];
    net->delta[m] = net->delta[m - 1] + net_desc->neurons_n[m - 1];
  }

  /* Test
  for (m = 1; m < net->layers_n; ++m)
    for (j = 0; j < net->neurons_n[m]; ++j)
      for (i = 0; i <= net->neurons_n[m - 1]; ++i)
        net->w[m][j][i] = m + j * 100 + i * 100000;

  for (m = 1; m < net->layers_n; ++m)
    for (j = 0; j < net->neurons_n[m]; ++j)
      for (i = 0; i <= net->neurons_n[m - 1]; ++i)
        assert(net->w[m][j][i] == m + j * 100 + i * 100000);

  for (m = net->layers_n - 1; m > 0; --m)
    for (j = net->neurons_n[m] - 1; j >= 0; --j)
      for (i = net->neurons_n[m - 1]; i >= 0; --i)
        net->w[m][j][i] = m + j * 100 + i * 100000;

  for (m = 1; m < net->layers_n; ++m)
    for (j = 0; j < net->neurons_n[m]; ++j)
      for (i = 0; i <= net->neurons_n[m - 1]; ++i)
        assert(net->w[m][j][i] == m + j * 100 + i * 100000);
     Koniec testu */

  return net;
}


void
net_initialize_random(net_t *net)
{
  int i = 0, j = 0, m = 0;

  for (m = 1; m < net->layers_n; ++m)
    for (j = 0; j < net->neurons_n[m]; ++j)
      for (i = 0; i <= net->neurons_n[m - 1]; ++i)
        net->w[m][j][i] = 2.0 * (nfloat_t) rand() / RAND_MAX - 1.0;
}


net_t*
net_create(const net_desc_t *net_desc)
{
  net_t   *net = net_allocate(net_desc);

  net_initialize_random(net);
  return net;
}


net_t*
net_create_from_file(FILE *file)
{
  net_desc_t  net_desc = {
    .layers_n = 0,
    .neurons_n = NULL,
    .a = 1.0
  };
  net_t   *net = NULL;
  int     m = 0;

  fread(&net_desc.layers_n, sizeof(int), 1, file);
  net_desc.neurons_n = (int *) malloc(net_desc.layers_n * sizeof(int));
  fread(net_desc.neurons_n, sizeof(int), net_desc.layers_n, file);
  fread(&net_desc.a, sizeof(nfloat_t), 1, file);

  net = net_allocate(&net_desc);

  int w_n = 0;

  for (m = 1; m < net->layers_n; ++m)
    w_n += net->neurons_n[m] * (net->neurons_n[m - 1] + 1);

  fread(net->w[1][0], sizeof(nfloat_t), w_n, file);

  return net;
}


void
net_write_to_file(net_t *net, FILE* file)
{
  int w_n = 0, m = 0;

  for (m = 1; m < net->layers_n; ++m)
    w_n += net->neurons_n[m] * (net->neurons_n[m - 1] + 1);

  fwrite(&net->layers_n, sizeof(int), 1, file);
  fwrite(net->neurons_n, sizeof(int), net->layers_n, file);
  fwrite(&net->a, sizeof(nfloat_t), 1, file);
  fwrite(net->w[1][0], sizeof(nfloat_t), w_n, file);
}


void
net_compute(net_t *net)
{
  int   m = 0, j = 0, i = 0;

  for (m = 1; m < net->layers_n; ++m)
    for (j = 0; j < net->neurons_n[m]; ++j) {
      net->phi[m][j] = net->w[m][j][net->neurons_n[m - 1]];

      for (i = 0; i < net->neurons_n[m - 1]; ++i)
        net->phi[m][j] += net->y[m - 1][i] * net->w[m][j][i];

      net->y[m][j] = net_f(net->a, net->phi[m][j]);
    }
}


void
net_run(net_t *net, net_input_t input, net_output_t output)
{
  //memcpy(net->y[0], input, net->neurons_n[0] * sizeof(nfloat_t));
  net->y[0] = input;
  net->y[net->layers_n - 1] = output;
  net_compute(net);
  //memcpy(output, net->y[net->layers_n - 1], net->neurons_n[net->layers_n - 1] * sizeof(nfloat_t));
}


void
net_learn(net_t *net, nfloat_t n, net_input_t input, net_output_t output)
{
  int m = 0, j = 0, i = 0, l = 0;

  memcpy(net->y[0], input, net->neurons_n[0] * sizeof(nfloat_t));
  net_compute(net);

  /* Obliczanie błędów dla warstwy wyjściowej */
  m = net->layers_n - 1;

  for (j = 0; j < net->neurons_n[m]; ++j)
    net->delta[m][j] = net_df(net->a, net->phi[m][j]) * (output[j] - net->y[m][j]);

  /* Obliczanie błędów dla pozostałych warstw */
  for (m = net->layers_n - 2; m > 0; --m)
    for (j = 0; j < net->neurons_n[m]; ++j) {
      net->delta[m][j] = 0;

      for (l = 0; l < net->neurons_n[m + 1]; ++l)
        net->delta[m][j] += net->delta[m + 1][l] * net->w[m + 1][l][j];

      net->delta[m][j] *= net_df(net->a, net->phi[m][j]);
    }

  /* Korekta wag */
  for (m = 1; m < net->layers_n; ++m)
    for (j = 0; j < net->neurons_n[m]; ++j) {
      for (i = 0; i < net->neurons_n[m - 1]; ++i)
        net->w[m][j][i] += n * net->delta[m][j] * net->y[m - 1][i];

      /* Korekta wagi biasu */
      net->w[m][j][net->neurons_n[m - 1]] += n * net->delta[m][j];
    }
}

