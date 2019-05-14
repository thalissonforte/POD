#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "quicksort.h"

#define RAM 5
#define NUM_NUMS 16
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
  for (int i = 0; i < num; i++){
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

// VERIFICAR FINAL DO ARQUIVO
int final_arquivo(FILE* arqtemp, int byte){
  int valor_1, byte_pos_ler;
  // SERVEM SO PRA PULAR O PONTEIRO
  fread(&valor_1, sizeof(int), 1, arqtemp);
  fread(&valor_1, sizeof(int), 1, arqtemp);
  byte_pos_ler = ftell(arqtemp); // BUSCA A ATUAL DISTANCIA DO COMEÇO
  if(byte == byte_pos_ler) return 1; // FINAL DO ARQUIVO
  fseek(arqtemp, byte, 0); // RETORNAR AO INICIO
  return 0; // NÃO CHEGOU AO FINAL
}

// RETORNAR MENOR VALOR ENTRE OS ARQUIVOS E INCREMENTAR O PONTEIRO
int ler_menor_arquivos(int deslocamento_base, int arquivo_base, int *deslocamento_1, int *deslocamento_2, int *deslocamento_3, FILE **arqstemp, char *nome_arq, int tamanho_divisao){

  int conteudo_1, conteudo_2, conteudo_3, menor = VALOR_GRANDE;
  int *arq_menor;
  int final_arquivo_1 = 0, final_arquivo_2 = 0, final_arquivo_3 = 0;

  // FAZER COM QUE OS ARQUIVOS LEIAM NAS POSIÇÕES CORRETAS SE NÃO TIVEREM CHEGADO AO FINAL
  fseek(arqstemp[arquivo_base], (deslocamento_base + *deslocamento_1) * sizeof(int), 0);
  fseek(arqstemp[arquivo_base + 1], (deslocamento_base + *deslocamento_2) * sizeof(int), 0);
  fseek(arqstemp[arquivo_base + 2], (deslocamento_base + *deslocamento_3) * sizeof(int), 0);

  // ARQUIVOS CHEGARAM AO FINAL / PROBLEMA NO EOF
  final_arquivo_1 = final_arquivo(arqstemp[arquivo_base], ftell(arqstemp[arquivo_base]));
  final_arquivo_2 = final_arquivo(arqstemp[arquivo_base+1], ftell(arqstemp[arquivo_base+1]));
  final_arquivo_3 = final_arquivo(arqstemp[arquivo_base+2], ftell(arqstemp[arquivo_base+2]));

  // CONTEUDO PRIMEIRO ARQUIVO
  if(*deslocamento_1 < tamanho_divisao && !final_arquivo_1){
    fread(&conteudo_1, sizeof(int), 1, arqstemp[arquivo_base]);
    if(conteudo_1 < menor){
      menor = conteudo_1;
      arq_menor = deslocamento_1;
    }
  }
  // CONTEUDO SEGUNDO ARQUIVO
  if(*deslocamento_2 < tamanho_divisao && !final_arquivo_2){
    fread(&conteudo_2, sizeof(int), 1, arqstemp[arquivo_base + 1]);
    if(conteudo_2 < menor){
      menor = conteudo_2;
      arq_menor = deslocamento_2;
    }
  }
  // CONTEUDO TERCEIRO ARQUIVO
  if(*deslocamento_3 < tamanho_divisao && !final_arquivo_3){
    fread(&conteudo_3, sizeof(int), 1, arqstemp[arquivo_base + 2]);
    if(conteudo_3 < menor){
      menor = conteudo_3;
      arq_menor = deslocamento_3;
    }
  }

  // ERRO
  if (menor == VALOR_GRANDE) return -1;
  // ANDA O PONTEIRO DO ARQUIVO MENOR
  (*arq_menor)++;
  // RETORNA MENOR VALOR
  return menor;
}

int intercala_arquivos(int arquivo_inicial, int arquivo_final, char* nome_arq, FILE** arqstemp, int quantia_a_criar, int tamanho_divisao){

  int total_lidos = 0, tamanho_memoria = 0;
  int buffer[RAM];
  int i_1 = 0, i_2 = 0, i_3 = 0;
  int deslocamento_base = 0, arquivo_base;
  int indice_arquivo = 0; // INDICE ONDE VAI SER ESCRITO
  int idx_arquivo_atual = 0;
  int arquivos_criados = 1;

  int quantia_arquivos_base = arquivo_final - arquivo_inicial;

  if(arquivo_final - arquivo_inicial == 0){
    // ERRO
    return -1;
  }

  arquivo_base = arquivo_inicial;
  // ABRIR ARQUIVOS
  abre_arqs_temp(0, arquivo_final, nome_arq, arqstemp, "rb");
  // CRIAR NOVOS ARQUIVOS
  abre_arqs_temp(arquivo_final, arquivo_final+quantia_a_criar, nome_arq, arqstemp, "wb");

  while(total_lidos < NUM_NUMS){ // ENQUANTO AINDA EXISTIREM VALORES PRA ORDENAR / RODADAS DE ORDENAÇÃO

    while(total_lidos < NUM_NUMS && tamanho_memoria < RAM && (buffer[tamanho_memoria] = ler_menor_arquivos(deslocamento_base, arquivo_base, &i_1, &i_2, &i_3, arqstemp, nome_arq, tamanho_divisao)) != -1){
      tamanho_memoria++;
      total_lidos++;
    } // BUFFER CHEIO (5 números)

    // GUARDAR VALORES DO BUFFER
    indice_arquivo = arquivo_final + idx_arquivo_atual; // ( 3 4 5 3 4 5)
    // CERTEZA DO PONTEIRO CORRETO
    fseek(arqstemp[indice_arquivo], 0, SEEK_END);
    fwrite(buffer, sizeof(int)*tamanho_memoria, 1, arqstemp[indice_arquivo]);

    if(quantia_arquivos_base < 2) i_2 = tamanho_divisao;
    if(quantia_arquivos_base < 3) i_3 = tamanho_divisao;

    // ATUALIZAR INFORMACOES PRA PROXIMO ARQUIVO
    if((i_1 == tamanho_divisao && i_2 == tamanho_divisao && i_3 == tamanho_divisao)){ // TROCAR DE FITA
      idx_arquivo_atual++; // PROXIMO ARQUIVO
      idx_arquivo_atual = idx_arquivo_atual % quantia_a_criar; // LIMITAR OS ARQUIVOS ( 0 1 2 0 1 2 0 )
      deslocamento_base += tamanho_divisao; // ESSES VALORES JA FORAM LIDOS, PASSAR PRA SEGUNDA LINHA DOS ARQUIVOS
      // ZERAR OS DESLOCAMENTOS
      i_1 = 0;
      i_2 = 0;
      i_3 = 0;
      arquivos_criados++;
    }
    tamanho_memoria = 0; // ZERA MEMORIA (buffer)
  }


  fecha_arqs(arquivo_final+quantia_a_criar, arqstemp);
  return quantia_a_criar;
}

int intercalacao_balanceada(int num_caminhos, FILE **arqstemp, char *nome_arq){
  int arquivos_existentes = num_caminhos; // 3
  int total_arquivos = num_caminhos;

  int num_arquivos_criados;
  int tamanho_divisao = RAM; // FITAS ESTAO DIVIDIDAS DE ACORDO COM A RAM ( 5 EM 5 )

  int arquivo_base_inicial = 0, arquivo_base_final = num_caminhos;
  int quantia_a_criar = 3;

  while(tamanho_divisao < NUM_NUMS){ // ITERAR ATÉ FORMAR 1 FITA APENAS
    // INTERCALA OS ARQUIVOS
    num_arquivos_criados = intercala_arquivos(arquivo_base_inicial, arquivo_base_final, nome_arq, arqstemp, quantia_a_criar, tamanho_divisao);
    arquivo_base_inicial = arquivo_base_final;
    arquivo_base_final += quantia_a_criar;
    total_arquivos += quantia_a_criar;

    // ATUALIZA O TAMANHO DAS DIVISOES DE ACORDO COM A QUANTIA DE ARQUIVOS CRIADOS
    tamanho_divisao += tamanho_divisao * quantia_a_criar;
  }
  num_arquivos_criados = intercala_arquivos(arquivo_base_inicial, arquivo_base_inicial+quantia_a_criar, nome_arq, arqstemp, 1, tamanho_divisao);
  total_arquivos++;

  //fecha_arqs(total_arquivos,arqstemp);

  return total_arquivos;

}

int distribui(int num_caminhos, char *nome_arq){
  FILE *arq = fopen(nome_arq,"rb");

  FILE **arqstemp = malloc(sizeof(FILE *) * num_caminhos * RAM);
  int buffer[RAM], i=0, lidos;

  abre_arqs_temp(0,num_caminhos,nome_arq,arqstemp,"wb"); //abre args temporarios

  while( RAM == (lidos = fread(&buffer, sizeof(int), RAM, arq))){ // le o arquivo de entrada para a RAM
    quick_sort(buffer,RAM); // ordena
    fwrite(buffer, sizeof(int)*RAM, 1, arqstemp[i]); // escreve para o arquivo temporario atual
    i++;
    i %= num_caminhos; // incrementa o arquivo temporario
  }
  // trata os ultimos numeros do arquivo (%5)
  quick_sort(buffer,lidos); // ordena
  fwrite(buffer, sizeof(int)*lidos, 1, arqstemp[i]); // escreve para o arquivo temporario atual
  fecha_arqs(num_caminhos, arqstemp);

  // INTERCALA BALANCEADO
  return intercalacao_balanceada(num_caminhos, arqstemp, nome_arq);
}

int main(){

  char nome_arq[]="teste.arq";
  char *nome_arq_temp = malloc(sizeof(char)*strlen(nome_arq)+3); // suporta ate .99 arquivos

  cria_arq_rand(nome_arq, NUM_NUMS); // CRIA NUMEROS
  int arquivos = distribui(NUM_CAMINHOS, nome_arq); // DISTRIBUI PARA AS FITAS
  for (int i=0; i<arquivos; i++){

    if(i%3==0) printf("\n\n\n");


    if(i == arquivos) printf("\n\n\tFita final:\n");
    sprintf(nome_arq_temp, "%s.%d", nome_arq, i);
    printf("------- %s:\n", nome_arq_temp);
    le_arq(nome_arq_temp);
  }
}
