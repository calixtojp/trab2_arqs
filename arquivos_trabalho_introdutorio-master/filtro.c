#include "filtro.h"
#include "util.h"

// Cabeçalho de função interna
void preencher_campo_filtro(Filtro *meu_filtro, char* campo, char* valor);


// Funções disponíveis ao usuário

Filtro* criar_filtro(){
    /* Aloca e inicializa estrutura 'Filtro' */
    
    Filtro *meu_filtro = NULL;
    meu_filtro = malloc(sizeof(Filtro));
    
    // Todos os campos necessitam ser ao menos NULOS para funcionamento de posterior comparação
    meu_filtro->idCrime         = NULL;
    meu_filtro->dataCrime       = NULL;
    meu_filtro->numeroArtigo    = NULL;
    meu_filtro->marcaCelular    = NULL;
    meu_filtro->lugarCrime      = NULL;
    meu_filtro->descricaoCrime  = NULL;
    
    return meu_filtro;
}

bool preencher_filtro(Filtro *meu_filtro, char *campoIndexado,char **valor_buscado){
    /* Função que preenche o filtro com os campos e valores inseridos pelo usuário */

    /*
    Ex:
    Antes:
    meu_filtro->idCrime         = NULL;
    meu_filtro->dataCrime       = NULL;
    meu_filtro->numeroArtigo    = NULL;
    meu_filtro->marcaCelular    = NULL;
    meu_filtro->lugarCrime      = NULL;
    meu_filtro->descricaoCrime  = NULL;

    Entrada
    3 lugarCrime "SAO PAULO" numeroArtigo 171 dataCrime "28/02/2024"

    Depois:
    meu_filtro->idCrime         = NULL;
    meu_filtro->dataCrime       = "28/02/2024";
    meu_filtro->numeroArtigo    = 171;
    meu_filtro->marcaCelular    = NULL;
    meu_filtro->lugarCrime      = "SAO PAULO";
    meu_filtro->descricaoCrime  = NULL;

    */
    
    char campo[tam_buffer];
    char valor[tam_buffer];
    bool buscou_indice = false; 

    int qtd_pares;
    scanf("%d",&qtd_pares);   
    for(int i = 0; i < qtd_pares; i++){
        
        scanf("%s",campo);
        if((!strcmp(campo,"idCrime")) || (!strcmp(campo,"numeroArtigo")) ){
            scanf("%s",valor);
        }
        else{
            // Caso o campo indexado for string, utiliza função disponibilizada
            scan_quote_string(valor);
        }

        // Caso o usuário buscar por um campo indexado, salva o valor buscado para
        // posterior busca no arquivo de índice
        if(!strcmp(campo,campoIndexado)){
            *valor_buscado = malloc(strlen(valor)+1); // + 1 para '\0'
            strcpy(*valor_buscado,valor);
            // Para sinalizar que a busca no índice será necessária
            buscou_indice = true;
        }

        preencher_campo_filtro(meu_filtro,campo,valor);
    }

    return buscou_indice;
}

bool adequa_filtro(Registro *meu_registro, Filtro *meu_filtro){
    /* Compara um registro com um filtro */
    
    if(meu_filtro->idCrime != NULL){
        if(meu_registro->idCrime != *(meu_filtro->idCrime)){
            // Se for diferente, já não se adéqua ao filtro, retorna falso
            return false;
        }
    }

    if(meu_filtro->dataCrime != NULL){
        // Para maior segurança, strncmp é preferível, visto que o tamanho é fixo (10 bytes)
        if(strncmp(meu_registro->dataCrime,meu_filtro->dataCrime,tam_dataCrime)){
            return false;
        }
    }

    if(meu_filtro->numeroArtigo != NULL){
        if(meu_registro->numeroArtigo != *(meu_filtro->numeroArtigo)){
            return false;
        }
    }

    if(meu_filtro->marcaCelular != NULL){
        // Mesmo caso do dataCrime, tamanho fixo (12 bytes)
        if(strncmp(meu_registro->marcaCelular,meu_filtro->marcaCelular,tam_marcaCelular)){
            return false;
        }
    }

    if(meu_filtro->lugarCrime != NULL){
        if(strcmp(meu_registro->lugarCrime,meu_filtro->lugarCrime)){
            return false;
        }
    }

    if(meu_filtro->descricaoCrime != NULL){
        if(strcmp(meu_registro->descricaoCrime,meu_filtro->descricaoCrime)){
            return false;
        }
    }

    // Caso chegue aqui, o registro passou no filtro
    return true;
}

