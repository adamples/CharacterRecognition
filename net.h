#ifndef _NET_H_
#define _NET_H_

typedef double nfloat_t;

typedef nfloat_t(*act_func_t)(nfloat_t x);

typedef struct _net_desc_t {
  /** Liczba warstw sieci, łącznie z "warstwą" wejściową */
  int         layers_n;
  /** Liczba neuronów w warstwie, począwszy od warstwy wejściowej, kończąc na
   *  wyjściowej */
  int         *neurons_n;
  act_func_t  f;
  act_func_t  df;
} net_desc_t;

typedef nfloat_t *net_input_t;
typedef nfloat_t *net_output_t;

typedef struct _net_t net_t;


  net_t*  net_create(const net_desc_t *net_desc);
  void    net_run(net_t *net, net_input_t input, net_output_t output);
  void    net_learn(net_t *net, nfloat_t n, net_input_t input, net_output_t output);
  void    net_free(net_t **net);


#endif // _NET_H_
