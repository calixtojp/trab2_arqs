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

    void fwriteStatusArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho);
    void fwriteCabecalhoArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho);
    void ler_cabecalho_arq_arvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho);
    void ler_pagina_disco(FILE *arqArvore, no_arvore_t *no);
    long long int busca_bin_no(no_arvore_t *no_atual, int ini, int fim, int chave, int *P);
    void set_no(no_arvore_t *no_configurar, int nChaves, int nivel);
    void insere_ordenado(no_arvore_t *no_inserir, chave_t *chave_inserir);

#endif