#ifndef _NET_PRIVATE_H_
#define _NET_PRIVATE_H_

struct _net_t {
  int layers_n;
  int *neurons_n;
  act_func_t f;
  act_func_t df;
  /** Tablica tablic wag dla poszczególnych warstw (w[warstwa][neuron][wejscie]) */
  nfloat_t ***w;
  /** Tablica wyjść dla poszczególnych warstw (y[warstwa][neuron]) */
  nfloat_t **y;
  /** Tablica sum dla poszczególnych warstw (phi[warstwa][neuron]) */
  nfloat_t **phi;
  /** Tablica błędów dla poszczególnych warstw (delta[warstwa][neuron]) */
  nfloat_t **delta;
};

#endif
