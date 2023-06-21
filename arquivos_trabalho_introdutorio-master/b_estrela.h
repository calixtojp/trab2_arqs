#ifndef _EST_H
#define _EST_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include "registro.h"


    /* Utilizamos alguns defines para melhorar a legibilidade de algumas constantes pelo código */

    #define inseriu              0
    #define nao_inseriu         -1
    #define m                    5  
    #define qtd_chavesMax        m-1
    #define qtd_descendentesMax  m
    #define CONSISTENTE         '1'
    #define INCONSISTENTE       '0'
    /* 
    No vetor de ramos da estrutura de Árvore, estabelecemos a seguinte convenção 
    do que cada índice armazenará 
    */
    #define ramo_atual           0
    #define ramo_pai             1
    #define ramo_irmaEsq         2
    #define ramo_irmaDir         3


    /* Estruturas utilizadas */

    typedef struct{
        int id;
        long long int byteOffset;
    } Chave;

    typedef struct {
        int nivel;
        int qtd_chaves;
        Chave chaves[qtd_chavesMax];
        int descendentes[qtd_descendentesMax];
    } No;

    typedef struct{
        char status;
        int raiz;
        int RRNproxNo;
        int nroNiveis;
        int nroChaves;
        No *ramos;
    } Arvore_B;


    /* Funções disponíveis ao usuário */

    Arvore_B * criar_arvore();
    void inserir_chaveArvore(FILE *arq_arv, Arvore_B *arvore, int id, long long int byteOffSet);
    void escrever_arvore(FILE *arq_bin, FILE *arq_arv, Arvore_B *arvore, Cabecalho *meu_cabecalho);
    void liberar_arvore(Arvore_B *arvore);
    long long int buscar_valorRamo(FILE *arq_bin, No *ramos, int rrn_atual, int valor_buscado);
    void escrever_cabecalhoArvore(FILE *arq_arv, Arvore_B *arvore);
    bool ler_cabecalhoArvore(FILE *arq_indice, Arvore_B *arvore);

#endif