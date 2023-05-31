/*
	Biblioteca que fornece ferramentas para utilizar
	os arquivos de Índice. teste
*/

#ifndef _ARQINDICE
#define _ARQINCIDE

    typedef struct Cabecalho_indx cabecalho_indx_t;
    typedef struct Dados_indx_int dados_indx_int_t;
    typedef struct Dados_indx_str dados_indx_str_t;

    cabecalho_indx_t *alocar_cbl_indx(void);
    dados_indx_int_t *alocDadoIndxInt(void);
    dados_indx_str_t *alocDadoIndxStr(void);
    dados_indx_int_t **aloc_vet_indx_DadoInt(int nroRegValidos);
    dados_indx_str_t **aloc_vet_indx_DadoStr(int nroRegValidos); 
    char *alocarCampoIndexado(void);
    void desalocarVetIndxDadoInt(void *vetor, int nroRegValidos); 
    void desalocarVetIndxDadoStr(void *vetor, int nroRegValidos); 
    void copiaVetInt(void *vet_dest, void *vet_ori, int ini_dest, int fim_dest, int ini_ori, int fim_ori);
    void copiaVetStr(void *vet_dest, void *vet_ori, int ini_dest, int fim_dest, int ini_ori, int fim_ori);
    void copiaDadoIndex_int(void *destino, void *origem);
    void copiaDadoIndex_str(void *destino, void *origem);
    void setCabecalhoIndex(cabecalho_indx_t *cabecalho, const char status, int qtdReg);
    void setStatusIndex(cabecalho_indx_t *cabecalho, char status);
    void setDadoIndxInt(void *dado, long int byteOffSet, void *valor);
    void setDadoIndxStr(void *dado, long int byteOffSet, void *valor);
    void setVetIndx_int(void *vet, int pos, void *dado);
    void setVetIndx_str(void *vet, int pos, void *dado);
    void set_qtdReg(cabecalho_indx_t *cabecalho, int qtd_reg);
    cabecalho_indx_t *get_cabecalho_indx(FILE *arqIndex);
    long int get_byteOffset_int(void *ponteiro, int pos);
    long int get_byteOffset_str(void *ponteiro, int pos);
    char getStatusIndex(cabecalho_indx_t *cabecalho);
    int getTamCabecalhoIndx(void);
    int getTamDadoIndx_int(void);
    int getTamDadoIndx_str(void);
    int get_qtdReg(cabecalho_indx_t *cabecalho);
    void ler_index_cabecalho(FILE *arq, cabecalho_indx_t* cabecalho);
    dados_indx_int_t **ler_index_dado_int(FILE *arq, cabecalho_indx_t *cabecalho);
    dados_indx_str_t **ler_index_dado_str(FILE *arq, cabecalho_indx_t *cabecalho);
    void *leDadoIndx_int(FILE *arqIndex);
    void *leDadoIndx_str(FILE *arqIndex);
    int compara_dado_int(const void *a, const void *b);
    int compara_dado_str(const void *a, const void *b);
    int comparacao_vet_dados_indx_int_RegIndx(void *vetor, int pos, void *dado);
    int comparacao_vet_dados_indx_str_RegIndx(void *vetor, int pos, void *dado);
    void ordenaVetIndex_int(void *vetor_generico, int qntd_reg);
    void ordenaVetIndex_str(void *vetor_generico, int qntd_reg);
    void escreveCabecalhoIndex(FILE *arqIndex, cabecalho_indx_t *cabecalho);
    void escreveVetIndx_int(FILE *arqIndex, void *vet_indx_int, int pos);
    void escreveVetIndx_str(FILE *arqIndex, void *vet_indx_str, int pos);
    int busca_bin_int(void *vetor, cabecalho_indx_t *cabecalho, void *chave, int *qtd_reg_val);
    int busca_bin_str(void *vetor, cabecalho_indx_t *cabecalho, void *chave, int *qtd_reg_val);
    void shiftarVetIndxInt(void *vet_dado_indx, int pos, int qtd_reg);
    void shiftarVetIndxStr(void *vet_dado_indx, int pos, int qtd_reg);
    void fwriteStatusIndex(FILE *arq, cabecalho_indx_t *cabecalho);
 
#endif