#ifndef _IND_H
#define _IND_H

    #include "registro.h"

    typedef struct indice_dados_t{
        long long int ByteOffset;
        // Como o valor do índice é ou string ou int, utilizamos union
        union{
            int chaveInt;
            char chaveString[13];
        };
    }Indice_dados;

    typedef struct indice_t{
        // Todas as partes do cabeçalho do índice
        char status;
        int numIndices;
        char campoIndexado[20];
        char tipoDado[10];
        // Estrutura de dados utilizada para armazenar os índices é um vetor que será dinamicamente alocado
        Indice_dados *dados;
    }Indice;


    // Estruturas utilizadas
    typedef struct registro_t Registro;
    typedef struct cabecalho_t Cabecalho;
    

    // Criação do índice
    Indice *criar_indice(char *campo, char *tipo);
    void preencher_indice(FILE *arq_bin, Indice *meu_indice, Cabecalho *meu_cabecalho);
    void ordenar_indice(Indice *meu_indice);
    
    // Leitura de um índice
    bool ler_cabecalho_indice(Indice *meu_indice, FILE *arq_indice);
    void ler_indice_binario(Indice *meu_indice, FILE *arq_indice);

    // Preenchendo um índice
    int inserir_indice(Indice **meu_indice, Registro *meu_registro, long long int offset, int posicao);
    void atualizar_indice(Registro *reg_nov, Registro *reg_ant, Indice **meu_ind, long long int offset_ant, long long int offset_nov, int posicao);

    // Busca por um índice
    int achar_registro(Indice *meu_indice, char *tipoDado, char* valor);
    bool adequa_indice(Registro *meu_registro, char *campoIndexado, char *valor);
    
    // Remoção de índices
    bool remover_noArquivo(FILE *arq_bin, Registro *meu_registro, int pos);
    void remover_noIndice(Indice *meu_indice, int pos);

    // Salvar em binário e desalocar índice
    void salvar_indice(FILE *arq_indice, Indice *meu_indice);
    void liberar_indice(Indice *meu_indice);

#endif