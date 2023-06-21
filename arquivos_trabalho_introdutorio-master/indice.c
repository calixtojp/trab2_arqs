#include "indice.h"
#include "util.h"

// Cabeçalho das funções internas
int comparar_string (const void * a, const void * b);
int comparar_int (const void * a, const void * b);
int busca_binaria_string(Indice *meu_indice, char *valor, int esquerdo, int direito);
int busca_binaria_inteiro(Indice *meu_indice, int valor, int esquerdo, int direito);
void adequa_string_indice(char **string);
void inserir_cabecalho_indice(Indice *meu_indice, FILE *arq_indice);


// Funções disponíveis ao usuário

Indice *criar_indice(char *campo, char *tipo){
    /* Cria o a estrutura 'índice' segundo as especificaçõs do trabalho */
    
    Indice *meu_indice = malloc(sizeof(Indice));
    
    meu_indice->status = inconsistente;
    meu_indice->dados = NULL;
    meu_indice->numIndices = 0;
    strcpy(meu_indice->campoIndexado, campo);
    strcpy(meu_indice->tipoDado, tipo);

    return meu_indice;
}

int inserir_indice(Indice **meu_indice, Registro *meu_registro, long long int offset, int posicao){
    /* Funcao usada para inserir um indice em determianda posicao no vetor de índices */

    int inteiro_do_registro; // Inteiro que pode vir a ser inserido
    char *string_do_registro; // String que pode vir a ser inserida

    if(!strcmp((*meu_indice)->tipoDado, "inteiro")) // Se for do tipo inteiro
    {
        // Seleciono qual dos tipos inteiros
        inteiro_do_registro = selecionar_int(meu_registro, (*meu_indice)->campoIndexado);

        // Verifico se esse valor eh Nulo
        if(inteiro_do_registro == -1) return 0; // Retorno 0 se for nulo

        //Se nao for, dou realloc no vetor de registros e salvo o inteiro
        (*meu_indice)->dados = realloc((*meu_indice)->dados, (posicao + 1)* sizeof(Indice_dados));
        (*meu_indice)->dados[posicao].chaveInt = inteiro_do_registro;
    }
    else if(!strcmp((*meu_indice)->tipoDado, "string")) // Se for do tipo string
    {
        // Seleciono qual dos tipos strings
        string_do_registro = selecionar_string(meu_registro, (*meu_indice)->campoIndexado, 13);

        // Verifico se o valor eh nulo
        if(string_do_registro[0] == '\0' || string_do_registro[0] == '$') 
        {   
            free(string_do_registro);
            return 0; // Retorno 0 se for nulo
        }

        // Se nao for, aloco espaco
        (*meu_indice)->dados = realloc((*meu_indice)->dados, (posicao + 1)* sizeof(Indice_dados));
        // Preencho com $ se for necessario para completar 12 e coloco o \0
        adequa_string_indice(&string_do_registro);

        // Salva no indice
        strcpy((*meu_indice)->dados[posicao].chaveString, string_do_registro);

        free(string_do_registro);
    }
    // Salvo o offset desse registro e aumento o n_de_elementos do indice
    (*meu_indice)->dados[posicao].ByteOffset = offset;

    return 1; // Retorno 1 para indicar que foi um sucesso
}

void preencher_indice(FILE *arq_bin, Indice *meu_indice, Cabecalho *meu_cabecalho){
    /* Funcao para ler o binario e criar um indice na memoria */
    
    Registro *meu_registro;

    long long int bytes_lidos = 17, byteOffset; // Usados para contar a minha posicao no binario

    for(int i = 0; i < meu_cabecalho->nroRegArq; i++){ // Ate chegar ao ultimo registro
        
        byteOffset = bytes_lidos; // Salvo o valor do inicio do registro
        meu_registro = ler_registro_binario(arq_bin,&bytes_lidos); // Leio e aumento a posicao

        // Checar se o registro não está removido
        if(meu_registro->removido == nao_removido){ // Se nao estiver removido
            // Tento inserir, retorna 0 se nao inserir entao nao aumenta o numIndices
            meu_indice->numIndices += inserir_indice(&meu_indice, meu_registro, byteOffset, meu_indice->numIndices);
        }
        liberar_registro(meu_registro); // Desaloco memoria
    }
}

