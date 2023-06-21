#include "util.h"

void abrir_arq(FILE **arquivo, char *nome_arquivo, char* modo){
    /* Função para abrir arquivos com algum modo especificado */
    
    (*arquivo) = fopen(nome_arquivo, modo);
    
    // Não foi possível abrir o arquivo => "Falha no processamento do arquivo"
    if(*arquivo == NULL) 
        erro_1();
}
void estabiliza_arq(FILE *arq){
    /* Função para estabilizar arquivos (mudar status para consistente) */

    if(arq == NULL) return;
    
    char status = consistente;
    fseek(arq, 0 ,SEEK_SET);
    fwrite(&status,sizeof(char),1,arq);
}
void desestabiliza_arq(FILE *arq, int proxByteOffset){
    /* Função para desestabilizar arquivos (mudar status para inconsistente) */

    if(arq == NULL) return;

    // Escrevendo inconsistente no primeiro byte do arquivo
    char status = inconsistente;
    fseek(arq, 0 ,SEEK_SET);
    fwrite(&status,sizeof(char),1,arq);

    // Caso deseje mover o cursor para algum byte após desestabilização do arquivo
    if(proxByteOffset != -1){
        fseek(arq,proxByteOffset,SEEK_SET);
    }
}

void ignorar_linha(FILE *arq_in, int tamanho_linha){
    /* Função para ignorar a primeira linha de todo .csv */
    char buffer[1024];
    fgets(buffer, sizeof(buffer), arq_in);
}

char* ler_linha(FILE *arq_in, int tamanho_linha){
    /* Função para ler uma linha do .csv, correspondente a 1 registro */
    char *buffer;
    buffer = malloc(tamanho_linha);

    // fgets -> NULL, caso a linha estiver completamente vazia (apenas EOF) 
    // fgets -> *ptr, caso contiver qualquer coisa (incluso '\n')
    if(fgets(buffer, tamanho_linha, arq_in) != NULL){
        return buffer;
    }else{
        free(buffer);
        return NULL;
    }
}

int selecionar_int(Registro *meu_registro, char *campoIndexado){
    /* Funcao usada para retornar o inteiro de um campo escolhido em um registro */
    
    if(!strcmp(campoIndexado,"idCrime")){ // Compara com o campo dado
        return meu_registro->idCrime;
    }
    else if(!strcmp(campoIndexado,"numeroArtigo")){ // Compara com o campo dado
        return meu_registro->numeroArtigo;
    }
}
char *selecionar_string(Registro *meu_registro, char *campoIndexado, int tamanho){
    /* Funcao usada para retornar uma string de um campo escolhido em um registro de determinado tamanho */

    char *string_copia = malloc(tamanho * sizeof(char));

    if(!strcmp(campoIndexado,"dataCrime")){ // Compara com o campo dado
        strncpy(string_copia, meu_registro->dataCrime, tamanho); // Copia os tamanho bytes e insere \0 no final
        string_copia[tamanho - 1] = '\0';
        return string_copia;
    }
    else if(!strcmp(campoIndexado,"marcaCelular")){
        strncpy(string_copia, meu_registro->marcaCelular, tamanho);
        string_copia[tamanho - 1] = '\0';
        return string_copia;
    }
    else if(!strcmp(campoIndexado,"lugarCrime")){
        strncpy(string_copia, meu_registro->lugarCrime, tamanho);
        string_copia[tamanho - 1] = '\0';
        return string_copia;
    }
    else if(!strcmp(campoIndexado,"descricaoCrime")){
        strncpy(string_copia, meu_registro->descricaoCrime, tamanho);
        string_copia[tamanho - 1] = '\0';
        return string_copia;
    }
}
void copia_vetor(int *dest, int *fonte, int tamanho){
    for(int i = 0; i < tamanho; i++){
        dest[i] = fonte[i];
    }
}

void erro_1(){
    /* Ação de erro segundo especificação do trabalho */
    
    printf("Falha no processamento do arquivo.\n");
    exit(0);
}