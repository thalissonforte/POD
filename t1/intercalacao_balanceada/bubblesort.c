#include <stdio.h>

bubble_sort(char *vetor, int tam){
  int trocou = 1, i;
  char aux;

  while(trocou){
    trocou = 0;
    for(i=1; i<tam; i++){
      if(vetor[i-1] > vetor[i]){
        aux = vetor[i];
        vetor[i] = vetor[i-1];
        vetor[i-1] = aux;
        trocou = 1;
      }
    }
  }
}

