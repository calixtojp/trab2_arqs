#ifndef _REG_H
#define _REG_H


    #include "cabecalho.h"
    #include <stdbool.h>
    #include <string.h>

    // Utilizamos alguns 'defines' para tornar o uso de algumas contantes mais legível ao decorrer do projeto

    #define delimitador_campo   "|"
    #define delimitador_reg     "#"
    #define enchimento          "$"
    
    #define esta_removido       '1'
    #define nao_removido        '0'
    #define consistente         '1'
    #define inconsistente       '0'

    #define tam_buffer          100
    #define tam_dataCrime       10
    #define tam_marcaCelular    12

    #define ID_CRIME            1 
    #define DATA_CRIME          2
    #define NUMERO_ARTIGO       3
    #define LUGAR_CRIME         4
    #define DESCRICAO_CRIME     5
    #define MARCA_CELULAR       6


    // Estrutura de um registro, segundo especificações do trabalho

    typedef struct registro_t{
        char removido;
        int idCrime;
        char dataCrime[11];// Mais um de espaco para o '\0'
        int numeroArtigo;
        char marcaCelular[13];// Mais um de espaco para o '\0'
        char *lugarCrime;
        char *descricaoCrime;
    }Registro;


    // Estruturas necessárias
    typedef struct indice_t Indice;
    typedef struct filtro_t Filtro;


    Registro *criar_registro();
    Registro* ler_registro_binario(FILE *arq_bin, long long int *tam_lido);
    bool imprimir_registro(Registro *data);

    void preencher_registro(Registro* meu_registro, char* buffer);
    void salvar_registro(FILE *arq_bin, Registro *info, Cabecalho *meu_cabecalho);
    void delimita_registro(FILE *arq_bin, Cabecalho *meu_cabecalho);
    void inserir_final(FILE *arq_bin, FILE *arq_index, Indice **meu_indice, Cabecalho *meu_cabecalho);
    void liberar_registro(Registro *meu_registro);

    // Para atualização
    void insere_atualizacao(Registro **meu_registro, Filtro *meu_filtro);
    void atualizacao_ind(FILE *arq_bin, Filtro *meu_filtro, Filtro *filtro_atu, Cabecalho *meu_cabecalho,
    Indice *meu_indice, char *valor_buscado, bool flag_atu);
    void atualizacao_seq(FILE *arq_bin, Filtro *meu_filtro, Filtro *filtro_atu, Cabecalho *meu_cabecalho,
    Indice *meu_indice, bool flag_atu);

    // Para busca
    void busca_indice(FILE *arq_bin, Filtro *meu_filtro, Indice *meu_indice, char *valor_buscado);
    void busca_sequencial(FILE *arq_bin, Filtro *meu_filtro, Cabecalho *meu_cabecalho);

    // Para remoção
    void remocao_indice(FILE *arq_bin, Filtro *meu_filtro, Cabecalho *meu_cabecalho, Indice *meu_indice, char *valor_buscado);
    void remocao_sequencial(FILE *arq_bin, Filtro *meu_filtro, Cabecalho *meu_cabecalho, Indice *meu_indice,char *valor_buscado);


    /* ARVORE B * */
    Registro* ler_registro_teclado(FILE *arq_bin);

#endif