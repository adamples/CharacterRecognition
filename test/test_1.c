#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "net.h"
/** Dla słusznej sprawy testu */
#include "net_private.h"


  /**
   * Celem testu jest sprawdzenie, czy metoda ucząca nie zmienia wag w sieci,
   * jeśli wynik jej działania jest taki sam, jak wynik pożądany.
   */


#define A 100.0



int main(int argc, char **argv)
{
  const net_desc_t network = {
    .layers_n = 3,
    .neurons_n = (int[]) { 2, 2, 2 },
    .a = A
  };
  net_t     *net = NULL;
  nfloat_t  input[2] = { -0.4, 0.9 };
  nfloat_t  output[2] = { 1.0, -1.0 };

  net = net_create(&network);

  /** Wagi są ustawiane na znane wartości */
  net->w[1][0][0] = -0.90809536348474;
  net->w[1][0][1] =  0.37515224021634;
  net->w[1][0][2] = 0;
  net->w[1][1][0] = -0.80302682789184;
  net->w[1][1][1] = -0.99203876964377;
  net->w[1][1][2] = 0;
  net->w[2][0][0] =  0.48382498020484;
  net->w[2][0][1] = -0.96482618803383;
  net->w[2][0][2] = 0;
  net->w[2][1][0] = -0.91950458750106;
  net->w[2][1][1] = -0.68238289292338;
  net->w[2][1][2] = 0;

  net_run(net, input, output);

  printf("weights:\n");

  for (int m = 1; m < network.layers_n; ++m)
    for (int j = 0; j < network.neurons_n[m]; ++j)
      for (int i = 0; i <= network.neurons_n[m - 1]; ++i)
        printf("  net->w[%d][%d][%d] = %f\n", m, j, i, net->w[m][j][i]);

  printf("output: %f %f\n", output[0], output[1]);

  net_learn(net, 0.1, input, output);

  for (int m = 1; m < network.layers_n; ++m)
    for (int j = 0; j < network.neurons_n[m]; ++j)
      for (int i = 0; i <= network.neurons_n[m - 1]; ++i)
        printf("  net->w[%d][%d][%d] = %f\n", m, j, i, net->w[m][j][i]);

  assert(net->w[1][0][0] == -0.90809536348474);
  assert(net->w[1][0][1] ==  0.37515224021634);
  assert(net->w[1][0][2] == 0);
  assert(net->w[1][1][0] == -0.80302682789184);
  assert(net->w[1][1][1] == -0.99203876964377);
  assert(net->w[1][1][2] == 0);
  assert(net->w[2][0][0] ==  0.48382498020484);
  assert(net->w[2][0][1] == -0.96482618803383);
  assert(net->w[2][0][2] == 0);
  assert(net->w[2][1][0] == -0.91950458750106);
  assert(net->w[2][1][1] == -0.68238289292338);
  assert(net->w[2][1][2] == 0);

  return 0;
}
