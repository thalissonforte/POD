#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "quicksort.h"

#define RAM 5
#define NUM_NUMS 20
#define VALOR_GRANDE 99999
#define ARQ_TEMP 3

// Cria um arquivo temporario chamado nome com tam numeros aleatorios.
// Os numeros gerados sÃ£o mostrados na tela.
void cria_arq_rand(char *nome, int tam){
  FILE *arq;
  int num,j=0;
  arq = fopen(nome,"wb");

  for (int i = 0; i < tam; i++) {
    num = rand()%100;
    fwrite(&num, sizeof(num), 1, arq);
    printf("%d:%d\t",j++,num);
    if (j%RAM==0)
      printf("\n");
  }
  printf("\n");
  printf("\n");
//  fflush(arq); // garante que tudo foi gravado antes de fechar
  fclose(arq);
}

// Le um aquivo com numeros inteiros e mostra-os na tela.
void le_arq(char *nome){
  FILE *arq;
  int num;
  int i=0;

  arq = fopen(nome,"rb");

  while( 1 == fread(&num,sizeof(num),1,arq)){
    printf("%d:%d\t",i++,num);
    if (i%RAM==0)
      printf("\n");
  }
  printf("\n");
  if (!feof(arq))
    printf("Houston!\n\n"); // teoricamente nunca devemos ver isso!
}

void fecha_arqs(int num, FILE **arqs){
  for (int i =0; i < num; i++){
    //fflush(arqs[i]); // garante que tudo foi gravado antes de fechar
    fclose(arqs[i]);
  }
}

void abre_arqs_temp(int inicio, int fim, char *nome_arq, FILE **arqstemp, char *como){
  char nome_arq_temp[20];
  for (int i=inicio;i<fim;i++){
    // cria nome do arq temporario e abre
    sprintf(nome_arq_temp,"%s.%d",nome_arq,i);
    arqstemp[i]=fopen(nome_arq_temp, como);
  }
}
void retirar_N_GRANDE(char *nome_arq, FILE **arqstemp,int indice,int* vetor,int tam){
    abre_arqs_temp(indice,indice+1,nome_arq,arqstemp,"w");
    fecha_arqs(3,arqstemp);
    abre_arqs_temp(indice,indice+1,nome_arq,arqstemp,"wb");
    fwrite(vetor,sizeof(int)*(tam),1,arqstemp[indice]);
    fecha_arqs(3,arqstemp);
}

void ler_vetor(char *nome_arq, FILE **arqstemp,int indice,int**vetor,int tam_Base){
    abre_arqs_temp(indice,indice+1,nome_arq,arqstemp,"rb");
    int i;
    for(i=0;i<tam_Base;i++)
        fread(&vetor[i], sizeof(int), 1, arqstemp[indice]);
    fclose(arqstemp[indice]);
}

void limpar_arquivo(char *nome_arq, FILE **arqstemp,int indice){
    abre_arqs_temp(indice,indice+1,nome_arq,arqstemp,"w");
    fclose(arqstemp[indice]);
}

void resto(char *nome_arq, FILE **arqstemp,int indice,int** vet,int i){
    limpar_arquivo(nome_arq,arqstemp,indice);
    abre_arqs_temp(indice,indice+1,nome_arq,arqstemp,"wb");
    fwrite(vet,sizeof(int)*(i),1,arqstemp[indice]);
    fclose(arqstemp[indice]);
}

