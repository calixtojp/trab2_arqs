#include "registro.h"
#include "filtro.h"
#include "indice.h"
#include "util.h"

// Cabeçalho das funções internas
Registro* ler_registro_teclado(FILE *arq_bin);
Registro *atualizar_registro(Registro *registro_antigo, Filtro *filtro_atualiza, int *tamanho_novo);
int tamanho_registro(Registro *meu_registro);
void preencher_campos(Registro *meu_registro,char *dados, int campo_atual);
void registro_copia(Registro **destino, Registro *fonte);
void enchimento_registro(FILE *arq_bin, int qtd_enchimento);
void escreve_tamFixo(FILE *arq_bin, char *string, int tam);
void escreve_tamVariavel(FILE *arq_bin, char *string);

// Funções disponíveis ao usuário

Registro *criar_registro(){
    /* Inicialização da estrutura onde cada registro do .csv será armazenado*/

    Registro *meu_registro;
    meu_registro = malloc(sizeof(Registro));

    // Necessário para evitar aviso do valgrind, inicializando todos os bytes como 0
    memset(meu_registro,0,sizeof(Registro));
    
    // Inicialziando o registro como 'não removido'
    meu_registro->removido = nao_removido;

    // Inicializando os ponteiros
    meu_registro->lugarCrime = NULL;
    meu_registro->descricaoCrime = NULL;
    
    return meu_registro;
}

Registro* ler_registro_binario(FILE *arq_bin, long long int *tam_lido){
    /* 
    Lê uma sequência de 'x' bytes do arquivo binário e preenche uma estrutura 
    de registro com os campos lidos contando o tamanho lido se o ponteiro for diferente de NULL
    */
    
    // Inicializando e alocando estrutura onde os dados lidos serão armazenados
    Registro *meu_registro;
    meu_registro = criar_registro();
    
    
    /*Leitura*/

    // Não podem ser nulos, Lê normalmente e aumenta o tamanho
    // status
    fread(&(meu_registro->removido),1,sizeof(meu_registro->removido),arq_bin);
    if(tam_lido != NULL)
        (*tam_lido) += 1;
    // idCrime
    fread(&(meu_registro->idCrime),1,sizeof(meu_registro->idCrime),arq_bin);
    if(tam_lido != NULL)
        (*tam_lido) += 4;

    // Caso dataCrime nulo, campo será preenchido por 10 chars de '$'
    fread(&(meu_registro->dataCrime),1, tam_dataCrime,arq_bin);
    meu_registro->dataCrime[tam_dataCrime] = '\0';
    if(tam_lido != NULL)
        (*tam_lido) += 10;

    // Caso numeroArtigo nulo, campo será preenchido por -1
    fread(&(meu_registro->numeroArtigo),1,sizeof(meu_registro->numeroArtigo),arq_bin);
    if(tam_lido != NULL)
        (*tam_lido) += 4;

    /* MarcaCelular pode não preencher todos os 12 chars ou ser nulo, em ambos
    os casos, o espaço livre será preenchido por '$' */
    fread(&(meu_registro->marcaCelular),1, tam_marcaCelular,arq_bin);
    meu_registro->marcaCelular[tam_marcaCelular] = '\0';
    if(tam_lido != NULL)
        (*tam_lido) += 12;
    

    /* 
    Leitura dos campos de tamanho variável, necessário alocação dinâmica 
    - Lê byte a byte até encontrar um delimitador de campo '|', preenche o que
    foi lido no campo da estrutura 
    */

    // LugarCrime
    int qtd_chars = 0;
    char letra_atual;
    do{
        fread(&letra_atual,1,sizeof(char),arq_bin);
        if(letra_atual != '|'){
            meu_registro->lugarCrime = realloc(meu_registro->lugarCrime,(++qtd_chars)*sizeof(char));
            meu_registro->lugarCrime[qtd_chars-1] = letra_atual;
        }
    }while(letra_atual != '|');
    meu_registro->lugarCrime = realloc(meu_registro->lugarCrime,(++qtd_chars)*sizeof(char));
    meu_registro->lugarCrime[qtd_chars-1] = '\0'; // Transformando em string imprimível
    if(tam_lido != NULL)
        (*tam_lido) += qtd_chars;
    

    // DescriçãoCrime
    qtd_chars = 0;
    do{
        fread(&letra_atual,1,sizeof(char),arq_bin);
        if(letra_atual != '|'){
            meu_registro->descricaoCrime = realloc(meu_registro->descricaoCrime,(++qtd_chars)*sizeof(char));
            meu_registro->descricaoCrime[qtd_chars-1] = letra_atual;
        }
    }while(letra_atual != '|');
    meu_registro->descricaoCrime = realloc(meu_registro->descricaoCrime,(++qtd_chars)*sizeof(char));
    meu_registro->descricaoCrime[qtd_chars-1] = '\0'; // Transformando em string imprimível
    if(tam_lido != NULL)
        (*tam_lido) += qtd_chars;
    

    // Consumindo, byte a byte, o lixo de memoria até que o delimitador de registro (char '#') sejá lido
     do{
        fread(&letra_atual,1,sizeof(char),arq_bin);
        if(tam_lido != NULL)
            (*tam_lido)++;
    }while(letra_atual != '#');
    

    // Retornando registro totalmente preenchido
    return meu_registro;
}