void ordenar_indice(Indice *meu_indice){
    /* Ordenação dos índices como especificado pelo trabalho, primeiro o campo depois byteOffset */

    // Utilizamos a função 'qsort' já implementada na biblioteca padrão do C
    int tamanho = meu_indice->numIndices;
    
    if(!strcmp(meu_indice->tipoDado,"inteiro")){
        qsort(meu_indice->dados,tamanho,sizeof(Indice_dados),comparar_int);
    }
    else{
        qsort(meu_indice->dados,tamanho,sizeof(Indice_dados),comparar_string);
    }
}

void ler_indice_binario(Indice *meu_indice, FILE *arq_indice){
    /* Função que lê um arquivo binário de índices e armazena em memória primária */

    int int_lido;
    char string_lida[13];
    long long int byteoffset_lido;

    // Aloca espaço no vetor de índices para a quantidade de índices no arquivo
    meu_indice->dados = calloc((meu_indice->numIndices),sizeof(Indice_dados));

    // Lerá todo arquivo de índice
    for(int i = 0; i < meu_indice->numIndices; i++){
        // Caso o tipo de dado do arquivo de índice é inteiro, lê o int e preenche campo int da célula
        if(!strcmp(meu_indice->tipoDado, "inteiro")){
            fread(&int_lido, sizeof(int), 1, arq_indice);
            meu_indice->dados[i].chaveInt = int_lido;
        }
        // Caso for string, lê string e preenche campo string da célula
        else if(!strcmp(meu_indice->tipoDado, "string")){
            fread(&string_lida, sizeof(char), 12, arq_indice);
            string_lida[12] = '\0';
            strcpy(meu_indice->dados[i].chaveString, string_lida);
        }

        // Lê ByteOffset e preenche na célula
        fread(&byteoffset_lido, sizeof(long long int), 1, arq_indice);
        meu_indice->dados[i].ByteOffset = byteoffset_lido;
    }

    return;
}

void liberar_indice(Indice *meu_indice){
    /* Liberar memória utilizada pela estrutura 'Indice'*/

    if(meu_indice == NULL) return;
    free(meu_indice->dados);
    free(meu_indice);
}

bool ler_cabecalho_indice(Indice *meu_indice, FILE *arq_indice){
    /* Função que lê o cabeçalho de um arquivo de índice e checa sua consistência */
    
    // return false; --> não foi possível ler (arquivo inconsistente, não existe, etc)
    // return true; --> foi possível ler (arquivo consistente, existe, etc)

    if(arq_indice == NULL){
        liberar_indice(meu_indice);
        return false;
    }

    // Lendo status (1 byte)
    fread(&(meu_indice->status), sizeof(char), 1, arq_indice);
    
    if(meu_indice->status == inconsistente){
        liberar_indice(meu_indice);
        fclose(arq_indice);
        return false;
    }

    // Lendo número de registros (4 bytes)
    fread(&(meu_indice->numIndices), sizeof(int), 1, arq_indice);

    return true;
}

void remover_noIndice(Indice *meu_indice, int pos){
    /* Remove algum campo do vetor de índices */
    
    // Realizamos um shift caso o elemento não estiver na última posição do vetor
    if((meu_indice->numIndices - 1) != pos){
        // vetor[i] recebe vetor[i + 1] até percorrer todo o vetor (n-1)
        for(int i = pos; i < meu_indice->numIndices - 1; i++) {
            meu_indice->dados[i] = meu_indice->dados[i+1];
        }
    }
    meu_indice->numIndices--;

    // Remodelando espaço ocupado
    meu_indice->dados = realloc(meu_indice->dados,(meu_indice->numIndices)*sizeof(Indice_dados));
}

