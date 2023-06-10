#include <stdio.h>
#include <stdlib.h>
#include "arvore_b_estrela.h"

#define M 5
#define TAM_PAGINA 76

struct cabecalho_arvore{
    char stataus;
    int noRaiz;
    int RRNproxNo;
    int nroNiveis;
    int nroChaves;
    char lixo[59];
};

struct chave{
    int C;
    long long Pr;
};

struct no_arvore{
    int nivel;
    int n;
    int P[M];
    chave_t chaves[M-1];
};