bool imprimir_registro(Registro *meu_registro){
    /* Função utilizada para imprimir os registros como especificado pelo trabalho */

    // Se o registro está marcado como removido, não é imprimido
    if(meu_registro->removido == esta_removido) return false;
    

    /*Impressão*/

    // idCrime não pode ser nulo
    printf("%d, ",meu_registro->idCrime);
    
    // Tratando nulo da dataCrime
    if(meu_registro->dataCrime[0] == '$'){
        printf("NULO, ");
    }
    else{
        printf("%s, ",meu_registro->dataCrime);
    }

    // Tratando nulo do nroArtigo
    if(meu_registro->numeroArtigo == -1){
        printf("NULO, ");
    }
    else{
        printf("%d, ",meu_registro->numeroArtigo);
    }

    // Tratando nulo do lugarCrime
    if(meu_registro->lugarCrime[0] == '\0'){
        printf("NULO, ");
    }
    else{
        printf("%s, ",meu_registro->lugarCrime);
    }

    // Tratando nulo da descriçãoCrime
    if(meu_registro->descricaoCrime[0] == '\0'){
        printf("NULO, ");
    }
    else{
        printf("%s, ",meu_registro->descricaoCrime);
    }
    
    // Tratando nulo da marcaCelular
    if(meu_registro->marcaCelular[0] == '$'){
        printf("NULO\n");
    }
    else{
        for(int i = 0; i < 12; i++){
            if(meu_registro->marcaCelular[i] != '$'){
                printf("%c",meu_registro->marcaCelular[i]);
            }
        }
        printf("\n");
    }

    // Foi possível imprimir, registro não está removido
    return true;
}

void preencher_registro(Registro* meu_registro, char* buffer){
    /* Função utilizada para separar a linha em campos (strsep), similar a strtok */
    char *token, *ptr_buffer = buffer;
    int campo_atual = 1;
    if(ptr_buffer != NULL){
        token = strsep(&ptr_buffer, ",");
        while(token != NULL){
            preencher_campos(meu_registro,token,campo_atual++);
            token = strsep(&ptr_buffer, ",");
        }
    }
}

void salvar_registro(FILE *arq_bin, Registro *meu_registro, Cabecalho *meu_cabecalho){
    /* 
    Funcao para salvar um registro lido no arquivo binario, caso nao seja 
    necessário alterar o byteOffset do cabeçalho, passamos como nulo
    */
    
    if(meu_registro == NULL) return;
    
    // Escreve o campo 'removido' (1 byte)
    fwrite(&(meu_registro->removido), sizeof(char), 1, arq_bin);    
    // Escreve o campo 'idCrime' (4 bytes)
    fwrite(&(meu_registro->idCrime), sizeof(int), 1, arq_bin);
    if(meu_cabecalho != NULL)
        meu_cabecalho->proxByteOffset += 5;

    // Escreve o campo de tamanho fixo (10 bytes) 'dataCrime'
    escreve_tamFixo(arq_bin, meu_registro->dataCrime, 10);
    if(meu_cabecalho != NULL)
        meu_cabecalho->proxByteOffset += 10;

    // Escreve 'numeroArtigo' (4 bytes) 
    fwrite(&(meu_registro->numeroArtigo), sizeof(int), 1, arq_bin);
    if(meu_cabecalho != NULL)
        meu_cabecalho->proxByteOffset += 4;
    
    // Escreve 'marcaCelular' (12 bytes) 
    escreve_tamFixo(arq_bin, meu_registro->marcaCelular, 12);
    if(meu_cabecalho != NULL)
        meu_cabecalho->proxByteOffset += 12;
    
    // Escreve 'lugarCrime' de tamanho variável
    escreve_tamVariavel(arq_bin, meu_registro->lugarCrime);
    if(meu_cabecalho != NULL)
        meu_cabecalho->proxByteOffset += strlen(meu_registro->lugarCrime) + 1; // + delimitador de campo (1 byte)

    // Escreve 'descricaoCrime' de tamanho variável
    escreve_tamVariavel(arq_bin, meu_registro->descricaoCrime);
    if(meu_cabecalho != NULL)
        meu_cabecalho->proxByteOffset += strlen(meu_registro->descricaoCrime) + 1; // + delimitador de campo (1 byte)


    // Atualiza quantidade de registros no cabeçalho se necessário (!= NULL)
    if(meu_cabecalho != NULL)
        meu_cabecalho->nroRegArq++;
}

