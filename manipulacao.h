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
typedef void (*FncAcaoBranch) (Arvore_t*,pagina_t*,pagina_t*,void*);
typedef int (*FncMetodoLeitura) (dados_t*, FILE*);

/*--------------------------------Alocações------------------------------------*/
ArqDados_t *alocar_arq_dados(void);
Arvore_t *alocar_arvore(void);
void desalocar_ArqDados(ArqDados_t *arq_dados);
void desalocar_Arvore(Arvore_t *arvore);
void configuraInsercao(ArqDados_t *arq_dados, Arvore_t *arvore);
void configuraBusca(ArqDados_t *arq_dados, Arvore_t *arvore);
void ler_nome_arq_dados(ArqDados_t *arq_dados);
void ler_nome_arvore(Arvore_t *arvore);
void abrir_arq_dados(ArqDados_t *arq_dados, const char *tipo_leitura);
void iniciar_arvore(Arvore_t *arvore, const char *tipo_leitura);
void fechar_arq_dados(ArqDados_t *arq_dados);
void fechar_arvore(Arvore_t *arvore);

/*----------------------------Gets, Sets e escritas/leituras -----------------------------------*/
void ler_cabecalho_dados(ArqDados_t *arq_dados);
void ler_cabecalho_arvore(Arvore_t *arvore);
void escreverCabecalhoDados(ArqDados_t *arq_dados);
void escreverCabecalhoArvore(Arvore_t *arvore);
void setCabecalhoArvoreNulo(Arvore_t *arvore);
int getTamCabecalhoDados(ArqDados_t *arq_dados);
int get_nroRegTotal(ArqDados_t *arq_dados);
int get_nroRegValidos(ArqDados_t *arq_dados);
InfoDados_t *ler_criterios_busca();
InfoDados_t *alocar_InfoDados(int qtd_crit);
void desalocar_InfoDados(InfoDados_t *informacao);
void processaRegistros(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, void *info_aux);
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
int arvore_vazia(Arvore_t *arvore);

/*------------------------------------------AÇÕES---------------------------------*/

void validaPrinta(ArqDados_t *arq_dados, InfoDados_t *criterios, long int byteOffset);
void NoOpAcaoRegArv(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3);

void printa_busca(dados_t *registro);
void NoOpAcaoRegSeq(dados_t *ignorar1);

void NoOpAcaoFinal(int ignorar);
void achouReg(int flag);

void insercao_branch(Arvore_t *arvore, pagina_t *pgn_mae, pagina_t *pgn_atual, void *info_aux);
void NoOpAcaoBranch(Arvore_t *ignorar1, pagina_t *ignorar2, pagina_t *ignorar3, void *ignorar4);

/*------------------------------------------DEMAIS FUNÇÕES---------------------------------*/
int validaInfoDados(InfoDados_t *info);
void cria_raiz(ArqDados_t *arq_dados, Arvore_t *arvore, long int byteOffSet, InfoDados_t *info_dados_inserir);
void insercao_arqDados(ArqDados_t *arq_dados, InfoDados_t *info_inserir);
void insercao_arvore(ArqDados_t *arq_dados, Arvore_t *arvore, long int byteOffSet, InfoDados_t *info_dados_inserir);
void buscaRegistro(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios);
InfoInserida_t *criar_InfoInserida(ArqDados_t *arq_dados, InfoDados_t *info_dados, long int byteOffset);
void mostrar_info_dados(InfoDados_t *criterios);
InfoDados_t *ler_dados_registro(int(*metodoLeitura)(dados_t *, FILE *), ArqDados_t *arq_dados, int *tam_reg);
int lerRegDoArqDados(dados_t *reg, FILE *arq);
int lerRegDoStdin(dados_t *reg, FILE *arq);
long int getProxByteOffSet(ArqDados_t *arq_dados);

#endif