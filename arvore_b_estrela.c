#include <stdio.h>
#include <stdlib.h>
#include "arvore_b_estrela.h"

#define M 5
#define TAM_PAGINA 76

struct cabecalho_arvore{
    char status;
    int noRaiz;
    int RRNproxNo;
    int nroNiveis;
    int nroChaves;
    char lixo[59];
};

struct chave{
    int C; //Conteúdo do campo indexado
    long long Pr; //ByteOffSet do registro no arquivo de dados
};

struct no_arvore{
    int nivel;
    int n;
    int P[M];
    chave_t chaves[M-1];
};

cabecalho_arvore_t *alocar_cabecalho_arvore(void){
    cabecalho_arvore_t *cabecalho_retorno = malloc(sizeof(cabecalho_arvore_t));
    return cabecalho_retorno;
}

void mostra_cabecalho_arvore(cabecalho_arvore_t *cabecalho){
    printf("Cabeçalho da arvore: \n");
    printf("status=%c|noRaiz=%d|RRNproxNo=%d|nroNiveis=%d|nroChaves=%d\n"
        , cabecalho->status
        , cabecalho->noRaiz
        , cabecalho->RRNproxNo
        , cabecalho->nroNiveis
        , cabecalho->nroChaves
    );
}

void ler_cabecalho_arq_arvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho){
    fread(&(cabecalho->status), sizeof(char), 1, arqArvore);
    fread(&(cabecalho->noRaiz), sizeof(int), 1, arqArvore);
    fread(&(cabecalho->RRNproxNo), sizeof(int), 1, arqArvore);
    fread(&(cabecalho->nroNiveis), sizeof(int), 1, arqArvore);
    fread(&(cabecalho->nroChaves), sizeof(int), 1, arqArvore);
    for(int i = 0; i < 59; ++i){
        fread(&(cabecalho->lixo[i]), sizeof(char), 1, arqArvore);
    }

    mostra_cabecalho_arvore(cabecalho);
}

char getStatusArvore(cabecalho_arvore_t *cabecalho){
    return cabecalho->status;
}