void liberar_filtro(Filtro* meu_filtro){
    /* Caso não haja especificação de filtro para algum campo, esse não precisa
    ser desalocado, ao contrário, será */
    
    if(meu_filtro->idCrime != NULL)
        free(meu_filtro->idCrime);
    
    if(meu_filtro->dataCrime != NULL)
        free(meu_filtro->dataCrime);
    
    if(meu_filtro->numeroArtigo != NULL)
        free(meu_filtro->numeroArtigo);
    
    if(meu_filtro->marcaCelular != NULL)
        free(meu_filtro->marcaCelular);
    
    if(meu_filtro->lugarCrime != NULL)
        free(meu_filtro->lugarCrime);
    
    if(meu_filtro->descricaoCrime != NULL)
        free(meu_filtro->descricaoCrime);
    
    free(meu_filtro);
}


// Função interna
void preencher_campo_filtro(Filtro *meu_filtro, char* campo, char* valor){
    /* Preenche um dado campo de um filtro com um certo valor */
    
    if(!strcmp(campo,"idCrime")){
        meu_filtro->idCrime = malloc(sizeof(int));
        *(meu_filtro->idCrime) = atoi(valor); // Converssão para Int
    }
    else if(!strcmp(campo,"dataCrime")){
        // Data tem tamanho fixo de 10(12/02/2004) + 1 ('\0') = 11
        meu_filtro->dataCrime = malloc(11*sizeof(char));
        strcpy(meu_filtro->dataCrime,valor);
    }
    else if(!strcmp(campo,"numeroArtigo")){
        meu_filtro->numeroArtigo = malloc(sizeof(int));
        *(meu_filtro->numeroArtigo) = atoi(valor);
    }
    /*
    Apesar do campo 'marcaCelular' ser de tamanho fixo de 12, o tamanho real da 
    marca é variável, exemplo: ASUS, SAMSUNG, APPLE. Assim, deve-se preencher o 
    campo do filtro com a marca em sí, e o resto dos 12 chars com '$', para o 
    correto funcionamento de comparação posterior
    */
    else if(!strcmp(campo,"marcaCelular")){
        // Tamanho da string (12) + '\0' (1) = 13 chars
        meu_filtro->marcaCelular = malloc(13);
        strcpy(meu_filtro->marcaCelular,valor);

        /* 
        Preenchendo o que não foi preenchido com '$'
        0 1 2 3 4 5 6  7  8  9 10 11 12      
        S A M S U N G \0  _  _  _  _  _ 
        ==> 
        0 1 2 3 4 5 6  7  8  9 10 11 12
        S A M S U N G  $  $  $  $  $ \0 
        */

        for(int i = strlen(meu_filtro->marcaCelular); i < 12; i++){
            meu_filtro->marcaCelular[i] = '$';
        }
        meu_filtro->marcaCelular[12] = '\0';
    }
    else if(!strcmp(campo,"lugarCrime")){
        meu_filtro->lugarCrime = malloc((strlen(valor)+1)*sizeof(char));
        strcpy(meu_filtro->lugarCrime,valor);
    }
    else if(!strcmp(campo,"descricaoCrime")){
        meu_filtro->descricaoCrime = malloc((strlen(valor)+1)*sizeof(char));
        strcpy(meu_filtro->descricaoCrime,valor);
    }

    // Caso o usuário informar algum campo inexistente, acarretará em erro de processamento
    else{
        erro_1();
    }
}