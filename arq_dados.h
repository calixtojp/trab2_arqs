/*
	Biblioteca que fornece ferramentas para utilizar
	os arquivos de Dados.
*/

#ifndef _ARQDADOS
#define _ARQDADOS

	#include <stdlib.h>
	
	typedef struct Cabecalho cabecalho_t;
	typedef struct Dados dados_t;	

	cabecalho_t *alocar_cabecalho_dados();
	void mostrar_cabecalho_dados(cabecalho_t *cabecalho);
	dados_t *alocar_dados();
	dados_t **alocar_vet_dados(int n);
	void desalocar_vet_dados(dados_t **vetor, int n);
	void inicializar_cabecalho(cabecalho_t *cabecalho);
	void arquivo_status(cabecalho_t *cabecalho,char status);
	int get_nroRegArq(cabecalho_t *cabecalho);
	int get_nroRegRem(cabecalho_t *cabecalho);
	long int get_proxByteOffset(cabecalho_t *cabecalho);
	int getIdCrime(dados_t *dado);
	void getDataCrime(dados_t *dado, char *dataDestino);
	int getNumeroArtigo(dados_t *dado);
	void getMarcaCelular(dados_t *dado, char *marcaDestino);
	void *getCampoInt(dados_t *dado, char *campo);
	void *getCampoStr(dados_t *dado, char *campo);
	void getRegistro(long int byteOffSet, FILE *arqDados, dados_t *reg);
	int campoNulo_int(void *campo_int);
	int campoNulo_str(void *campo_str);
	int bytesAteCampoIndexado(dados_t *reg, char *campo);
	long int len_reg_dados(dados_t *dado);
	int len_cabecalho_dados(void);
	void cabecalho_nroRegArq_incrementar(cabecalho_t *cabecalho, int qtd);
	void setCabecalhoDados_nroRegArq(cabecalho_t *cabecalho, int nroRegArq);
	void setCabecalhoDados_proxByteOffSet(cabecalho_t *cabecalho, int byteOffSet);
	void setCabecalhoDados_nroRegRem(cabecalho_t *cabecalho, int nroRegrem);
	void escrever_bin_registro_cabecalho(cabecalho_t *cabecalho, FILE *arq);
	void escreverCampoRemovido(FILE *arqDados);
	void ler_csv_registro(FILE *arq, dados_t *dados, char *c, int *registro_nulo);
	int ler_csv_campo_int(FILE *arq, char *c, int *registro_nulo);
	void ler_csv_campo_str_fixo(FILE *arq, char *c, char *str_fixo, int tam);
	char *ler_csv_campo_str_variavel(FILE *arq, char *c);
	void prepara_para_escrita(dados_t *reg);
	void escrever_bin_registro_dados(dados_t *dados, FILE *arq, cabecalho_t *cabecalho);
	void escrever_bin_campo_variavel(char *texto, FILE *arq, cabecalho_t *cabecalho);
	void reescrever_registro_dados(dados_t *dados, FILE *arq);
	void reescrever_campo_variavel(char *texto, FILE *arq);
	cabecalho_t *ler_cabecalho(FILE *arq_bin);
	void fwriteCabecalhoDados(FILE *arq_bin, cabecalho_t *cabecalho);
	void sair_fechando(FILE *arq_bin);
	void desalocar_registro(dados_t *registro);
	int existem_registros(cabecalho_t *cabecalho);
	void copia_registro(dados_t *destino, dados_t *origem);
	void mostrar_registros(FILE *arq_bin);
	int get_registro_removido(dados_t *registro);
	int ler_bin_status(FILE *arq_bin, dados_t *registro);
	int ler_bin_registro(dados_t *registro, FILE *arq_bin);
	void ler_bin_campos_fixos(FILE *arq_bin, dados_t *registro, int *flag_chegou_fim);
	void ler_bin_campos_variaveis(FILE *arq_bin, dados_t *registro, int *flag_chegou_fim);
	char *ler_bin_char_variavel(FILE *arq_bin);
	void mostrar_campos(dados_t *registro);
	void mostrar_campo_fixo(char *cursor, int tam_palavra);
	void mostrar_campo_variavel(char *palavra);
	cabecalho_t* ler_dados_cabecalho(FILE *arq_bin);
	void leRegStdin(dados_t *reg);
	int testar_criterios(dados_t *reg_dados, char **vet_nomes, char **vet_vals_str, int *vet_vals_int, int qtd_crit);
	char getStatusDados(cabecalho_t *cabecalho);
	void escrever_campo_removido(FILE *arq, dados_t *dados);
	void remocaoLogica(dados_t *registro, cabecalho_t *cabecalho);
	void fazAlteracoes(dados_t *reg, char **vet_nomes, char **vet_vals_str, int *vet_vals_int, int qtd_crit);
	void completaRegistroComDollar(FILE *arqDados, int qtdDolars);
	void fwriteStatusDados(FILE *arq, cabecalho_t *cabecalho);
	void setStatusDados(cabecalho_t *cabecalho, char status);

#endif