void intercalacao_polifasica(FILE **arqstemp,char *nome_arq,int n_de_blocos,int fibo,int n){
    int qtd_no_blocos=2,vet_aux[fibo],aux,no_de_blocos=n_de_blocos;

    int i=0,arquivo_vazio=2,arquivo_maior=0,arquivo_menor=1;
    //para quando qtd por bloco for igual ao n de fibonacci
    while(qtd_no_blocos<=fibo){
      i=0;
      //arq existentes
        abre_arqs_temp(arquivo_maior,arquivo_maior+1,nome_arq,arqstemp,"rb");
        abre_arqs_temp(arquivo_menor,arquivo_menor+1,nome_arq,arqstemp,"rb");
      //arq vazio
        abre_arqs_temp(arquivo_vazio, arquivo_vazio+1, nome_arq, arqstemp, "wb");

      //intercalação
      int buffer[no_de_blocos*qtd_no_blocos];
      while(!feof(arqstemp[arquivo_menor])){
        if (i>=qtd_no_blocos*no_de_blocos) break;
        int j;
        for(j=0;j<fibo_ant(qtd_no_blocos);j++){
            fread(&vet_aux[j], sizeof(int), 1, arqstemp[arquivo_maior]);}
        aux=j;
        for(j;j<fibo_ant(fibo_ant(qtd_no_blocos))+aux;j++){
            fread(&vet_aux[j], sizeof(int), 1, arqstemp[arquivo_menor]);;
        }
        //ordena bloco
        quick_sort(vet_aux,qtd_no_blocos);
        j=0,aux=i;
        //coloca no buffer
        for(i;i<aux+qtd_no_blocos;i++){
          if(i<qtd_no_blocos*no_de_blocos) buffer[i] =vet_aux[j];
          j++;
        }
      }

      //escreve no arquivo vazio
      fwrite(buffer,sizeof(int)*(qtd_no_blocos*no_de_blocos),1,arqstemp[arquivo_vazio]);
      //ler dados q restaram
      aux=i;
      for(i=0;i<fibo-aux;i++)
        fread(&vet_aux[i], sizeof(int), 1, arqstemp[arquivo_maior]);

      //limpar Arquivo
      limpar_arquivo(nome_arq,arqstemp,arquivo_menor);
      //retirar elementos ja ordenados e colocar o resto
      resto(nome_arq,arqstemp,arquivo_maior,&vet_aux,i);
      //
      fecha_arqs(ARQ_TEMP,arqstemp);
      //atualiza indices
      aux=arquivo_maior;
      arquivo_maior=arquivo_vazio;
      arquivo_vazio=arquivo_menor;
      arquivo_menor=aux;
      qtd_no_blocos=fibo_proximo(qtd_no_blocos+1);
      no_de_blocos=fibo_ant(no_de_blocos);
      //fecha arq
      fecha_arqs(ARQ_TEMP,arqstemp);
    }
    ler_vetor(nome_arq,arqstemp,arquivo_maior,&vet_aux,n);
    retirar_N_GRANDE(nome_arq,arqstemp,arquivo_maior,vet_aux,n);
}



void distribui_poli(char *nome_arq,int n){
  FILE *arq = fopen(nome_arq,"rb");
  FILE **arqstemp = malloc(sizeof(FILE *) * (ARQ_TEMP));
  int fibo=fibo_proximo(n);
  int fibo_atual=fibo_ant(fibo);
  int fibo_anterior=fibo-fibo_atual;
  int buffer[fibo_atual], i=0;
  abre_arqs_temp(0,ARQ_TEMP-1,nome_arq,arqstemp,"wb"); //abre args temporarios

  //Precisa refazer para n que não é fibonacci
  fread(&buffer, sizeof(int), fibo_atual, arq); // le o arquivo de entrada para a RAM
  fwrite(buffer, sizeof(int)*fibo_atual, 1, arqstemp[i]); // escreve para o arquivo temporario atual
  fread(&buffer, sizeof(int), n-fibo_atual, arq); // restantes
  if(n!=fibo){
    int i;
    for(i=n-fibo_atual;i<fibo_anterior;i++){
        buffer[i]=VALOR_GRANDE;
    }
  }
  // trata os ultimos numeros do arquivo
  fwrite(buffer, sizeof(int), fibo_anterior, arqstemp[1]); // escreve para o arquivo temporario atual
  fecha_arqs(ARQ_TEMP-1, arqstemp);
  // TODO
  intercalacao_polifasica(arqstemp, nome_arq,fibo_anterior,fibo,n);
}



//retorna n na seq de fibo mais prox
double dmod(double x, double y) {
    return x - (int)(x/y) * y;
}

int fibo_proximo(int n){
  if (dmod(sqrt(5*pow(n,2)+4),1)==0 || dmod(sqrt(5*pow(n,2)-4),1) ==0)
    return n;
  else{
    int c = 0;
    while (1){
      c += 1;
      if (dmod(sqrt(5*pow(c+n,2)+4),1)==0 || dmod(sqrt(5*pow(n+c,2)-4),1)==0)
        return c+n;
     }
   }
}

int fibo_ant(int n){
    int  ant=0,atual=1,prox=1;
    while(prox!=n){
        ant=atual;
        atual=prox;
        prox =ant+atual;
    }
    return atual;
}


int main(){
  //srand(time(NULL));
  char nome_arq[]="teste.arq";
  char *nome_arq_temp = malloc(sizeof(char)*strlen(nome_arq)+3); // suporta ate .99 arquivos
  cria_arq_rand(nome_arq, NUM_NUMS);
  distribui_poli(nome_arq,NUM_NUMS);
  for (int i=0; i<ARQ_TEMP; i++){
    sprintf(nome_arq_temp, "%s.%d", nome_arq, i);
    printf("------- %s:\n", nome_arq_temp);
    le_arq(nome_arq_temp);
  }
}