#ifndef _ARVORE_B_ESTRELA
#define _ARVORE_B_ESTRELA

    #include <stdlib.h>

    typedef struct cabecalho_arvore cabecalho_arvore_t;
    typedef struct chave chave_t;
    typedef struct no_arvore no_arvore_t;

    cabecalho_arvore_t *alocar_cabecalho_arvore(void);
    no_arvore_t *alocar_no(void);
    void desalocar_no(no_arvore_t *no);
    void mostrar_no(no_arvore_t *no);
    void ler_cabecalho_arq_arvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho);
    char getStatusArvore(cabecalho_arvore_t *cabecalho);
    int get_ordem_arvore(void);
    int get_tam_pagina(void);
    int get_noRaiz(cabecalho_arvore_t *cabecalho);
    int get_nChaves(no_arvore_t *no);
    void ler_pagina_disco(FILE *arqArvore, no_arvore_t *no);
    long long int busca_bin_no(no_arvore_t *no_atual, int ini, int fim, int chave, int *P);

#endif