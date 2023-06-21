#ifndef _CAB_H
#define _CAB_H
    
    #include <stdio.h>
    #include <stdlib.h>


    typedef struct cabecalho_t{
        char status;
        long long int proxByteOffset;
        int nroRegArq;
        int nroRegRem;
    }Cabecalho;


    Cabecalho *criar_cabecalho();
    Cabecalho* ler_cabecalho(FILE *arq_bin);
    void escrever_cabecalhoBinario(FILE *arq_bin, Cabecalho *meu_cabecalho);
    void imprimir_cabecalho(Cabecalho *header);

#endif