void atualizar_indice(Registro *reg_nov, Registro *reg_ant, Indice **meu_ind, long long int offset_ant, long long int offset_nov, int posicao){
    /* Função utilizada para atualizar o campo de um índice no arquivo de índices */
    
    int valor_antigo, valor_novo, flag = 1;
    char *string_antiga, *string_nova;
    
    // Se for indice de inteiros
    if(!strcmp((*meu_ind)->tipoDado, "inteiro")){
        // Pego a informacao antiga e a informacao nova
        valor_antigo = selecionar_int(reg_ant, (*meu_ind)->campoIndexado);
        valor_novo = selecionar_int(reg_nov,(*meu_ind)->campoIndexado);

        if(valor_antigo == -1) // Se nao estava no indice antes
        {
            // Tento inserir no indice  a informacao inicial se ela for -1, retorna 0
            (*meu_ind)->numIndices += inserir_indice(meu_ind, reg_nov, offset_nov, (*meu_ind)->numIndices);
            return;
        }
        // A partir daqui o valor antigo nao eh nulo, ou seja, esta no indice 
        // Procuro a primeira vez que a informacao aparece se for indice, ja sei a posicao entao nao entro
        if(posicao == -1){ // Caso atualizacao sequencial, nao sei a posicao

            // Acho a primeira ocorrencia(mais a esquerda) do valor no indice
            posicao = busca_binaria_inteiro((*meu_ind), valor_antigo, 0, (*meu_ind)->numIndices);
            if(posicao == -1) return; // Nao deveria acontecer, pois ja esta no indice, usado por seguranca mesmo

            // Percorro ate achar uma que bata o offset
            for(posicao; posicao < (*meu_ind)->numIndices; posicao++){
                if( (*meu_ind)->dados[posicao].ByteOffset == offset_ant) break; // Encontrei

                if((*meu_ind)->dados[posicao].chaveInt != valor_antigo){ 
                    // Nao era para acontecer, pois ja passei pelo filtro e pelo nulo entao deveria ter no indice
                    // Usado para debugar e achar erro no indice
                    return;
                }
            }
        }
        if(valor_novo == -1) // Valor novo nulo, ou seja, vou remover do indice
        {
            remover_noIndice((*meu_ind), posicao);
            return;
        }
        // Se passou por tudo, so insiro a atualizacao sem remover ou adicionar no indice
        (*meu_ind)->dados[posicao].chaveInt = valor_novo;
        (*meu_ind)->dados[posicao].ByteOffset = offset_nov;
    }
    else{ // Se for indice de strings
        // Pego a informacao antiga e a informacao nova

        string_antiga = selecionar_string(reg_ant,(*meu_ind)->campoIndexado, 13);
        string_nova = selecionar_string(reg_nov,(*meu_ind)->campoIndexado, 13);

        if(string_antiga[0] == '\0' || string_antiga[0] == '$') // Se nao estava no indice antes, tento inserir
        {
            (*meu_ind)->numIndices += inserir_indice(meu_ind, reg_nov, offset_nov, (*meu_ind)->numIndices);
            free(string_antiga); // Desaloco memoria
            free(string_nova); // Desaloco memoria
            return;
        }
        // Adequo ela ao formato do indice, de 12 bytes e cifrao completando
        adequa_string_indice(&string_antiga);

        // Se for busca no indice ja sei a posicao, caso nao, encontro ela
        if(posicao == -1){

            // Procuro o primeiro local(mais a esquerda) em que aparece essa informacao
            posicao = busca_binaria_string((*meu_ind), string_antiga, 0, (*meu_ind)->numIndices);
            if(posicao == -1) return; // Nao deveria acontecer, pois ja esta no indice, usado por seguranca mesmo 
            
            // Percorro ate encontrar a posicao que bate o offset
            for(posicao; posicao < (*meu_ind)->numIndices; posicao++){
                if((*meu_ind)->dados[posicao].ByteOffset == offset_ant) break; // Encontrei
                if(strcmp((*meu_ind)->dados[posicao].chaveString, string_antiga))
                {
                    // Nao era para acontecer, pois ja passei pelo filtro e pelo nulo entao deveria ter no indice
                    // Usado para debugar e achar erro no indice
                    return;
                }
            }
         }
        if(string_nova[0] == '\0' || string_nova[0] == '$') // Valor novo nulo, ou seja, vou remover do indice
        {
            remover_noIndice((*meu_ind), posicao);
            free(string_antiga); // Desaloco memoria
            free(string_nova); // Desaloco memoria
            return;
        }
        
        // Adequo ela ao formato do indice, de 12 bytes e cifrao completando
        adequa_string_indice(&string_nova);

        // Se passou por tudo, so insiro a atualizacao sem remover ou adicionar no indice
        strcpy((*meu_ind)->dados[posicao].chaveString, string_nova);
        (*meu_ind)->dados[posicao].ByteOffset = offset_nov;
        free(string_antiga);
        free(string_nova);
    }
    
    return;
}