void delimita_registro(FILE *arq_bin, Cabecalho *meu_cabecalho){
    /* Coloca o # no final do registro, está função é utilizada em conjunto com a 'salvar_registro' */
    fwrite(delimitador_reg, sizeof(char), 1, arq_bin);
    if(meu_cabecalho != NULL){
        meu_cabecalho->proxByteOffset += 1; // + delimitador de campo (1 byte)
    }
}

void inserir_final(FILE *arq_bin, FILE *arq_index, Indice **meu_indice, Cabecalho *meu_cabecalho){
    /* Le um registro do teclado e insere no final do arquivo, requer arquivo de indice sequencial */
    
    Registro *meu_registro;
    long long int offSet = meu_cabecalho->proxByteOffset; // Local onde sera inserido(final do arquivo)
    meu_registro = ler_registro_teclado(arq_bin); // Leio o registro do teclado
    
    salvar_registro(arq_bin,meu_registro,meu_cabecalho); // O fseek ja foi dado pela funcionalidade 6
    delimita_registro(arq_bin, meu_cabecalho); // Marco o fim do registro
    
    // Insiro o novo registro no indice, aumentando o num de indices se a inserção for um sucesso
    (*meu_indice)->numIndices += inserir_indice(meu_indice, meu_registro, offSet, (*meu_indice)->numIndices);
    
    // Desaloco a memoria
    liberar_registro(meu_registro);

    return;
}

void liberar_registro(Registro *meu_registro){
    /* Liberar os campos alocados dinamicamente e a estrutura 'Registro' */

    if(meu_registro == NULL) return;
    
    free(meu_registro->lugarCrime);
    free(meu_registro->descricaoCrime);
    free(meu_registro);
}

