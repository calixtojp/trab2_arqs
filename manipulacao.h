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

typedef void (*FncAcao) (ArqDados_t*, ArqIndex_t*, dados_t*, InfoDados_t*, long int);
typedef void (*FncFinaliza) (void*, int);
typedef long int (*FncGetByteOffSet) (void*, int);
typedef int (*FncCampoNulo) (void*);
typedef void* (*FncGetCampoIndexIndexado) (dados_t*, char*);
typedef void (*FncSetDadoIndx) (void*, long int, void*);
typedef void (*FncSetVetIndx) (void*, int, void*);
typedef void (*FncOrdemTipo) (void*, int);
typedef int (*FncBuscaBin) (void*, cabecalho_indx_t*, void*, int*);
typedef void (*FncCopiaVet) (void*, void*, int, int , int, int);

ArqDados_t *alocar_arq_dados(void);
Arvore_t *alocar_arvore(void);
void ler_nome_arq_dados(ArqDados_t *arq_dados);
void ler_nome_arvore(Arvore_t *arvore);
void ler_tipoDado(ArqIndex_t *arq_index);
void abrir_arq_dados(ArqDados_t *arq_dados, const char *tipo_leitura);
void iniciar_arvore(Arvore_t *arvore, const char *tipo_leitura);
void ler_dados_arq_index(ArqIndex_t *arq_index);
void desalocar_ArqDados(ArqDados_t *arq_dados);
void desalocar_Arvore(Arvore_t *arvore);
void fechar_arq_dados(ArqDados_t *arq_dados);
void fechar_arvore(Arvore_t *arvore);
void ler_cabecalho_dados(ArqDados_t *arq_dados);
void ler_cabecalho_arvore(Arvore_t *arvore);
void escreverCabecalhoDados(ArqDados_t *arq_dados);
int getTamCabecalhoDados(ArqDados_t *arq_dados);
void printa_busca(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *registro, InfoDados_t *alteracoes,long int byteOffSet);
int get_nroRegTotal(ArqDados_t *arq_dados);
int get_nroRegValidos(ArqDados_t *arq_dados);
char *getNomeArqIndex(ArqIndex_t *arq_index);
char *getNomeArqDados(ArqDados_t *arq_dados);
void levaFinalCursorDados(ArqDados_t *arq_dados);
void alocar_vet_index(ArqIndex_t *arq_index, unsigned int nroRegValidos);
void realocar_vet_index(ArqIndex_t *arq_index, int original, int acrescimo);
int indexaRegistro(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int pos_reg, long int *byteOffSetAnt);
void desindexaRegistro(ArqIndex_t *arq_index, int pos);
void ordenaVetIndex(ArqIndex_t *arq_index, int qntd_reg);
void escreveVetIndex(ArqIndex_t *arq_index, int inicio, int fim);
int get_nroRegIndex(ArqIndex_t *arq_index);
void inserirReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg, int pos);
int inserirRegStdin(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int qtdInserir);
int existe_index(InfoDados_t *criterios, ArqIndex_t *arq_index);
void percorrer_index(FncGetByteOffSet get_byteOffset, int pos_prim, int qtd_reg_val, 
                    ArqDados_t *arq_dados, ArqIndex_t *arq_index,InfoDados_t *criterios, InfoDados_t *alteracoes,FncAcao acao, FncFinaliza final);
void busca_bin_index(ArqIndex_t *arq_index, ArqDados_t *arq_dados, int pos_chave, InfoDados_t *criterios, InfoDados_t *alteracoes,FncAcao acao, FncFinaliza final);
void busca_seq_dados(ArqDados_t *arq_dados, ArqIndex_t *arq_index,InfoDados_t *criterios, InfoDados_t *alteracoes, FncAcao acao, FncFinaliza final);
int get_nroRegIndex(ArqIndex_t *arq_index);
InfoDados_t *ler_criterios_busca();
InfoDados_t *alocar_InfoBusca(int qtd_crit);
void desalocar_InfoBusca(InfoDados_t *informacao);
void processaRegistros(ArqDados_t *arq_dados, ArqIndex_t *arq_index, InfoDados_t *criterios, InfoDados_t *alteracoes, FncAcao acao, FncFinaliza final);
void achouReg(void *arq_index, int flag);
int testarStatusArvore(Arvore_t *arvore);
int testarStatusDados(ArqDados_t *arq_dados);
void deletarRegistro(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *registro, InfoDados_t *ignorar, long int byteOffset);
void reiniciarCursorIndex(ArqIndex_t *arq_index);
void reiniciarCursorDados(ArqDados_t *arq_dados);
void alterarStatusIndex(ArqIndex_t *arq_index, int status);
void alterarStatusDados(ArqDados_t *arq_dados, int status);
void escreverStatusDados(ArqDados_t *arq_dados);
void escreverStatusIndex(ArqIndex_t *arq_index);
void escreveArqIndex(ArqIndex_t *arq_index);
void copiaVetTemp(void *ponteiro, int ignorar);
void escreverCabecalhoIndex(ArqIndex_t *arq_index);
void alterarQtdRegIndex(ArqIndex_t *arq_index, int qtdRegIndex);
void modificaReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg_antigo, InfoDados_t *alteracoes,long int byteOffSet);
void ordenaVetIndexFinal(void *arq_index, int qtdReg_ordenar);

#endif