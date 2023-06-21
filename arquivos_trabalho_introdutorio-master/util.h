#ifndef _UTIL_H
#define _UTIL_H

    /* Arquivo com funções utilitárias usadas em diferentes partes do projeto */

    #include "registro.h"

    // Manipulação de arquivos
    void abrir_arq(FILE **arquivo, char *nome_arquivo, char* modo);
    void estabiliza_arq(FILE *arq);
    void desestabiliza_arq(FILE *arq, int proxByteOffset);

    // Manipulação da leitura de um .csv
    char* ler_linha(FILE *arq_in, int tamanho_linha);
    void ignorar_linha(FILE *arq_in, int tamanho_linha);

    // Obter algum campo de um registro
    int selecionar_int(Registro *meu_registro, char *campoIndexado);
    char *selecionar_string(Registro *meu_registro, char *campoIndexado, int tamanho);
    
    // Manipulacao de vetores inteiros
    void copia_vetor(int *dest, int *fonte, int tamanho);
    
    // Mensagem de erro geral, segundo a especificação
    void erro_1();

#endif