// Para atualização
void insere_atualizacao(Registro **meu_registro, Filtro *meu_filtro){
    /* Função que inserirá os dados a serem atualizados de um filtro em um certo
    registro (registro <- filtro) */

    // IdCrime
    if(meu_filtro->idCrime != NULL){
        (*meu_registro)->idCrime = *(meu_filtro->idCrime);
    }
    // dataCrime
    if(meu_filtro->dataCrime != NULL){
        strcpy((*meu_registro)->dataCrime , meu_filtro->dataCrime);
    }
    // numeroArtigo
    if(meu_filtro->numeroArtigo != NULL){
        (*meu_registro)->numeroArtigo = *(meu_filtro->numeroArtigo);
    }
    // marcaCelular
    if(meu_filtro->marcaCelular != NULL){
        strcpy((*meu_registro)->marcaCelular , meu_filtro->marcaCelular);
    }
    // lugarCrime
    if(meu_filtro->lugarCrime != NULL){
        if(strlen((*meu_registro)->lugarCrime) < strlen(meu_filtro->lugarCrime)){
            (*meu_registro)->lugarCrime = realloc((*meu_registro)->lugarCrime, strlen(meu_filtro->lugarCrime) + 1);
        }
        strcpy((*meu_registro)->lugarCrime , meu_filtro->lugarCrime);
    }
    // descricaoCrime
    if(meu_filtro->descricaoCrime != NULL){
        if(strlen((*meu_registro)->descricaoCrime) < strlen(meu_filtro->descricaoCrime)){
            (*meu_registro)->descricaoCrime = realloc((*meu_registro)->descricaoCrime, strlen(meu_filtro->descricaoCrime) + 1);
        }
        strcpy((*meu_registro)->descricaoCrime , meu_filtro->descricaoCrime);
    }
}
void atualizacao_ind(FILE *arq_bin, Filtro *meu_filtro, Filtro *filtro_atualiza, Cabecalho *meu_cabecalho,Indice *meu_ind, char *val_buscado, bool flag_atualiza){
    /* Funcao utilizada para atualizar os arquivos quando a busca se da pelo indice */
    Registro *registro_novo, *registro_antigo;

    long long int pos_ini, pos_final, tamanho_lido = 17; // Quantidade de bytes lidos (começa em 17, logo após o cabeçalho)
    int num_ind = meu_ind->numIndices;
    int pos_ind, tamanho_antigo, tamanho_novo;
    
    pos_ind = achar_registro(meu_ind,meu_ind->tipoDado,val_buscado); // Acho uma ocorrencia do registro no indice
    
    if(pos_ind != -1){ // Se esta no indice
        fseek(arq_bin,meu_ind->dados[pos_ind].ByteOffset,SEEK_SET); // Vou para a posicao no binario
        tamanho_lido = meu_ind->dados[pos_ind].ByteOffset;  // Atualizo para contar os bytes saltados pelo fseek
        
        pos_ini = tamanho_lido; // Salvo o incio do registro
        registro_antigo = ler_registro_binario(arq_bin,&tamanho_lido); // Leio
        tamanho_antigo = tamanho_lido - pos_ini; // Salvo o tamanho dele

        do{
            if(adequa_filtro(registro_antigo,meu_filtro)){ // Se passou no filtro de busca

                // Crio um registro com a atualizacao e conto o tamanho dele
                registro_novo = atualizar_registro(registro_antigo, filtro_atualiza, &tamanho_novo); 

                if(tamanho_antigo >= tamanho_novo){ // Se cabe no registro anterior
                    // Volto para o início do registro e insiro a atualizacao, com lixo $ ocupando a diferenca de espaco
                    fseek(arq_bin, (-1 * tamanho_antigo), SEEK_CUR);
                    salvar_registro(arq_bin, registro_novo, NULL);
                    enchimento_registro(arq_bin, tamanho_antigo - tamanho_novo);
                    delimita_registro(arq_bin, NULL);

                    if(flag_atualiza == true) // Se tiver alterado o valor do indice, atualizo
                        // Sem mudar o offset(pos_final = pos_ini) e coma  posicao no indice ja dada
                        atualizar_indice(registro_novo, registro_antigo, &meu_ind, pos_ini, pos_ini, pos_ind);
                }
                else{
                    // Nao cabe no registro antigo
                    // Salvo o final do arquivo, pois sera o novo offset na atualizacao
                    pos_final = meu_cabecalho->proxByteOffset;
                    atualizar_indice(registro_novo, registro_antigo, &meu_ind, pos_ini, pos_final, pos_ind);
                    
                    // Vou para o final e insiro  registro atualizado
                    fseek(arq_bin, pos_final, SEEK_SET);
                    salvar_registro(arq_bin, registro_novo, meu_cabecalho);
                    delimita_registro(arq_bin, meu_cabecalho);
                    // Removo o registro anterior
                    remover_noArquivo(arq_bin, registro_antigo, pos_ini);
                    meu_cabecalho->nroRegRem++;
                }
                liberar_registro(registro_novo);
            }

            pos_ind++; // Vou para o proximo elemento do indice
            if(pos_ind >= num_ind)break; // Se passar do numero de indices, saio

            // Desaloco memoria anterior e leio o novo registro, salvando seu tamanho e offset
            liberar_registro(registro_antigo);
            fseek(arq_bin,meu_ind->dados[pos_ind].ByteOffset,SEEK_SET);
            tamanho_lido = meu_ind->dados[pos_ind].ByteOffset;
            pos_ini = tamanho_lido;
            registro_antigo = ler_registro_binario(arq_bin,&tamanho_lido);
            tamanho_antigo = tamanho_lido - pos_ini;

            // Se o registro ainda se encaixar no indice, continuo
        }while(adequa_indice(registro_antigo,meu_ind->campoIndexado,val_buscado));
        liberar_registro(registro_antigo); // Libero memoria
    }   
    free(val_buscado); // Libero memoria
}
void atualizacao_seq(FILE *arq_bin,Filtro *meu_filtro,Filtro *filtro_atualiza,Cabecalho *meu_cabecalho,Indice *meu_indice, bool flag_atualiza){
    /* Funcao utilizada para atualizar os arquivos quando a busca eh sequencial */
    Registro *registro_novo, *registro_antigo;
    
    long long int pos_ini, pos_final, tamanho_lido = 17; // Quantidade de bytes lidos (começa em 17, logo após o cabeçalho)
    int tamanho_antigo, tamanho_novo, pos_ind;
    int num_reg = meu_cabecalho->nroRegArq;

    for(int i = 0; i < num_reg; i++){ // Leio ate o ultimo registro do binario

        // Leio um registro, salvando seu offset e seu tamanho
        pos_ini = tamanho_lido;
        registro_antigo = ler_registro_binario(arq_bin,&tamanho_lido);
        tamanho_antigo = tamanho_lido - pos_ini;

        // Se ele se encaixar no filtro e nao estiver removido
        if(registro_antigo->removido == nao_removido && adequa_filtro(registro_antigo,meu_filtro)){ 
            // Crio um registro com a atualizacao e conto o tamanho dele
            registro_novo = atualizar_registro(registro_antigo, filtro_atualiza, &tamanho_novo);
            
            if(tamanho_antigo >= tamanho_novo){ // Se cabe no registro anterior
                // Volto para o início do registro e insiro a atualizacao, com lixo $ ocupando a diferenca de espaco
                fseek(arq_bin, (-1 * tamanho_antigo), SEEK_CUR);
                salvar_registro(arq_bin, registro_novo, NULL);
                enchimento_registro(arq_bin, tamanho_antigo - tamanho_novo);
                delimita_registro(arq_bin, NULL);
                
                if(flag_atualiza == true) // Se tiver alterado o valor do indice, atualizo
                    // Sem mudar o offset(pos_final = pos_ini) e busca a posicao la dentro
                    atualizar_indice(registro_novo, registro_antigo, &meu_indice, pos_ini, pos_ini, -1);
            }
            else{ // Nao cabe no registro antigo
                // Salvo o final do arquivo, pois sera o novo offset na atualizacao
                pos_final = meu_cabecalho->proxByteOffset;
                atualizar_indice(registro_novo, registro_antigo, &meu_indice, pos_ini, pos_final, -1);
                
                // Vou para o final e insiro  registro atualizado
                fseek(arq_bin, pos_final, SEEK_SET);
                salvar_registro(arq_bin, registro_novo, meu_cabecalho);
                delimita_registro(arq_bin, meu_cabecalho);
                // Removo o registro anterior
                remover_noArquivo(arq_bin, registro_antigo, pos_ini);
                meu_cabecalho->nroRegRem++;
            }
            liberar_registro(registro_novo);
        }
        liberar_registro(registro_antigo);
    }

    return;
}

