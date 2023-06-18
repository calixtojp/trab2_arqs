#ifndef _AUXILIARES
#define _AUXILIARES

    typedef int (*FncComparacao) (void*,int,void*);

	void binarioNaTela(char *nomeArquivoBinario);
	void readline(char *string);
	void ler_aspas_string(char *str);
	void erro(void *ponteiro);
	void mensagem_erro();
	char **alocar_vetor_string(int n_elem, int n_chars);
	void desalocar_vetor_string(char **vetor, int n_elem);
	int ehInteiro(char *campo);
	char *truncar(char *original, int tam_truncamento);
	char *truncar_dataCrime(char *original);
	char *truncar_marcaCelular(char *original);
	int strParaInt(char *str);
	int comparar_strings(char *s1, char *s2);
	int comparar_n_chars(char *s1, char *s2, int n);
	void copia_n_chars(char *str1, char *str2, int n);
	int tamanhoStr(char *string);
	char *alocar_nome(int tamanho_nome);
	void pular_campos_fixos(FILE *arq_bin);
	void pular_ate_fim_registro(FILE *arq_bin);
	void pular_registro_removido(FILE *arq_bin);
	void pular_cabecalho_csv(FILE *arq);
	void mensagem_NULO(void);
	void ignorar_palavra_stdin(void);
	int busca_bin_rec(void *vetor, int ini, int fim, void *chave, FncComparacao comparacao);
	size_t meu_fwrite(void *__restrict__ __ptr, size_t __size, size_t __n, FILE *__restrict__ __s);
	size_t meu_fread(void *__restrict__ __ptr, size_t __size, size_t __n, FILE *__restrict__ __s);

#endif