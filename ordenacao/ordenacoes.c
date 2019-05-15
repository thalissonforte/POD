#include <stdio.h>

void main(){
    int vetor[20] = {9,3,5,10,13,0,19,18,2,1,17,15,12,4,11,7,8,6,14,16};

    //bubblesort(vetor, 20);
    quicksort(vetor, 20);
    //insercao_direta(vetor, 20);
    //shellsort(vetor, 20);
    //selecao_direta(vetor, 20);

    printf("\nIndice: %i\n\n",pesquisa_binaria(vetor, 20, 22));
    exibir(vetor, 20);

}

// PESQUISA BINARIA
int pesquisa_binaria(int *vetor, int tam, int valor){

    int sup = tam-1, inf = 0;
    int meio;

    while(inf<=sup){
        meio = (inf+sup)/2;
        if(vetor[meio] == valor) return meio;

        if(vetor[meio] > valor){
            sup = meio-1;
        }else{ 
            inf = meio+1;
        }
    }
    return -1;
}

// SELECAO DIRETA
void selecao_direta(int *vetor, int tam){
    int i, j, aux, idx_menor;

    for(i=0; i<tam-1;i++){
        idx_menor = i;
        for(j=i; j<tam; j++){
            if(vetor[j] < vetor[idx_menor]) idx_menor = j;
        }

        // TROCA
        aux = vetor[i];
        vetor[i] = vetor[idx_menor];
        vetor[idx_menor] = aux;
    }

}

// INSERCAO DIRETA
void insercao_direta(int *vetor, int tam){
    int i, j, chave;
    for(i = 1; i<tam; i++){
        chave = vetor[i];
        j = i-1;
        while((j>=0) && vetor[j]>chave){
            vetor[j+1] = vetor[j];
            j--;
        }
        vetor[j+1] = chave;
    }
}

// SHELLSORT
void shellsort(int *vetor, int tam){
    int i, j, chave, intervalo;
    for(intervalo = tam/2; intervalo>0; intervalo/=2){
        for(i = intervalo; i<tam; i++){
            chave = vetor[i];
            j = i-intervalo;
            while((j>=0) && vetor[j]>chave){
                vetor[j+intervalo] = vetor[j];
                j-=intervalo;
            }
            vetor[j+intervalo] = chave;
        }
    }
}

// QUICKSORT
void quicksort(int *vetor, int tam){
    int esq = 0, dir = tam-1;
    quicksort1(vetor, esq, dir);
}
void quicksort1(int *vetor, int esq, int dir){
    if(esq>=dir) return;

    int part = quicksort2(vetor, esq, dir);
    quicksort1(vetor, part+1, dir);
    quicksort1(vetor, esq, part-1);
}
int quicksort2(int *vetor, int esq, int dir){
    int aux;
    int pivo = vetor[esq];

    while(esq < dir){
        while(vetor[esq] < pivo) esq++;
        while(vetor[dir] > pivo) dir--;
        aux = vetor[esq];
        vetor[esq] = vetor[dir];
        vetor[dir] = aux;
    }

    return esq;
}

// BUBBLESORT
void bubblesort(int *vetor, int tam){
    int i, aux, troca = 1;
    while(troca == 1){
        troca = 0;
        for(i = 1; i < tam; i++){
            if(vetor[i]<vetor[i-1]){
                // TROCA
                aux = vetor[i];
                vetor[i] = vetor[i-1];
                vetor[i-1] = aux;
                troca = 1;
            }
        }
    }
}

void exibir(int *vetor, int tam){
    for(int i = 0; i < tam; i++){
        printf("[%i] - %i\n", i, vetor[i]);
    }
}