// Para busca
void busca_indice(FILE *arq_bin, Filtro *meu_filtro, Indice *meu_indice, char *valor_buscado){
    /* Função que realiza a busca de registros no índice e os imprime caso se adequem ao filtro */
    
    Registro *meu_registro;
    
    int pos_indice, registros_impressos = 0;
    
    // Achar posição do vetor que se encontra o primeiro elemento com o 'valor_buscado'
    pos_indice = achar_registro(meu_indice,meu_indice->tipoDado,valor_buscado);
    
    // Se encontrou no índice
    if(pos_indice != -1){
        // Pula para o byteOffset e lê o registro
        fseek(arq_bin,meu_indice->dados[pos_indice].ByteOffset,SEEK_SET);
        meu_registro = ler_registro_binario(arq_bin, NULL);
        pos_indice++;
        
        do{
            if(adequa_filtro(meu_registro,meu_filtro)){
                // Caso o registro se adeque ao filtro, imprime
                imprimir_registro(meu_registro);
                registros_impressos++;
            }
            liberar_registro(meu_registro);
            
            // Pula para o próximo byteOffset e lê o registro
            fseek(arq_bin,meu_indice->dados[pos_indice++].ByteOffset,SEEK_SET);
            meu_registro = ler_registro_binario(arq_bin, NULL);

            // Loop ocorre até que o valor do campo indexado se torne diferente do buscado 
        }while(adequa_indice(meu_registro,meu_indice->campoIndexado,valor_buscado));
        liberar_registro(meu_registro);
    }

    if(registros_impressos == 0){
        printf("Registro inexistente.\n");
    }
    
    free(valor_buscado);
}
void busca_sequencial(FILE *arq_bin, Filtro *meu_filtro, Cabecalho *meu_cabecalho){
    /* Função que realiza a busca sequencial de registros e os imprime caso se adequem ao filtro */
    
    Registro *meu_registro;

    int registros_impressos = 0;
    for(int i = 0; i < meu_cabecalho->nroRegArq; i++){
        // Lê registro, se passar no filtro imprime, avança até todos os registros terem sido lidos
        meu_registro = ler_registro_binario(arq_bin, NULL);
        if (adequa_filtro(meu_registro,meu_filtro)){
            imprimir_registro(meu_registro);
            registros_impressos++;
        }
        liberar_registro(meu_registro);
    }

    if(registros_impressos == 0){
        printf("Registro inexistente.\n");
    }
}

