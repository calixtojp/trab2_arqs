/*
        Biblioteca que utiliza as ferramentas fornecidas
    nas bibliotecas de manipulação dos arquivos de
    Índice e de Dados para manipula-los.
        Essa manipulação ocorre através dos tipos ArqDados
    (referente ao arquivo de dados) e o tipo ArqIndex (referente)
    ao arquivo de Index
*/

#ifndef _MANIPULACAO
#define _MANIPULACAO

#include "arq_dados.h"

typedef struct ArqDados ArqDados_t;
typedef struct Arvore Arvore_t;
typedef struct InfoDados InfoDados_t;

typedef void (*FncAcao) (ArqDados_t*, Arvore_t*, dados_t*, InfoDados_t*, long int);
typedef void (*FncFinaliza) (void*, int);
typedef int (*FncCampoNulo) (void*);
typedef void* (*FncGetCampoIndexIndexado) (dados_t*, char*);
typedef void (*FncSetDadoIndx) (void*, long int, void*);
typedef void (*FncSetVetIndx) (void*, int, void*);
typedef void (*FncOrdemTipo) (void*, int);
typedef void (*FncCopiaVet) (void*, void*, int, int , int, int);

ArqDados_t *alocar_arq_dados(void);
Arvore_t *alocar_arvore(void);
void ler_nome_arq_dados(ArqDados_t *arq_dados);
void ler_nome_arvore(Arvore_t *arvore);
void abrir_arq_dados(ArqDados_t *arq_dados, const char *tipo_leitura);
void iniciar_arvore(Arvore_t *arvore, const char *tipo_leitura);
void desalocar_ArqDados(ArqDados_t *arq_dados);
void desalocar_Arvore(Arvore_t *arvore);
void fechar_arq_dados(ArqDados_t *arq_dados);
void fechar_arvore(Arvore_t *arvore);
void ler_cabecalho_dados(ArqDados_t *arq_dados);
void ler_cabecalho_arvore(Arvore_t *arvore);
void escreverCabecalhoDados(ArqDados_t *arq_dados);
int getTamCabecalhoDados(ArqDados_t *arq_dados);
void printa_busca(ArqDados_t *arq_dados, Arvore_t *arvore, dados_t *registro, InfoDados_t *alteracoes,long int byteOffSet);
int get_nroRegTotal(ArqDados_t *arq_dados);
int get_nroRegValidos(ArqDados_t *arq_dados);
char *getNomeArqDados(ArqDados_t *arq_dados);
char *getNomeArvore(Arvore_t *arvore);
void levaFinalCursorDados(ArqDados_t *arq_dados);
// int indexaRegistro(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int pos_reg, long int *byteOffSetAnt);
// void desindexaRegistro(ArqIndex_t *arq_index, int pos);
// void inserirReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg, int pos);
// int inserirRegStdin(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int qtdInserir);
InfoDados_t *ler_criterios_busca();
InfoDados_t *alocar_InfoBusca(int qtd_crit);
void desalocar_InfoDados(InfoDados_t *informacao);
void processaRegistros(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, InfoDados_t *alteracoes, FncAcao acao, FncFinaliza final);
void achouReg(void *arvore, int flag);
int testarStatusArvore(Arvore_t *arvore);
int testarStatusDados(ArqDados_t *arq_dados);
void reiniciarCursorDados(ArqDados_t *arq_dados);
void alterarStatusDados(ArqDados_t *arq_dados, int status);
void escreverStatusDados(ArqDados_t *arq_dados);
// void modificaReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg_antigo, InfoDados_t *alteracoes,long int byteOffSet);

#endif