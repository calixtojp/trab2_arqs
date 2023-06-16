#ifndef _ARVORE_B_ESTRELA
#define _ARVORE_B_ESTRELA

    #include <stdlib.h>

    typedef struct cabecalho_arvore cabecalho_arvore_t;
    typedef struct chave chave_t;
    typedef struct no_arvore no_arvore_t;
    typedef struct pagina{//Struct auxiliar
        no_arvore_t *no;
        int RRN_no;
    }pagina_t;

    typedef struct InfoPromovida{//Struct auxiliar
        chave_t *chave_promovida;
        int *ponteiro_promovido;
    }InfoPromovida_t;

    typedef int (*FncGetRRNirma) (int, no_arvore_t*);
    typedef size_t (*FncFluxoMemSec) (void*, size_t, size_t, FILE*);//funções de fluxo com a memória secundária (fread/fwrite)
    //size_t fwrite(const void *__restrict__ __ptr, size_t __size, size_t __n, FILE *__restrict__ __s)
    //size_t fread(void *__restrict__ __ptr, size_t __size, size_t __n, FILE *__restrict__ __stream)

    //Funções de alocar/desalocar
    cabecalho_arvore_t *alocar_cabecalho_arvore(void);
    no_arvore_t *alocar_no(void);
    void desalocar_no(no_arvore_t *no);
    chave_t *alocar_chave();
    void desalocar_chave(chave_t *chave);

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
    long long int buscaBinNo(no_arvore_t *no_atual, int ini, int fim, int chave, int *P);
    void insere_ordenado_no(no_arvore_t *no_inserir, chave_t *chave_inserir);
    int redistribuicao(FILE *arqArvore, pagina_t *pgn_mae, pagina_t *pgn_atual, InfoPromovida_t *info);
    void split_1_para_2(FILE *arqArvore, cabecalho_arvore_t *cabecalho, pagina_t *pgn_mae, pagina_t *pgn_atual, InfoPromovida_t *info);
    void split_2_para_3(FILE *arqArvore, cabecalho_arvore_t *cabecalho, pagina_t *pgn_mae, pagina_t *pgn_atual, InfoPromovida_t *info);

    //debug
    void mostra_cabecalho_arvore(cabecalho_arvore_t *cabecalho);

#endif