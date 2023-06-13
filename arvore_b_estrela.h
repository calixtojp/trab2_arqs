#ifndef _ARVORE_B_ESTRELA
#define _ARVORE_B_ESTRELA

    #include <stdlib.h>

    typedef struct cabecalho_arvore cabecalho_arvore_t;
    typedef struct chave chave_t;
    typedef struct no_arvore no_arvore_t;
    // int retorna_irma_esq(int pos_vet_P, int n, no_arvore_t *no)
    typedef int (*FncGetRRNirma) (int, no_arvore_t*);
    typedef size_t (*FncFluxoMemSec) (const void*, size_t, size_t, FILE*);//funções de fluxo com a memória secundária (fread/fwrite)
    //size_t fwrite(const void *__restrict__ __ptr, size_t __size, size_t __n, FILE *__restrict__ __s)
    //size_t fread(void *__restrict__ __ptr, size_t __size, size_t __n, FILE *__restrict__ __stream)

    //Funções de alocar/desalocar
    cabecalho_arvore_t *alocar_cabecalho_arvore(void);
    no_arvore_t *alocar_no(void);
    void desalocar_no(no_arvore_t *no);

    void mostrar_no(no_arvore_t *no);

    //Funções get/set
    int get_ordem_arvore(void);
    int get_tam_pagina(void);
    char getStatusArvore(cabecalho_arvore_t *cabecalho);
    void setStatusArvore(cabecalho_arvore_t *cabecalho, char status);
    int get_noRaiz(cabecalho_arvore_t *cabecalho);
    void set_noRaiz(cabecalho_arvore_t *cabecalho, int nova_raiz);
    int get_nChaves(no_arvore_t *no);
    void set_nChaves(no_arvore_t *no, int nova_nChaves);
    int get_nroNiveis(cabecalho_arvore_t *cabecalho);
    void set_nroNiveis(cabecalho_arvore_t *cabecalho, int nova_nroNiveis);
    int get_nivel_no(no_arvore_t *no);    
    void set_nivel_no(no_arvore_t *no, int nivel);

    //funções escrita/leitura da memória
    void fluxo_StatusArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo);
    void fluxo_CabecalhoArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo);
    void fluxo_ponteiro_no(FILE *arqArvore, no_arvore_t *no, int pos_vet_ponteiros, FncFluxoMemSec funcFluxo);
    void fluxo_chave_no(FILE *arqArvore, no_arvore_t *no, int pos_vet_chaves, FncFluxoMemSec funcFluxo);
    void fluxo_no(FILE *arqArvore, no_arvore_t *no_operar, FncFluxoMemSec funcFluxo);

    //Demais funções
    long long int busca_bin_no(no_arvore_t *no_atual, int ini, int fim, int chave, int *P);
    void insere_ordenado_no(no_arvore_t *no_inserir, chave_t *chave_inserir);
    int redistribuicao(FILE *arqArvore, no_arvore_t *no_mae, no_arvore_t *no_filho, chave_t *chave_inserir);

#endif