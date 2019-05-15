#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bubblesort.h"

#define RAM 5
#define NUM_NUMS 22
#define NUM_CAMINHOS 3
#define VALOR_GRANDE 99999

FILE **ARQUIVOS_FINAIS = NULL;
int TOTAL_ARQUIVOS = 0;

// Cria um arquivo temporario chamado nome com tam numeros aleatorios.
// Os numeros gerados sÃ£o mostrados na tela.
void cria_random(char *nome, int tam){
  FILE *arquivo;
  arquivo = fopen(nome,"w");
  int aleatorio;

  for(int i=0; i < tam; i++){
    aleatorio = rand()%26; //0 a 25
    aleatorio += 'A'; // A a Z
    fputc(aleatorio, arquivo);
  }

  fclose(arquivo);
}

char dado(FILE *arquivo){
  int byte = ftell(arquivo);
  char letra = fgetc(arquivo);
  fseek(arquivo, byte, 0);
  return letra;
}

void alocar_novos(int ultimo, int novos, FILE **arquivos_temporarios){
  arquivos_temporarios = (FILE**) realloc(arquivos_temporarios, sizeof(FILE*) * (ultimo + novos));
}

void intercala_arquivos(int arquivo_inicial, int arquivo_final, FILE **arquivos_temporarios, char *nome_arquivo, int tamanho_divisao){
  int rodada = 1;
  abrir_arquivos(arquivo_inicial, arquivo_final, nome_arquivo, arquivos_temporarios, "r");
  int qnt_arquivos = arquivo_final - arquivo_inicial;
  char buffer[RAM];
  int total_lidos = 0;
  int menor, tamanho_memoria = 0;
  int indice_arquivos = 0, idx_arquivo = 0;
  int aux_rodada = 0;

  if(tamanho_divisao >= NUM_NUMS){ // 1 arquivo apenas

    abrir_arquivos(arquivo_final, arquivo_final+1, nome_arquivo, arquivos_temporarios, "w");

    while(total_lidos < NUM_NUMS){
      while((total_lidos < NUM_NUMS) && (tamanho_memoria < RAM) && (buffer[tamanho_memoria] = menor_arquivos(arquivo_inicial, arquivo_final, arquivos_temporarios, nome_arquivo, tamanho_divisao, rodada)) != -1){
        tamanho_memoria++;
        total_lidos++;
      }
      for(int j = 0; j<tamanho_memoria;j++){
        fputc(buffer[j], arquivos_temporarios[arquivo_final]);
        //printf("[%i] - %c\n",j,buffer[j]);
      }

      tamanho_memoria = 0; // RESETAR MEMORIA
      for(int j = 0; j < NUM_CAMINHOS; j++){
        if(dado(arquivos_temporarios[arquivo_inicial+j]) == -1 || byte(arquivos_temporarios[arquivo_inicial+j])%tamanho_divisao == 0){
          aux_rodada++;
        }
      }
      if(aux_rodada == NUM_CAMINHOS) rodada++; // AUMENTAR RODADA
      aux_rodada = 0;
    }
    fechar_arquivos(arquivo_final, arquivo_final+1, arquivos_temporarios);

  }else{ // mais de 1 arquivo
    //alocar_novos(arquivo_final, NUM_CAMINHOS, arquivos_temporarios);
    abrir_arquivos(arquivo_final, arquivo_final+NUM_CAMINHOS, nome_arquivo, arquivos_temporarios, "w");
    while(total_lidos < NUM_NUMS){
      while((total_lidos < NUM_NUMS) && (tamanho_memoria < RAM) && (buffer[tamanho_memoria] = menor_arquivos(arquivo_inicial, arquivo_final, arquivos_temporarios, nome_arquivo, tamanho_divisao, rodada)) != -1){
        tamanho_memoria++;
        total_lidos++;
      }

      idx_arquivo = arquivo_final + indice_arquivos;

      for(int j = 0; j<tamanho_memoria;j++){
        fputc(buffer[j], arquivos_temporarios[idx_arquivo]);
        //printf("[%i] - %c\n",j,buffer[j]);
      }
      //printf("idx: %i \t%i", idx_arquivo,tamanho_memoria);
      //if(idx_arquivo == 6) exit(0);

      indice_arquivos++;
      indice_arquivos %= NUM_CAMINHOS;

      tamanho_memoria = 0; // RESETAR MEMORIA
      for(int j = 0; j < NUM_CAMINHOS; j++){
        if(dado(arquivos_temporarios[arquivo_inicial+j]) == -1 || byte(arquivos_temporarios[arquivo_inicial+j])%tamanho_divisao == 0){
          aux_rodada++;
        }
      }
      if(aux_rodada == NUM_CAMINHOS) rodada++; // AUMENTAR RODADA
      aux_rodada = 0;
    }
    fechar_arquivos(arquivo_final, arquivo_final+NUM_CAMINHOS, arquivos_temporarios);
  }
}

int byte(FILE *arquivo){
  return ftell(arquivo);
}

