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
#include "arvore_b_estrela.h"

typedef struct ArqDados ArqDados_t;
typedef struct Arvore Arvore_t;
typedef struct InfoDados InfoDados_t;

typedef void (*FncAcaoRegSeq) (dados_t*);
typedef void (*FncAcaoRegArv) (ArqDados_t*, InfoDados_t*, long int);
typedef void (*FncAcaoFinal) (int);
typedef void (*FncAcaoBranch) (int);
typedef long int (*FncGetByteOffSet) (void*, int);
typedef int (*FncCampoNulo) (void*);

typedef struct acoes{
    //registro de ações a serem executadas dependendo da funcionalidade

    FncAcaoRegArv reg_arv; //ação que atua sobre um registro encontrado a partir da busca na árvore B*
    FncAcaoRegSeq reg_seq; //ação que atua sobre um registro encontrado a partir da busca sequencial
    FncAcaoBranch no; //Ação que atua sobre um branch (isto é, um nó e todos os seus descendentes) da árvore B*
    FncAcaoFinal final; //Ação executada depois das ações principais
}FncAcoes;


/*--------------------------------Alocações------------------------------------*/
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

/*----------------------------Gets e Sets -----------------------------------*/
int getTamCabecalhoDados(ArqDados_t *arq_dados);
int get_nroRegTotal(ArqDados_t *arq_dados);
int get_nroRegValidos(ArqDados_t *arq_dados);
InfoDados_t *ler_criterios_busca();
InfoDados_t *alocar_InfoDados(int qtd_crit);
void desalocar_InfoDados(InfoDados_t *informacao);
void processaRegistros(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, FncAcoes *acoes);
int testarStatusArvore(Arvore_t *arvore);
int testarStatusDados(ArqDados_t *arq_dados);
void reiniciarCursorDados(ArqDados_t *arq_dados);
void reiniciarCursorArvore(Arvore_t *arvore);
void alterarStatusDados(ArqDados_t *arq_dados, int status);
void alterarStatusArvore(Arvore_t *arvore, int status);
void escreverStatusDados(ArqDados_t *arq_dados);
void escreverStatusArvore(Arvore_t *arvore);
void levaFinalCursorDados(ArqDados_t *arq_dados);
char *getNomeArqDados(ArqDados_t *arq_dados);
char *getNomeArvore(Arvore_t *arvore);


/*------------------------------------------AÇÕES---------------------------------*/
FncAcoes *alocar_acoes();
void desalocar_acoes(FncAcoes *acoes);
void set_acoes(FncAcoes *acoes, FncAcaoRegArv reg_arv, FncAcaoRegSeq reg_seq, FncAcaoNo no, FncAcaoFinal final);

void validaPrinta(ArqDados_t *arq_dados, InfoDados_t *criterios, long int byteOffset);
void NoOpAcaoRegArv(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3);

void printa_busca(dados_t *registro);
void NoOpAcaoRegSeq(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3);

void NoOpAcaoNo(int);
void achouReg(int flag);

void NoOpAcaoFinal(void);

#endif