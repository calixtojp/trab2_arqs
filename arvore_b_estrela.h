#ifndef _ARVORE_B_ESTRELA
#define _ARVORE_B_ESTRELA

    #include <stdlib.h>

    typedef struct cabecalho_arvore cabecalho_arvore_t;
    typedef struct chave chave_t;
    typedef struct no_arvore no_arvore_t;

    cabecalho_arvore_t *alocar_cabecalho_arvore(void);
    void ler_cabecalho_arq_arvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho);
    char getStatusArvore(cabecalho_arvore_t *cabecalho);

#endif