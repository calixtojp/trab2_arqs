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

typedef struct InfoInserida{//Struct auxiliar que resume uma informação inserida na árvore.
    int valida;//flag que indica se há uma informação válida dentro dessa struct
    chave_t *chave;
    int *ponteiro;
}InfoInserida_t;

typedef enum result_redistribuicao{
    redistribuiu,
    retorna_dir,
    retorna_esq
}result_redistribuicao_t;

//funções que indicam qual irmã está sendo retornada (esquerda ou direita).
typedef int (*FncGetRRNirma) (int, no_arvore_t*);
//funções de fluxo com a memória secundária (fread/fwrite)
typedef size_t (*FncFluxoMemSec) (void*, size_t, size_t, FILE*);
typedef void (*FncInsereNo) (no_arvore_t *, int, void *, int);

//Funções de alocar/desalocar
InfoInserida_t *alocar_InfoInserida(void);
void desalocar_InfoInserida(InfoInserida_t *info);
cabecalho_arvore_t *alocar_cabecalho_arvore(void);
no_arvore_t *alocar_no(void);
void desalocar_no(no_arvore_t *no);
chave_t *alocar_chave();
void desalocar_chave(chave_t *chave);
pagina_t *aloca_pagina(void);
void desaloca_pagina(pagina_t *pagina);

//Funções get/set
int get_ordem_arvore(void);
int get_tam_pagina(void);
char getStatusArvore(cabecalho_arvore_t *cabecalho);
void setStatusArvore(cabecalho_arvore_t *cabecalho, char status);
int get_noRaiz(cabecalho_arvore_t *cabecalho);
void set_noRaiz(cabecalho_arvore_t *cabecalho, int nova_raiz);
int get_nroChaves(cabecalho_arvore_t *cabecalho);
void set_nroChaves(cabecalho_arvore_t *cabecalho, int nroChaves);
int get_RRNproxNo(cabecalho_arvore_t *cabecalho);
void set_RRNproxNo(cabecalho_arvore_t *cabecalho, int RRN);
void set_lixoCabecalho(cabecalho_arvore_t *cabecalho);
int get_nChaves(no_arvore_t *no);
void set_nChaves(no_arvore_t *no, int nova_nChaves);
int get_nroNiveis(cabecalho_arvore_t *cabecalho);
void set_nroNiveis(cabecalho_arvore_t *cabecalho, int nova_nroNiveis);
int get_nivel_no(no_arvore_t *no);    
void set_nivel_no(no_arvore_t *no, int nivel);
int get_chaves_C(chave_t *chave);
void set_chaves_C(chave_t *chave, int C);
long int get_chaves_Pr(chave_t *chave);
void set_chaves_Pr(chave_t *chave, int Pr);

//funções fluxo (escrita/leitura) da memória
void fluxo_StatusArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo);
void fluxo_CabecalhoArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo);
void fluxo_ponteiro_no(FILE *arqArvore, no_arvore_t *no, int pos_vet_ponteiros, FncFluxoMemSec funcFluxo);
void fluxo_chave_no(FILE *arqArvore, no_arvore_t *no, int pos_vet_chaves, FncFluxoMemSec funcFluxo);
void fluxo_no(FILE *arqArvore, no_arvore_t *no_operar, FncFluxoMemSec funcFluxo);

//Demais funções
long long int buscaBinNo(no_arvore_t *no_atual, int ini, int fim, int chave, int *P);
void insere_ordenado_no(FILE *arq, pagina_t *pgn, InfoInserida_t *info);
result_redistribuicao_t redistribuicao(FILE *arqArvore, pagina_t *pgn_mae, pagina_t *pgn_atual, 
                                        pagina_t **png_irma, InfoInserida_t *info);
void split_1_para_2(FILE *arqArvore, cabecalho_arvore_t *cabecalho, pagina_t *pgn_atual, InfoInserida_t *info);
void split_2_para_3(FILE *arqArvore, cabecalho_arvore_t *cabecalho, pagina_t *pgn_mae,
                                        pagina_t *pgn_esq, pagina_t *pgn_dir,InfoInserida_t *info);
int ehFolha(pagina_t *pgn_atual);

#endif