// Para remoção
void remocao_indice(FILE *arq_bin, Filtro *meu_filtro, Cabecalho *meu_cabecalho, Indice *meu_indice, char *valor_buscado){
    /* Função que busca os registros pelo índice, caso se adequem ao filtro, remove tanto no arquivo de índice quanto de registros */
    
    Registro *meu_registro;
    
    int pos_indice;

    // Quantidade de bytes lidos (começa em 17, logo após o cabeçalho)
    long long int pos_inicial, tamanho_lido = 17;
    
    pos_indice = achar_registro(meu_indice,meu_indice->tipoDado,valor_buscado);
    // Caso encontre o registro primeiro registro com valor igual a 'valor_buscado'
    if(pos_indice != -1){
        
        // Lê registro na posição encontrada
        fseek(arq_bin,meu_indice->dados[pos_indice].ByteOffset,SEEK_SET);
        meu_registro = ler_registro_binario(arq_bin,&tamanho_lido);
        pos_inicial = meu_indice->dados[pos_indice].ByteOffset;

        do{
            // Caso se adeque ao filtro, remove em ambos os arquivos
            if(adequa_filtro(meu_registro,meu_filtro)){
                // Remover no arquivo
                if(remover_noArquivo(arq_bin,meu_registro,pos_inicial)){
                    meu_cabecalho->nroRegRem++;
                }
                // Remover no índice
                remover_noIndice(meu_indice,pos_indice);
            }else{
                // Indo para próxima posição no vetor de índices
                pos_indice++;
            }

            liberar_registro(meu_registro);
            fseek(arq_bin,meu_indice->dados[pos_indice].ByteOffset,SEEK_SET);
            
            pos_inicial = meu_indice->dados[pos_indice].ByteOffset;
            meu_registro = ler_registro_binario(arq_bin,&tamanho_lido);

        }while(adequa_indice(meu_registro,meu_indice->campoIndexado,valor_buscado));
        liberar_registro(meu_registro);
    }   
    free(valor_buscado);
}
void remocao_sequencial(FILE *arq_bin, Filtro *meu_filtro, Cabecalho *meu_cabecalho, Indice *meu_indice,char *valor_buscado){
    /* Função que realiza uma busca sequencial dos registros, caso se adequem ao filtro, remove em ambos os arquivos */

    /* Como não devemos utilizar ftell para determinar o byteOffset de um certo
    registro, a variável 'tamanho_lido' armazenará os bytes lidos, e servirá para
    esse propósito. Ela é incrementada toda vez que algo é lido, o tanto de bytes
    lidos */

    int int_buscado, pos_indice;
    long long int pos_inicial, tamanho_lido = 17;

    Registro *meu_registro;
    for(int i = 0; i < meu_cabecalho->nroRegArq; i++){
        pos_inicial = tamanho_lido; // Obtendo byteoffset do registro
        meu_registro = ler_registro_binario(arq_bin,&tamanho_lido);
        
        if(meu_registro->removido == nao_removido && adequa_filtro(meu_registro,meu_filtro)){
            
            // Removendo no arquivo
            if(remover_noArquivo(arq_bin,meu_registro,pos_inicial)){
                meu_cabecalho->nroRegRem++;
            }
            
            /* Removendo no índice */
            /* 
            Sequência de passos: 
            . Armazenar byteOffset do registro
            . Obter valor do registro no campo que está indexado
            . Achar a posição no vetor de índices onde se encontra a primeira ocorrência
            desse valor
            . Avançar no vetor de índices, começando na posição encontrada, até que o
            byteOffset seja igual ao byteOffset armazenado
            . Remover índice encontrado (se refere o registro que se deseja remover)
            */

            if(!strcmp(meu_indice->tipoDado,"inteiro")){
                int_buscado = selecionar_int(meu_registro, meu_indice->campoIndexado);
                valor_buscado = malloc(13);
                // Convertendo o int buscado para string, 'achar_registro' requer uma string
                sprintf(valor_buscado,"%d",int_buscado);
            }
            else{
                valor_buscado = selecionar_string(meu_registro,meu_indice->campoIndexado,13);
            }

            pos_indice = achar_registro(meu_indice,meu_indice->tipoDado,valor_buscado);
            if(pos_indice != -1){
                // Enquanto byteOffset for distinto do desejado, avançar no vetor
                while(pos_indice < meu_indice->numIndices && meu_indice->dados[pos_indice].ByteOffset != pos_inicial){
                    pos_indice++;
                }
            }
            remover_noIndice(meu_indice,pos_indice);
            free(valor_buscado);
        }
        liberar_registro(meu_registro);
    }
}