int achar_registro(Indice *meu_indice, char *tipoDado, char* valor){
    /* Dado um valor, buscar no arquivo de índice a primeira posição do vetor
    que possui tal valor */
    

    // Formatando o valor para o tamanho fixo de 12, caso seja menor que 12, o 
    // resto será preenchido por '$', caso seja maior, valor será truncado

    char *valor_formatado;
    valor_formatado = malloc(13);
    strncpy(valor_formatado,valor,12);

    for(int i = strlen(valor); i < 12; i++){
        valor_formatado[i] = '$';
    }
    valor_formatado[12] = '\0';

    int pos_indice;
    if(!strcmp(tipoDado,"string")){
        pos_indice = busca_binaria_string(meu_indice,valor_formatado,0,meu_indice->numIndices);
        free(valor_formatado);
        return pos_indice;
    }
    else{
        int valor_int = atoi(valor); //Convertendo string para int
        pos_indice = busca_binaria_inteiro(meu_indice,valor_int,0,meu_indice->numIndices);
        free(valor_formatado);
        return pos_indice;
    }
}

bool adequa_indice(Registro *meu_registro, char *campoIndexado, char *valor_buscado){
    /* Verifica se um registro possui o mesmo valor no campo indexado que valor buscado */
    
    if(!strcmp(campoIndexado,"idCrime")){
        /* Como o 'valor_buscado' é string, deve ser convertido para int */
        return meu_registro->idCrime == atoi(valor_buscado);
    }

    if(!strcmp(campoIndexado,"dataCrime")){
        // Para campos fixos de strings
        for(int i = 0; i < tam_dataCrime; i++){
            if(meu_registro->dataCrime[i] == '$' && valor_buscado[i] == '\0'){
                return true;
            }
            if(meu_registro->dataCrime[i] != valor_buscado[i]){
                // Se algum caracter for distinto, não são iguais
                return false;
            }
        }
        return true;
    }

    if(!strcmp(campoIndexado,"numeroArtigo")){
        // Mesmo caso que 'idCrime'
        return meu_registro->numeroArtigo == atoi(valor_buscado);
    }

    if(!strcmp(campoIndexado,"marcaCelular")){
        // Mesmo caso que 'dataCrime'
        for(int i = 0; i < tam_marcaCelular; i++){
            if(meu_registro->marcaCelular[i] == '$' && valor_buscado[i] == '\0'){
                return true;
            }
            if(meu_registro->marcaCelular[i] != valor_buscado[i])
            {
                return false;
            }
        }
        return true;
    }

    if(!strcmp(campoIndexado,"lugarCrime")){
        return !(strcmp(meu_registro->lugarCrime,valor_buscado));
    }

    if(!strcmp(campoIndexado,"descricaoCrime")){
        return !(strcmp(meu_registro->descricaoCrime,valor_buscado));
    }
}

bool remover_noArquivo(FILE *arq_bin, Registro *meu_registro, int byteOffset){
    /* Rescreve um dado registro como removido em um arquívo binário */
    
    if(meu_registro->removido == esta_removido) return false;
    
    // Alterando de não-removido para removido
    meu_registro->removido = esta_removido;
    
    // Retornando para reescrever registro
    if(byteOffset != -1) // Caso o cursor já estiver no local correto, não necessita outro fseek
        fseek(arq_bin,byteOffset,SEEK_SET);

    // Salve o registro novamente no arquivo
    salvar_registro(arq_bin,meu_registro,NULL);
    delimita_registro(arq_bin, NULL);

    // Foi removido
    return true;
}