char menor_arquivos(int arquivo_inicial, int arquivo_final, FILE **arquivos_temporarios, char *nome_arquivo, int tamanho_divisao, int rodada){

  int arquivos = arquivo_final - arquivo_inicial;
  int idx_menor = -1;
  int menor = VALOR_GRANDE, valor_analise;

  for(int i=0; i<arquivos; i++){
    if(byte(arquivos_temporarios[arquivo_inicial+i]) < tamanho_divisao*rodada){ // NAO PODE PASSAR DO LIMITE DA DIVISAO
      valor_analise = dado(arquivos_temporarios[arquivo_inicial+i]); // BUSCA DADO SEM AVANÇAR
      if(valor_analise != -1 && valor_analise < menor){
        menor = valor_analise;
        idx_menor = arquivo_inicial+i;
      }
    }
  }

  // ERRO
  if(menor == VALOR_GRANDE && idx_menor == -1){
    printf("\nERRO MENOR VALOR GRANDE.\n");
    return -1;
  }
  return pega_dado(arquivos_temporarios[idx_menor]);
}

// INCREMENTA PONTEIRO
char pega_dado(FILE *arquivo){
  char l = fgetc(arquivo);
  return l;
}

void intercalacao_balanceada(int num_caminhos, FILE **arquivos_temporarios, char *nome_arquivo){
  int tamanho_divisao = RAM;
  int arquivo_inicial = 0, arquivo_final = num_caminhos;

  while(tamanho_divisao < NUM_NUMS){
    intercala_arquivos(arquivo_inicial, arquivo_final, arquivos_temporarios, nome_arquivo, tamanho_divisao);
    arquivo_inicial = arquivo_final;
    arquivo_final += num_caminhos;
    tamanho_divisao += tamanho_divisao * num_caminhos;
  }

  // SE SAIU SÓ FALTA O ÚLTIMO
  intercala_arquivos(arquivo_inicial, arquivo_final, arquivos_temporarios, nome_arquivo, tamanho_divisao);

  TOTAL_ARQUIVOS = arquivo_final + 1;
  ARQUIVOS_FINAIS = arquivos_temporarios;
  exibir_dados(ARQUIVOS_FINAIS, TOTAL_ARQUIVOS, nome_arquivo);
}

void abrir_arquivos(int inicio, int fim, char *nome_arq, FILE **arquivos_temporarios, char *modo){
  char nome_temporario[20];

  for(int i = inicio; i < fim; i++){
    sprintf(nome_temporario, "%d-%s", i, nome_arq);
    arquivos_temporarios[i] = fopen(nome_temporario, modo);
  }
}

void cria_temporarios(int num_caminhos, char *nome_arq){
  FILE *arquivo = fopen(nome_arq, "r");
  FILE **arquivos_temporarios = (FILE**) malloc(sizeof(FILE*) * num_caminhos * RAM); // VETOR DE ARQUIVOS TEMPORARIOS
  int buffer[RAM], quantia_lidos, idx = 0;

  abrir_arquivos(0, num_caminhos, nome_arq, arquivos_temporarios, "w");

  // VALORES INTEIROS
  while(RAM == (quantia_lidos = fread(&buffer, sizeof(char), RAM, arquivo))){
    bubble_sort(buffer, quantia_lidos);
    fwrite(buffer, sizeof(char) * RAM, 1, arquivos_temporarios[idx]);
    idx++;
    idx %= num_caminhos;
  }

  // VALORES RESTANTES
  bubble_sort(buffer, quantia_lidos);
  fwrite(buffer, sizeof(char) * quantia_lidos, 1, arquivos_temporarios[idx]);

  // FECHAR ARQUIVOS
  fechar_arquivos(0, num_caminhos, arquivos_temporarios);
  fclose(arquivo);

  intercalacao_balanceada(num_caminhos, arquivos_temporarios, nome_arq);
}

void exibir_dados(FILE **arquivos_temporarios, int total, char *nome_arquivo){
  abrir_arquivos(0, total, nome_arquivo, arquivos_temporarios, "r");
  char l;
  for(int i = 0; i < total; i++){
    printf("Arquivo [%i]: %i-temporario.txt\n\n    ",i,i);
    while((l = getc(arquivos_temporarios[i])) != -1){
      printf("%c ", l);
    }
    printf("\n\n\n");
    if((i+1)%3 == 0) printf("\n\n\n");
  }
}

void fechar_arquivos(int inicio, int fim, FILE **arquivos){
  for(int i = inicio; i < fim; i++){
    fclose(arquivos[i]);
  }
}


int main(){

  char nome_arq[] = "temporario.txt";
  char *nome_arq_temp = malloc(sizeof(char)*strlen(nome_arq)+3); // suporta ate .99 arquivos

  cria_random(nome_arq, NUM_NUMS); // CRIA ARQUIVO TEMPORARIO
  cria_temporarios(NUM_CAMINHOS, nome_arq); // CRIA NOVOS TEMPORARIOS E INICIA A INTERCALAÇÃO

}
