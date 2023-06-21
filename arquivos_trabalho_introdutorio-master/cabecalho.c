#include "cabecalho.h"
#include "util.h"

Cabecalho *criar_cabecalho(){
    /* Cria o cabecalho assim como as especificacoes do trabalho */
    
    Cabecalho *meu_cabecalho = malloc(sizeof(Cabecalho));
    
    meu_cabecalho->status = inconsistente; // Inicia como inconsistente
    meu_cabecalho->proxByteOffset = 17; // Adiciona-se o tamanho do cabeçalho
    meu_cabecalho->nroRegArq = 0;
    meu_cabecalho->nroRegRem = 0;

    return meu_cabecalho;
}

Cabecalho* ler_cabecalho(FILE *arq_bin){
    /*
    . Função reponsável por ler os primeiros 17 bytes de um arquivo binário e
    preencher uma estrutura de cabeçalho com os campos lidos
    . Verifica consistência do arquivo  
    */

    Cabecalho *meu_cabecalho;
    meu_cabecalho = malloc(sizeof(Cabecalho));

    fread(&(meu_cabecalho->status),1,sizeof(meu_cabecalho->status),arq_bin); // 1 byte
    if((meu_cabecalho->status == inconsistente))
    {
        fclose(arq_bin);
        free(meu_cabecalho);
        erro_1();
    }
    fread(&(meu_cabecalho->proxByteOffset),1,sizeof(meu_cabecalho->proxByteOffset),arq_bin); // 8 bytes
    fread(&(meu_cabecalho->nroRegArq),1,sizeof(meu_cabecalho->nroRegArq),arq_bin); // 4 bytes
    fread(&(meu_cabecalho->nroRegRem),1,sizeof(meu_cabecalho->nroRegRem),arq_bin); // 4 bytes

    return meu_cabecalho;
}

void escrever_cabecalhoBinario(FILE *arq_bin, Cabecalho *meu_cabecalho){
    /* Função que reescreve o cabeçalho no arquivo binário */

    // Vai para o início do arquivo se já não estiver lá
    if(ftell(arq_bin) != 0){
        fseek(arq_bin,0,SEEK_SET);
    }
    fwrite(&(meu_cabecalho->status), sizeof(char), 1, arq_bin);
    fwrite(&(meu_cabecalho->proxByteOffset), sizeof(long long int), 1, arq_bin);
    fwrite(&(meu_cabecalho->nroRegArq), sizeof(int), 1, arq_bin);
    fwrite(&(meu_cabecalho->nroRegRem), sizeof(int), 1, arq_bin);
}

void imprimir_cabecalho(Cabecalho *meu_cabecalho){
    /* Função não utilizada no código, feita para debugação */

    printf("Status: %c\n",meu_cabecalho->status);
    printf("ProxByteOffset: %lld\n",meu_cabecalho->proxByteOffset);
    printf("Registros: %d\n",meu_cabecalho->nroRegArq);
    printf("Removidos: %d\n",meu_cabecalho->nroRegRem);
}