// Funções internas
Registro* ler_registro_teclado(FILE *arq_bin){
    /* Funcao para ler um registro inserido via input do teclado/terminal */

    char string_lida[tam_buffer];
    
    // Inicializando e alocando estrutura onde os dados lidos serÃ£o armazenados

    Registro *meu_registro;

    meu_registro = criar_registro();

    meu_registro->lugarCrime = malloc(tam_buffer); // Criar um ponteiro para os proximos reallocs
    meu_registro->descricaoCrime = malloc(tam_buffer); // Criar um ponteiro para os proximos reallocs
    
    /*Leitura*/
    // Primeiro a leitura do id
    // Verifico se eh NULO, caso nao, converto para inteiro e salvo
    scanf("%s", string_lida);
    if(!strcmp(string_lida, "NULO")){
        // Se for Nulo, preencho com -1
        meu_registro->idCrime = -1;
    }
    else{// Se nao, converto para int e salvo
        meu_registro->idCrime = atoi(string_lida);
    }

    // Agora a leitura da meu_registro, feito pela funcao para ler strings
    scan_quote_string(meu_registro->dataCrime);

    // Agora a leitura do numero do artigo
    // Verifico se eh NULO, caso nao, converto para inteiro e salvo
    scanf("%s", string_lida);
    if(!strcmp(string_lida, "NULO")){
        // Se for Nulo, preencho com -1
        meu_registro->numeroArtigo = -1;
    }
    else{// Se nao, converto para int e salvo
        meu_registro->numeroArtigo = atoi(string_lida);
    }

    // Agora a leitura do lugar, feito pela funcao para ler strings
    scan_quote_string(meu_registro->lugarCrime);
    // Agora a leitura da descricao, feito pela funcao para ler strings
    scan_quote_string(meu_registro->descricaoCrime);
    // Agora a leitura da marca, feito pela funcao para ler strings
    scan_quote_string(meu_registro->marcaCelular);

    return meu_registro;
}
Registro *atualizar_registro(Registro *registro_antigo, Filtro *filtro_atualiza, int *tamanho_novo){
    /* Funcao para criar um registro atualziado, com base em um registro antigo e em um filtro contendo as atualizações */

    Registro *registro_novo;
    registro_copia(&registro_novo, registro_antigo); // Copio os valores do registro antigo
    insere_atualizacao(&registro_novo, filtro_atualiza); // Insiro as atualizacoes que serao feitas
    (*tamanho_novo) = tamanho_registro(registro_novo);// Calculo o tamanho novo com tamanho_registro, pois nao ha lixo de memoria

    return registro_novo;
}
int tamanho_registro(Registro *meu_registro){
    /* Função utilizada para calcular o tamanho dos registros sem lixo (subsequentes '$') */

    int tamanho = 0;
    tamanho += 1; // Somo o byte de removido
    tamanho += 4; // Somo o byte de ID
    tamanho += 10; // Somo o byte da meu_registro
    tamanho += 4; // Somo o byte do Artigo
    tamanho += 12; // Somo o byte da marca celular
    tamanho += 2; // Somo o byte dos dois delimitadores de campo variavel
    tamanho += 1; // Somo o byte do delimitador de registro

    //Todo registro tem tamanho minimo de 34, apos contar o #
    // Agora para os campos variaveis

    // Leio ate achar um \0, contando quantos bytes percorri
    for(int i = 0; meu_registro->descricaoCrime[i] != '\0'; i++){
        tamanho++;
    }
    for(int i = 0; meu_registro->lugarCrime[i] != '\0'; i++){
        tamanho++;
    }

    return tamanho;
}
void preencher_campos(Registro *meu_registro,char *dados, int campo_atual){
    /*
     * Preenchendo cada campo da estrutura registro com os campos lidos do .csv
     * A escolha do campo a ser preenchido é controlada pela flag 'campo_atual'
    */
    switch(campo_atual){
        case ID_CRIME:
            meu_registro->idCrime = atoi(dados); // string -> int
            break;
        case DATA_CRIME:
            strcpy(meu_registro->dataCrime,dados);
            break;
        case NUMERO_ARTIGO:
            if((*dados) == '\0'){ // Caso numeroArtigo é nulo
                meu_registro->numeroArtigo = -1;
            }else{
                meu_registro->numeroArtigo = atoi(dados);
            }
            break;
        case 4:
            // Campo variável, necessita ser alocado dinamicamente
            meu_registro->lugarCrime = malloc((strlen(dados)+1)*sizeof(char));
            strcpy(meu_registro->lugarCrime,dados);
            break;
        case DESCRICAO_CRIME:
            meu_registro->descricaoCrime = malloc((strlen(dados)+1)*sizeof(char));
            strcpy(meu_registro->descricaoCrime,dados);
            break;
        case MARCA_CELULAR:
            strcpy(meu_registro->marcaCelular,dados);
            break;
    }
}
void registro_copia(Registro **destino, Registro *fonte){
    /* Copia o conteudo de um registro para outro */
    
    // Inicializando e alocando estrutura onde os dados lidos serão armazenados
    (*destino) = malloc(sizeof(Registro));
    (*destino)->lugarCrime = NULL;
    (*destino)->descricaoCrime = NULL;
    
    // Copio campo a campo
    // Inteiros com atribuicoes
    // Strings fixas com strcpy
    // Strings variaveis com mallocs e strcpys
    
    (*destino)->removido = fonte->removido;

    (*destino)->idCrime = fonte->idCrime;

    strcpy((*destino)->dataCrime, fonte->dataCrime);

    (*destino)->numeroArtigo = fonte->numeroArtigo;

    strcpy((*destino)->marcaCelular, fonte->marcaCelular);

    (*destino)->lugarCrime = malloc(strlen(fonte->lugarCrime) + 1); // Garantir espaco para o /0
    strcpy((*destino)->lugarCrime, fonte->lugarCrime);

    (*destino)->descricaoCrime = malloc(strlen(fonte->descricaoCrime) + 1); // Garantir espaco para o /0
    strcpy((*destino)->descricaoCrime, fonte->descricaoCrime);

    return;
}
void enchimento_registro(FILE *arq_bin, int qtd_enchimento){
    /* Caso o espalo ocupado por um registro novo ser inferior ao registro antigo,
    o espaço sobrando deve ser preenchido por lixo ('$') */
    for(int i = 0; i < qtd_enchimento; i++)
    {
        fwrite(enchimento, sizeof(char), 1, arq_bin);
    }
}
void escreve_tamFixo(FILE *arq_bin, char *string, int tam){
    /* Funcao para escrever campos strings de tamanho fixo no binario*/
    
    switch(tam){
        // Se for campo dataCrime
        case tam_dataCrime:
            if(string[0] == '\0'){ // Se o primeiro char for '\0' significa que é nulo
                for(int i = 0; i < tam_dataCrime; i++){ // preenche com '$'
                    fwrite(enchimento, sizeof(char), 1, arq_bin);
                }
            }
            else{
                for(int i = 0; i < tam_dataCrime; i++){
                    fwrite(&(string[i]), sizeof(char), 1, arq_bin);
                }
            }
            break;
        
        
        // Se for campo marcaCelular
        case tam_marcaCelular:
            if(string[0] == '\0'){
                for(int i = 0; i < tam_marcaCelular; i++){ 
                    fwrite(enchimento, sizeof(char), 1, arq_bin);
                }
            }
            else{
                int i = 0;
                while(string[i] != '\0' && string[i] != '$'){ // Escreve até o final da string ou até encontrar '$'
                    fwrite(&(string[i]), sizeof(char), 1, arq_bin);
                    i++;
                }
                while(i < tam_marcaCelular){ // Preencher o que faltar com '$'
                    fwrite(enchimento, sizeof(char), 1, arq_bin);
                    i++;
                }
            }
            break;
    }
}
void escreve_tamVariavel(FILE *arq_bin, char *string){
    /* Funcao para escrever campos strings de tamanho variavel no binario */

    for(int i = 0; string[i] != '\0'; i++){ // Insiro ate o final da string
        fwrite(&(string[i]), sizeof(char), 1, arq_bin);
    }
    fwrite(delimitador_campo, sizeof(char), 1, arq_bin); // Delimito o fim do campo variavel
}