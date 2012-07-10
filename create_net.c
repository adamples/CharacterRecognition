#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "net.h"


void
print_help()
{
    fprintf(stderr, "Użycie: create_net plik_danych plik_sieci a m{2} ... m{n - 1}\n");
    fprintf(stderr, "  plik_danych     - plik z danymi treningowymi, używany do wyznaczenia\n");
    fprintf(stderr, "%20cliczby neuronów w warstwach wejściowej oraz wyjściowej\n", ' ');
    fprintf(stderr, "  plik_sieci      - plik, do którego zostaną zapisane współczynniki sieci\n");
    fprintf(stderr, "  a               - współczynnik funkcji aktywacji\n");
    fprintf(stderr, "  m{x}            - liczba neuronów w warstwach ukrytych\n");
}


int 
main(int argc, char **argv)
{
  net_desc_t  net_desc = {
    .layers_n = 0,
    .neurons_n = NULL,
    .a = 1.0
  };
  FILE      *net_file = NULL;
  FILE      *data_file = NULL;
  net_t     *net = NULL;
  int       i = 0;

  srand(time(NULL));

  if (argc < 4) {
    print_help();
    return 1;
  }

  net_desc.layers_n = argc - 2;
  net_desc.neurons_n = (int *) malloc(net_desc.layers_n * sizeof(int));
  printf("layers_n = %d\n", net_desc.layers_n);

  for (i = 1; i < net_desc.layers_n - 1; ++i) {
    
    if (sscanf(argv[i + 3], "%d", &(net_desc.neurons_n[i])) != 1) {
      fprintf(stderr, "Nieprawidłowy format liczby neuronów w warstwie %d (\"%s\")\n", 
        i + 1, argv[i + 3]);
      return 2;
    }
     
    if (net_desc.neurons_n[i] < 2) {
      fprintf(stderr, "Nieprawidłowa liczba neuronów w warstwie %d (%d)\n", 
        i + 1, net_desc.neurons_n[i]);
      return 3;
    }

  }
  
  if (sscanf(argv[3], "%lf", &net_desc.a) != 1) {
    fprintf(stderr, "Nieprawidłowy format współczynnika funkcji aktywacji (\"%s\")\n", argv[3]);
    return 4;
  }

  if (!(data_file = fopen(argv[1], "rb"))) {
    perror(argv[1]);
    return 5;
  }
  
  fread(&(net_desc.neurons_n[0]), sizeof(int), 1, data_file);
  fread(&(net_desc.neurons_n[net_desc.layers_n - 1]), sizeof(int), 1, data_file);
  net_desc.neurons_n[0] = net_desc.neurons_n[0] * net_desc.neurons_n[0];
  fclose(data_file);

  printf("Tworzenie sieci %d", net_desc.neurons_n[0]);
  
  for (i = 1; i < net_desc.layers_n; ++i)
    printf(" - %d", net_desc.neurons_n[i]);
    
  printf(" (A = %0.3lf)\n", net_desc.a);
      
  net = net_create(&net_desc);

  if (!(net_file = fopen(argv[2], "wb"))) {
    perror(argv[2]);
    return 6;
  }

  net_write_to_file(net, net_file);
  i = ftell(net_file);
  fclose(net_file);
  
  printf("%0.1fkB zapisano do pliku \"%s\"\n", (double) i / 1024, argv[2]);
  
  return 0;
}

