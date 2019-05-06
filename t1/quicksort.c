#include<stdio.h>

void imprime_vetor_p(int *vet, int i, int j){
  for (int k=i; k<=j; k++){
    printf("%i ", vet[k]);
  }
  printf("\n");
}

void imprime_vetor(int *vet, int tam){
  imprime_vetor_p(vet, 0, tam-1);
}


void quick_sort(int vet[], int tam){
  if(tam < 2) return;

  int pivo, menor, maior, aux;

  menor = 0;
  maior = tam - 1;

  pivo = vet[menor];                                                                                                     

  while(menor <= maior){
    while(vet[menor] < pivo)
      menor++;
    while(vet[maior] > pivo)
      maior--;

    //troca 
    if ( menor <= maior ) {
        aux = vet[menor];
        vet[menor] = vet[maior];
        vet[maior] = aux;
        menor++;
        maior--;
    }
  }

  quick_sort(vet, maior+1);
  quick_sort(&vet[menor], tam-menor);
}