void salvar_indice(FILE *arq_indice, Indice *meu_indice){
    /* Dado um vetor de índices em memória primária, escreve todo em um arquivo binário */
    
    if(arq_indice == NULL || meu_indice == NULL) return;

    // Volta para o início do arquivo se já não estiver lá
    if(ftell(arq_indice) != 0){
        fseek(arq_indice, 0, SEEK_SET);
    }

    // Escrevendo status
    fwrite(&(meu_indice->status),sizeof(char),1,arq_indice);
    // Escrevendo quantidade de índices
    fwrite(&(meu_indice->numIndices),sizeof(int),1,arq_indice);

    // Escrevendo índices
    for(int i = 0; i < meu_indice->numIndices; i++){
        if(!strcmp(meu_indice->tipoDado,"inteiro")){
            fwrite(&(meu_indice->dados[i].chaveInt),sizeof(int),1,arq_indice);
        }
        else{
            fwrite(&(meu_indice->dados[i].chaveString),sizeof(char),12,arq_indice);
        }
        fwrite(&(meu_indice->dados[i].ByteOffset),sizeof(long long int),1,arq_indice);        
    }
}


// Funções internas
int busca_binaria_string(Indice *meu_indice, char *valor, int esquerdo, int direito){
    /* Busca binária recursiva de uma string */
    
    // Caso não ache o valor
    if(esquerdo > direito) return -1;
    
    int meio = (direito + esquerdo)/2;
    int tam_valor = strlen(valor);

    if(!strcmp(meu_indice->dados[meio].chaveString,valor)){
        // Achando a primeira ocorrência do valor no arquivo de índice
        // (caminhando para direita o máximo possível)
        while(meio >= 0 && !strcmp(meu_indice->dados[meio].chaveString,valor)){
            meio--;
        }
        meio++;
        return meio;
    }
    else if(strcmp(meu_indice->dados[meio].chaveString,valor) > 0){
        return busca_binaria_string(meu_indice,valor,esquerdo,(meio-1));
    }
    else{
        return busca_binaria_string(meu_indice,valor,(meio+1),direito);
    }
}
int busca_binaria_inteiro(Indice *meu_indice, int valor, int esquerdo, int direito){
    /* Busca binária recursiva de um int */

    if(esquerdo > direito) return -1;
    
    int meio = (direito + esquerdo)/2;
    if(meu_indice->dados[meio].chaveInt == valor){
        while(meio >= 0 && meu_indice->dados[meio].chaveInt == valor){
            meio--;
        }
        meio++;
        return meio;
    }
    else if(meu_indice->dados[meio].chaveInt > valor){
        return busca_binaria_inteiro(meu_indice,valor,esquerdo,(meio-1));
    }
    else{
        return busca_binaria_inteiro(meu_indice,valor,(meio+1),direito);
    }
}
int comparar_string (const void * a, const void * b){
    /* Comparação de duas strings */
    
    // Convertendo o ponteiro para void para a struct 'Indice_dados'
    Indice_dados *ia = (Indice_dados *)a;
    Indice_dados *ib = (Indice_dados *)b;
    
    // Casos as strings forem diferentes, ordenada pela ordem alfabética
    if(strcmp(ia->chaveString,ib->chaveString)){
        return strcmp(ia->chaveString,ib->chaveString);
    }

    // Se forem iguais, ordena pelo byteOffset
    return (int)(ia->ByteOffset) - ia->ByteOffset;
}
int comparar_int (const void * a, const void * b){
    /* Comparação de dois inteiros */

    // Convertendo o ponteiro para void para a struct 'Indice_dados'
    Indice_dados *ia = (Indice_dados *)a;
    Indice_dados *ib = (Indice_dados *)b;

    // Casos os inteiros forem diferentes, ordenada pela ordem crescente
    if(ia->chaveInt != ib->chaveInt){
        return (int)(ia->chaveInt - ib->chaveInt);
    }

    // Se forem iguais, ordena pelo byteOffset
    return (int)(ia->ByteOffset) - ia->ByteOffset;
}
void adequa_string_indice(char **string){
    /* Torna a string no formato fixo de 12 bytes do índice*/
    
    // Caso ocupe menos que 12 bytes, o resto é preenchido por '$'
    for(int i = strlen((*string)); i < 12; i++){
        (*string)[i] = '$';
    }
}
void inserir_cabecalho_indice(Indice *meu_indice, FILE *arq_indice){
    /* Insere apenas o cabeçalho da estrutura 'Indice' em um arquivo */
    
    if(meu_indice == NULL) erro_1();
    
    fwrite (&meu_indice->status,sizeof(char), 1, arq_indice);
    fwrite(&meu_indice->numIndices, sizeof(int), 1, arq_indice);
}