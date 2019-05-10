#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "quicksort.h"

#define RAM 5
#define NUM_NUMS 22
#define NUM_CAMINHOS 3
#define VALOR_GRANDE 99999

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

// RETORNAR MENOR VALOR ENTRE OS ARQUIVOS E INCREMENTAR O PONTEIRO
int ler_menor_arquivos(int deslocamento_base, int arquivo_base, int *deslocamento_1, int *deslocamento_2, int *deslocamento_3, FILE **arqstemp, char *nome_arq){

  int conteudo_1, conteudo_2, conteudo_3, menor = VALOR_GRANDE;
  int *arq_menor;

  // FAZER COM QUE OS ARQUIVOS LEIAM NAS POSIÇÕES CORRETAS SE NÃO TIVEREM CHEGADO AO FINAL
  if(!feof(arqstemp[arquivo_base])) fseek(arqstemp[arquivo_base], (deslocamento_base + *deslocamento_1) * sizeof(int), 0);
  if(!feof(arqstemp[arquivo_base+1])) fseek(arqstemp[arquivo_base + 1], (deslocamento_base + *deslocamento_2) * sizeof(int), 0);
  if(!feof(arqstemp[arquivo_base+2])) fseek(arqstemp[arquivo_base + 2], (deslocamento_base + *deslocamento_3) * sizeof(int), 0);

  

  printf("Deslocamento %i %i %i\n", *deslocamento_1, *deslocamento_2, *deslocamento_3);
  // CONTEUDO PRIMEIRO ARQUIVO
  if(*deslocamento_1 < 5 && !feof(arqstemp[arquivo_base])){
    fread(&conteudo_1, sizeof(int), 1, arqstemp[arquivo_base]);
    printf("0: %i\n",conteudo_1);
    if(conteudo_1 < menor){
      menor = conteudo_1;
      arq_menor = deslocamento_1;
    }

  }
  // CONTEUDO SEGUNDO ARQUIVO
  if(*deslocamento_2 < 5 && !feof(arqstemp[arquivo_base+1])){
    fread(&conteudo_2, sizeof(int), 1, arqstemp[arquivo_base + 1]);
    printf("1: %i\n",conteudo_2);
    if(conteudo_2 < menor){
      menor = conteudo_2;
      arq_menor = deslocamento_2;
    }

  }
  // CONTEUDO TERCEIRO ARQUIVO
  if(*deslocamento_3 < 5 && !feof(arqstemp[arquivo_base+2])){
    fread(&conteudo_3, sizeof(int), 1, arqstemp[arquivo_base + 2]);
    printf("2: %i\n",conteudo_3);
    if(conteudo_3 < menor){
      menor = conteudo_3;
      arq_menor = deslocamento_3;
    }
  }


  // ERRO
  if (menor == VALOR_GRANDE) return -1;
  printf("Menor: %i\n\n",menor);
  // ANDA O PONTEIRO DO ARQUIVO MENOR
  (*arq_menor)++;

  

  if(feof(arqstemp[arquivo_base])) printf("ARQUIVO 3 ACABOU!\n");
  if(feof(arqstemp[arquivo_base+1])) printf("ARQUIVO 4 ACABOU!\n");
  if(feof(arqstemp[arquivo_base+2])) printf("ARQUIVO 5 ACABOU!\n");

  // RETORNA MENOR VALOR
  return menor;
}

void intercalacao_balanceada(int num_caminhos, FILE **arqstemp, char *nome_arq){

  int buffer[RAM], lidos, tamanho_memoria = 0, novos_arquivos, auxiliar = 0, indice_arquivo, idx_novos = 0;
  int deslocamento_base = 0;
  int i_1 = 0, i_2 = 0, i_3 = 0; // POSIÇÕES QUE ESTÃO SENDO LIDAS

  // ARQUIVOS JA EXISTENTES
  abre_arqs_temp(0,num_caminhos,nome_arq,arqstemp,"rb");
  // NOVOS ARQUIVOS (3 a 5)
  abre_arqs_temp(num_caminhos, num_caminhos*2, nome_arq, arqstemp, "wb");
  auxiliar = 0;
  novos_arquivos = 3;

  while(idx_novos < 5){ // RODADAS DE ORDENAÇÃO TENHO QUE ARRUMAR (5 PQ TO USANDO 25 NUMEROS E 25/RAM = 5)
    while(tamanho_memoria < RAM){
      buffer[tamanho_memoria] = ler_menor_arquivos(deslocamento_base, 0, &i_1, &i_2, &i_3, arqstemp, nome_arq);
      tamanho_memoria++;
    } // BUFFER CHEIO (5 números)

    indice_arquivo = num_caminhos + auxiliar;
    fwrite(buffer, sizeof(int)*RAM, 1, arqstemp[indice_arquivo]);
    // ATUALIZAR INFORMACOES PRA PROXIMO ARQUIVO
    idx_novos++; // CONTAR MAIS UMA RODADA FEITA - TENHO QUE ARRUMAR ISSO PQ NAO PODE SER SEMPRE <5
    auxiliar++; // PRÓXIMO ARQUIVO
    auxiliar = auxiliar % num_caminhos; // LIMITAR ARQUIVOS ( 0 1 2 0 1 2 0 )
    printf("Buffer cheio! Passamos para a proxima fita: %i\tIndice do arquivo: %i\n\n",auxiliar, auxiliar+num_caminhos);
    if(auxiliar == 0){ // AUX == 0 SIGNIFICA QUE RETORNOU A FITA INICIAL
      deslocamento_base += 5;
      i_1 = 0;
      i_2 = 0;
      i_3 = 0;
    }
    tamanho_memoria = 0; // ZERA MEMORIA (buffer)
  }




  fecha_arqs(num_caminhos*2,arqstemp);
}

void distribui(int num_caminhos, char *nome_arq){
  FILE *arq = fopen(nome_arq,"rb");
  FILE **arqstemp = malloc(sizeof(FILE *) * num_caminhos);
  int buffer[RAM], i=0, lidos;

  abre_arqs_temp(0,num_caminhos,nome_arq,arqstemp,"wb"); //abre args temporarios

  while( RAM == (lidos = fread(&buffer, sizeof(int), RAM, arq))){ // le o arquivo de entrada para a RAM
    quick_sort(buffer,RAM); // ordena
    fwrite(buffer, sizeof(int)*RAM, 1, arqstemp[i]); // escreve para o arquivo temporario atual
    i++;
    i %= num_caminhos; // incrementa o arquivo temporario
  }
  // trata os ultimos numeros do arquivo
  quick_sort(buffer,lidos); // ordena
  fwrite(buffer, sizeof(int)*lidos, 1, arqstemp[i]); // escreve para o arquivo temporario atual
  fecha_arqs(num_caminhos, arqstemp);

  // TODO
  intercalacao_balanceada(num_caminhos, arqstemp, nome_arq);
}

int main(){
  //srand(time(NULL));
  char nome_arq[]="teste.arq";
  char *nome_arq_temp = malloc(sizeof(char)*strlen(nome_arq)+3); // suporta ate .99 arquivos
  cria_arq_rand(nome_arq, NUM_NUMS);
  distribui(NUM_CAMINHOS, nome_arq);
  for (int i=0; i<NUM_CAMINHOS*2; i++){
    sprintf(nome_arq_temp, "%s.%d", nome_arq, i);
    printf("------- %s:\n", nome_arq_temp);
    le_arq(nome_arq_temp);
  }
}
