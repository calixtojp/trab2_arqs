#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao.h"
#include "auxiliares.h"
#include "arq_dados.h"
#include "arq_indice.h"

#define MAX_NOME_ARQ 50
#define TAM_MAX_NOME 50
#define TAM_MAX_VALOR 50

struct ArqDados{
    char nomeArqDados[MAX_NOME_ARQ];//guarda o nome do arquivo de dados
    FILE *arqDados;
    cabecalho_t *cabecalhoDados;
};

struct ArqIndex{
    char campoIndexado[MAX_NOME_ARQ];//guarda o nome do campo indexado (idCrime, dataCrime, etc...)
    char tipoDado[MAX_NOME_ARQ];//guarda o tipo de dado indexado em formato de string
    char nomeArqIndex[MAX_NOME_ARQ];//guarda o nome do arquivo de index
    int tipoDadoInt;//guarda o tipo de dado indexado em formato de string
    FILE *arqIndex;
    cabecalho_indx_t *cabecalhoIndex;

    //Esses dois vetores armazenam o arquivo de index. Se o campo indexado for uma string, então o vetor para
    //campo indexado string será alocado. Caso contrário, um vetor para campo indexado int será alocado
    dados_indx_str_t **vet_indx_str;
    dados_indx_int_t **vet_indx_int;
    
    void *vetTemp;
    int qtdReg_vetTemp;
    int nro_addVetTemp;
    /*Os três últimos campos são usados para fazer alterações no vetor de dados_indx (int ou str)
    Essa lógica foi adotada pois não é interessante mudar as informações do arquivo de índice 
    enquanto ele está sendo processado, dado que isso pode gerar diversos erros. Assim, depois de
    fazer todas as alterações, esses campos são copiados de volta para os originais*/
};


//Struct utilizada para guardar informações de busca (ou alteração, na funcionalidade 7)
struct InfoBusca{
    int qtd_crit;
    char **nomes;
    char **vals_str;
    int *vals_int;
};

ArqDados_t *alocar_arq_dados(void){
    //Aloca o cabeçalho e o tipo ArqDados_t

    ArqDados_t *arq_dados_main;
    arq_dados_main = malloc(sizeof(ArqDados_t));

    arq_dados_main->cabecalhoDados = alocar_cabecalho_dados();

    return arq_dados_main;
}

ArqIndex_t *alocar_arq_index(void){
    //Aloca o cabeçalho e o tipo ArqIndex_t

    ArqIndex_t *arq_index_main;
    arq_index_main = malloc(sizeof(ArqIndex_t));

    arq_index_main->cabecalhoIndex = alocar_cbl_indx();
    arq_index_main->vet_indx_int = NULL;
    arq_index_main->vet_indx_str = NULL;
    return arq_index_main;
}

void *escolhe_vet_indx(ArqIndex_t *arq_index){
    //Escolhe o tipo de vetor de dados que está sendo utilizado, utilizando o modificador 'arq_index->tipoDadoInt'
    //e retorna esse mesmo vetor em formato de void* (para ser um tipo genérico).
    switch(arq_index->tipoDadoInt){
        case 0:
            return arq_index->vet_indx_int;
        case 1:
            return arq_index->vet_indx_str;
    }
}

void *escolhe_indx_dado(ArqIndex_t *arq_index){
    //Aloca uma struct dado_indx_int_t ou dado_indx_str_t
    switch(arq_index->tipoDadoInt){
        case 0:
            return alocDadoIndxInt();
        case 1:
            return alocDadoIndxStr();
    }
}

void *escolhe_criterio_vet_vals(InfoBusca_t *criterios, int pos, int tipoDado){
    switch(tipoDado){
        case 0:
            return &(criterios->vals_int[pos]);
        case 1:
            return criterios->vals_str[pos];
        default:
            break;
    }
}

void ler_nome_arq_dados(ArqDados_t *arq_dados){
    int retorno_scanf = scanf(" %s", arq_dados->nomeArqDados); 
    if(retorno_scanf != 1){
        mensagem_erro();
    }
}

void ler_nome_arq_index(ArqIndex_t *arq_index){
    int retorno_scanf = scanf(" %s", arq_index->nomeArqIndex); 
    if(retorno_scanf != 1){
        mensagem_erro();
    }
}

void ler_campoIndexado(ArqIndex_t *arq_index){
    int retorno_scanf = scanf(" %s", arq_index->campoIndexado);
    if(retorno_scanf != 1){
        mensagem_erro();
    }
}

void ler_tipoDado(ArqIndex_t *arq_index){
    int retorno_scanf;
    retorno_scanf = scanf(" %s", arq_index->tipoDado);
    if(retorno_scanf != 1){
        mensagem_erro();
    }

    if(strcmp(arq_index->tipoDado, "inteiro")==0){
        arq_index->tipoDadoInt = 0;
    }else if(strcmp(arq_index->tipoDado, "string")==0){
        arq_index->tipoDadoInt = 1;
    }
}

void ler_cabecalho_arq_index(ArqIndex_t *arq_index){
    //funcao que carrega na memoria primaria o registro de cabecalho de um arquivo de indice
    ler_index_cabecalho(arq_index->arqIndex, arq_index->cabecalhoIndex);
}

void ler_dados_arq_index(ArqIndex_t *arq_index){
    //funcao que carrega na memoria primaria os registros de dados de um arquivo de indice
    if(comparar_strings(arq_index->tipoDado,"inteiro")==0){
        //se o campo for inteiro, preencho o vetor de dados inteiros
        arq_index->vet_indx_int = ler_index_dado_int(arq_index->arqIndex,arq_index->cabecalhoIndex);
    }else{
        //se o campo nao for inteiro, entao ele eh string. Entao preencho o vetor de dados string
        arq_index->vet_indx_str = ler_index_dado_str(arq_index->arqIndex,arq_index->cabecalhoIndex);
    }
}

void abrir_arq_dados(ArqDados_t *arq_dados, const char *tipo_leitura){
    arq_dados->arqDados = fopen(arq_dados->nomeArqDados, tipo_leitura);
    if(arq_dados->arqDados == NULL){
        mensagem_erro();
    }
}

void abrir_arq_index(ArqIndex_t *arq_index, const char *tipo_leitura){
    arq_index->arqIndex = fopen(arq_index->nomeArqIndex, tipo_leitura);
    if(arq_index->arqIndex == NULL){
        mensagem_erro();
    }
} 

void desalocar_ArqDados(ArqDados_t *arq_dados){
    free(arq_dados->cabecalhoDados);
    free(arq_dados);
}

void desalocar_ArqIndex(ArqIndex_t *arq_index){
    int qtd_reg = get_qtdReg(arq_index->cabecalhoIndex);

    if(strcmp(arq_index->tipoDado, "inteiro")==0){
        desalocarVetIndxDadoInt(arq_index->vet_indx_int, qtd_reg);
    }else if(strcmp(arq_index->tipoDado, "string")==0){
        desalocarVetIndxDadoStr(arq_index->vet_indx_str, qtd_reg);
    }
    free(arq_index->cabecalhoIndex);
    free(arq_index);
}

void fechar_arq_dados(ArqDados_t *arq_dados){
    fclose(arq_dados->arqDados);
}

void fechar_arq_index(ArqIndex_t *arq_index){
    fclose(arq_index->arqIndex);
}

void ler_cabecalho_dados(ArqDados_t *arq_dados){
    arq_dados->cabecalhoDados = ler_dados_cabecalho(arq_dados->arqDados);
}

int ler_cabecalho_index(ArqIndex_t *arq_index){
    arq_index->cabecalhoIndex = get_cabecalho_indx(arq_index->arqIndex);
    if(arq_index->cabecalhoIndex == NULL){
        //Se não consegui ler o cabeçalho, retorno 0
        return 0;
    }else{
        return 1;
    }
}

int getTamCabecalhoDados(ArqDados_t *arq_dados){
    return len_cabecalho_dados();
}

int get_nroRegTotal(ArqDados_t *arq_dados){
    return get_nroRegArq(arq_dados->cabecalhoDados);
}

int get_nroRegValidos(ArqDados_t *arq_dados){
    //Retorna o número de registros válidos:
    //     nroRegArq - nroRegRem
    int resultado = 0;
    resultado += get_nroRegArq(arq_dados->cabecalhoDados);
    resultado -= get_nroRegRem(arq_dados->cabecalhoDados);
    return resultado;
}

char *getNomeArqIndex(ArqIndex_t *arq_index){
    return arq_index->nomeArqIndex;
}

char *getNomeArqDados(ArqDados_t *arq_dados){
    return arq_dados->nomeArqDados;
}

void levaFinalCursorDados(ArqDados_t *arq_dados){
    fseek(arq_dados->arqDados, 0, SEEK_END);
}

void alocar_vet_index(ArqIndex_t *arq_index, unsigned int nroRegValidos){
    /*
            Aloca, na memória primária, um vetor que guardará os registros do arquivo de index.
            O vetor ficará armazenado no vet_indx_str ou vet_indx_int, a depender o tipo de campo armazenado utilizado.
            Essa função leva em consideração o número de registros válidos (que serão utilizados para indexação).
    */

    int tam;
    if(strcmp(arq_index->tipoDado, "inteiro") == 0){
        arq_index->vet_indx_int = aloc_vet_indx_DadoInt(nroRegValidos);
        arq_index->tipoDadoInt = 0;
    }else if(strcmp(arq_index->tipoDado, "string") == 0){
        arq_index->vet_indx_str = aloc_vet_indx_DadoStr(nroRegValidos);
        arq_index->tipoDadoInt = 1;
    }
}

void realocar_vet_index(ArqIndex_t *arq_index, int original, int acrescimo){
    //Realoca o vetor carregado no arq_index->vet_indx_int ou arq_index->vet_indx_str 
    int tam_tot = original + acrescimo;

    if(strcmp(arq_index->tipoDado, "inteiro")==0){
        arq_index->vet_indx_int = realloc(arq_index->vet_indx_int,(sizeof(dados_indx_int_t*))*(tam_tot));

        for(int i = original; i < tam_tot; ++i){
            (arq_index->vet_indx_int)[i] = alocDadoIndxInt();
        }        

    }else if(strcmp(arq_index->tipoDado, "string")==0){
        arq_index->vet_indx_str = realloc(arq_index->vet_indx_str,(sizeof(dados_indx_str_t*))*(tam_tot));

        for(int i = original; i < tam_tot; ++i){
            (arq_index->vet_indx_str)[i] = alocDadoIndxStr();
        }

    }
}

void realocar_vetIndxTemp(ArqIndex_t *arq_index, int original, int acrescimo){
    //realoca o vetor temporário armazenado em arq_index->vetTemp
    int tam_tot = original + acrescimo;
    if(strcmp(arq_index->tipoDado, "inteiro")==0){
        arq_index->vetTemp = realloc(arq_index->vetTemp,(sizeof(dados_indx_int_t*))*(tam_tot));

        dados_indx_int_t **vet_real = (dados_indx_int_t**)arq_index->vetTemp;
        for(int i = original; i < tam_tot; ++i){
            vet_real[i] = alocDadoIndxInt();
        }

    }else if(strcmp(arq_index->tipoDado, "string")==0){
        arq_index->vetTemp = realloc(arq_index->vetTemp,(sizeof(dados_indx_str_t*))*(tam_tot));

        dados_indx_str_t **vet_real = (dados_indx_str_t**)arq_index->vetTemp;
        for(int i = original; i < tam_tot; ++i){
            vet_real[i] = alocDadoIndxStr();
        }
    }
}

int indexaRegistro(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int posReg, long int *byteOffSetAnt){
    /*
        Função que executa a leitura de registros do arquivo de dados e tenta indexar esses mesmos
    registros no vetor armazenado em arq_index->vet_indx_int ou em arq_index->vet_indx_str.
        Entrada:
            -arq_dados: possui arquivo de dados no qual está sendo lido os registros
            -arq_index: possui o vetor no qual está sendo inserido os registros lidos
            -posReg: indica em qual posição (no vetor armazenado em arq_index) que será armazenado o o registro lido
            -byteOffSetAnt: um contador de byteOffSet que é alterado por referência
        Saida:
            -retorna 1 se conseguiu indexar o registro lido
            -retorna 0 se não conseguiu indexado o registro (em caso de campo indexado nulo ou registro removido logicamente).
    */

    dados_t *atual_reg = alocar_dados();

    //-flag que indica se consegui ler um registro válido
    int liRegValido=0;
    //-flag que indica se consegui ler um registro
    int consegui_ler;

    //tipo de dado que será usado (int ou str)
    int tipDado = arq_index->tipoDadoInt;
 
    //consegui_ler armazena -1 caso consiga ler e armazena o tamanho do registro caso consiga
    consegui_ler = ler_bin_registro(atual_reg, arq_dados->arqDados);
    while(consegui_ler>0){
        //Se consegui ler, devo conferir se esse é um registro
        //indexável - isto é, não removido e com indexado campo válido (não nulo). Para isso:
        int eh_campo_nulo;

        //1-vejo se o registro é removido logicamente
        int eh_removido = get_registro_removido(atual_reg); 
        //2-declaro as funções que serão utilizadas.
        FncCampoNulo fncsCampoNulo[] = {campoNulo_int,campoNulo_str};
        FncGetCampoIndexIndexado fncsGetCampo[] = {getCampoInt,getCampoStr};

        //3-aloco espaço para o campo indexado
        void *campoIndexado = malloc(sizeof(void));

        //4-obtenho o campo indexado (int ou str)
        campoIndexado = fncsGetCampo[tipDado](atual_reg, arq_index->campoIndexado);

        //5-vejo se o campo é nulo ou não
        eh_campo_nulo = fncsCampoNulo[tipDado](campoIndexado);

        if(eh_removido || eh_campo_nulo){
            //Se não é um registro indexável, tento ler o próximo
            *byteOffSetAnt = (*byteOffSetAnt) + consegui_ler;
            consegui_ler = ler_bin_registro(atual_reg, arq_dados->arqDados);
        }else{
            //Se consegui ler, devo indexá-lo
            liRegValido=1;

            //Crio o dado indexado
            void *dadoIndexado;
            void *vetIndx;

            FncSetDadoIndx fncsSetDadoIndx[] = {setDadoIndxInt,setDadoIndxStr};
            FncSetVetIndx fncsSetVetIndx[] = {setVetIndx_int,setVetIndx_str};

            //escolho entre dado e o vetor para int ou str
            dadoIndexado = escolhe_indx_dado(arq_index);
            vetIndx = escolhe_vet_indx(arq_index);

            //escrevo as informações (campo indexado e byteOffSet)
            fncsSetDadoIndx[tipDado](dadoIndexado, *byteOffSetAnt, campoIndexado); 

            //agora escrevo o dado no vetor em RAM
            fncsSetVetIndx[tipDado](vetIndx, posReg, dadoIndexado);

            //agora incremento o contador de byteOffSet
            *byteOffSetAnt += consegui_ler;
            //Como a escrita ocorreu bem, retorno 1
            return 1;
        }
    }

    if(liRegValido == 0){//significa que não li nenhum registro indexável
        return 0;
    }
}

void ordenaVetIndex(ArqIndex_t *arq_index, int qntd_reg){
    //Ordena o vetor de index carregado em RAM 
    FncOrdemTipo fncs_ordena_vet_indx[] = {ordenaVetIndex_int,ordenaVetIndex_str};

    void *vet_indx = malloc(sizeof(void));
    vet_indx = escolhe_vet_indx(arq_index);

    fncs_ordena_vet_indx[arq_index->tipoDadoInt](vet_indx, qntd_reg);
}

void escreveArqIndex(ArqIndex_t *arq_index){

    escreveCabecalhoIndex(arq_index->arqIndex, arq_index->cabecalhoIndex);
    
    escreveVetIndex(arq_index, 0, get_qtdReg(arq_index->cabecalhoIndex)-1);
}

void escreveVetIndex(ArqIndex_t *arq_index, int inicio, int fim){
    void *vet_indx = escolhe_vet_indx(arq_index);

    typedef void (*FncEscreveIndx) (FILE*, void*, int);
    FncEscreveIndx fncsEscreveIndx[] = {escreveVetIndx_int,escreveVetIndx_str};

    int tipo_dado = arq_index->tipoDadoInt;//0 para int, 1 para string

    //escrevo efetivamente o dado
    for(int cont = inicio; cont <= fim; ++cont){
        fncsEscreveIndx[tipo_dado](arq_index->arqIndex, vet_indx, cont);    
    }
}

void alterarQtdRegIndex(ArqIndex_t *arq_index, int qtdRegIndex){
    set_qtdReg(arq_index->cabecalhoIndex, qtdRegIndex);
}

int existe_index(InfoBusca_t *criterios, ArqIndex_t *arq_index){
    /*Função que, se o vetor de nomes (lido da entrada da funcionalidade [4]) 
    contiver o nome do campo indexado no arquivo de index, retorna o índice dessa string no vetor. 
    Caso contrário, retorna -1.*/

    /*Essa função determina se deve ser realizada 
    busca binária no arquivo de index ou sequencial no de dados.*/

    for(int i=0; i<criterios->qtd_crit; i++){
        if(strcmp(arq_index->campoIndexado,criterios->nomes[i])==0){
            return i;
        }
    }
    return -1;
}

InfoBusca_t *alocar_InfoBusca(int qtd_crit){
    InfoBusca_t *informacao = malloc(sizeof(InfoBusca_t));
    informacao->nomes = alocar_vetor_string(qtd_crit,TAM_MAX_NOME);
    informacao->vals_str = alocar_vetor_string(qtd_crit, TAM_MAX_VALOR);
    informacao->vals_int = malloc(sizeof(int)*qtd_crit);
    informacao->qtd_crit = qtd_crit;

    return informacao;
}

InfoBusca_t *ler_criterios_busca(){
    int m;
    scanf("%d",&m);

    InfoBusca_t *criterios = alocar_InfoBusca(m);

    for(int j=0; j<m; j++){
        scanf("%s",criterios->nomes[j]);
        if(ehInteiro(criterios->nomes[j])){
            //se o campo for um int
            scanf("%d",&criterios->vals_int[j]); //leio o valor

            //indico no vetor de valores string que o valor deve ser procurado no vetor de int
            strcpy(criterios->vals_str[j],"int");
        }else{
            //se nao for int, é string
            ler_aspas_string(criterios->vals_str[j]); //leio o valor

            //indico no vetor de valores int que o valor deve ser procurado no vetor de strings
            criterios->vals_int[j] = -1;
        }
    }

    //ler o buffer até a próxima linha
    ignorar_resto_linha();

    return criterios;
}

void desalocar_InfoBusca(InfoBusca_t *informacao){
    desalocar_vetor_string(informacao->nomes,informacao->qtd_crit);
    desalocar_vetor_string(informacao->vals_str,informacao->qtd_crit);
    free(informacao->vals_int);
    free(informacao);
}

void achouReg(void *arq_index, int flag){
    if(flag == 0){
        printf("Registro inexistente.\n");
    }
}

int testarStatusDados(ArqDados_t *arq_dados){
	//funcao que retorna 1 caso o arquivo esteja consistente e 0 caso esteja inconsistente
	if(getStatusDados(arq_dados->cabecalhoDados) == '1'){
		return 1;
	}
	return 0;
}

int testarStatusIndex(ArqIndex_t *arq_index){
	//funcao que retorna 1 caso o arquivo esteja consistente e 0 caso esteja inconsistente
	if(getStatusIndex(arq_index->cabecalhoIndex) == '1'){
		return 1;
	}
	return 0;
}

void printa_busca(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *registro, InfoBusca_t *alteracoes,long int byteOffSet){
    //funcao que ignora os campos do tipo void para que toda as funções de 
    //'acao' em 'processaRegistros()' sejam do mesmo tipo, no caso o 'FncAcao'

    //Em seguida, printa os campos de um registro
    mostrar_campos(registro);
}

void criarVetTemp(ArqIndex_t *arq_index, int qtdRegOrigem){
    //Funcao que aloca um vetor temporário, o qual recebe as modificações da funcionalidades 
    //Esse vetor é uma cópia do vetor de dados do arquivo de índice

    //descubro qual é o vetor de dados do arquivo de índice original
    void *vetOrigem = escolhe_vet_indx(arq_index);

    //copio a quantidade de registros indexados
    arq_index->qtdReg_vetTemp = qtdRegOrigem;

    //seto o número de registros adicionados no vetor temporário para zero
    arq_index->nro_addVetTemp = 0;

    //Faço a cópia do vetor dependendo do seu tipo
    if(strcmp(arq_index->tipoDado, "inteiro")==0){
        arq_index->vetTemp = aloc_vet_indx_DadoInt(qtdRegOrigem);
        copiaVetInt(arq_index->vetTemp,vetOrigem, 0, qtdRegOrigem-1, 0, qtdRegOrigem-1);
    }else if(strcmp(arq_index->tipoDado, "string")==0){
        arq_index->vetTemp = aloc_vet_indx_DadoStr(qtdRegOrigem);
        copiaVetStr(arq_index->vetTemp,vetOrigem, 0, qtdRegOrigem-1, 0, qtdRegOrigem-1);
    }
}

void apagarVetTemp(ArqIndex_t *arq_index){
    if(strcmp(arq_index->tipoDado, "inteiro")==0){
        desalocarVetIndxDadoInt(arq_index->vetTemp, arq_index->qtdReg_vetTemp);
    }else if(strcmp(arq_index->tipoDado, "string")==0){
        desalocarVetIndxDadoStr(arq_index->vetTemp, arq_index->qtdReg_vetTemp);
    }
}

void processaRegistros(ArqDados_t *arq_dados, ArqIndex_t *arq_index, InfoBusca_t *criterios, InfoBusca_t *alteracoes, 
                        FncAcao acao, FncFinaliza final){
    /*Funcao que define se a busca sera binaria no arquivo de indice ou sequencial no arquivo de dados e encontra os registros. 
    Depois, usa a FncAcao acao, e a FncFinaliza final para processá-los.*/

    //Alocar um vetor temporário, o qual irei editar ao longo do processo
    criarVetTemp(arq_index, get_qtdReg(arq_index->cabecalhoIndex));

    int existe = existe_index(criterios,arq_index);
    /*Se existe arquivo de indice para um dos campos a serem buscados,
    a variável 'existe' recebe o indice do nome desse campo no vet_nomes.
    Se não existe, a variável recebe -1.*/

    if(existe >= 0 ){
        /*se existe arquivo de index para um dos campos que se deseja 
        buscar, faz-se busca binária no arquivo de indice*/
        busca_bin_index(arq_index,arq_dados,existe,criterios,alteracoes,acao,final);
    }else{
        //se não, faz-se busca sequencial no arquivo de dados
    
        //Para isso, deve-se reiniciar o ponteiro do arquivo de dados para o primeiro registro de dados (pulando o cabecalho) 
        //para fazer um novo processamento, pois não há garantia de que o ponteiro esteja corretamente posicionado
        fseek(arq_dados->arqDados,len_cabecalho_dados(),SEEK_SET);
        //em seguida, chama-se a função que realiza a busca sequencial
        busca_seq_dados(arq_dados, arq_index, criterios,alteracoes,acao,final);
    }

    //desalocar o vetor temporário
    apagarVetTemp(arq_index);
}

void busca_bin_index(ArqIndex_t *arq_index, ArqDados_t *arq_dados, int pos_chave, InfoBusca_t *criterios, InfoBusca_t *alteracoes,FncAcao acao, FncFinaliza final){

    //descubro o tipo de dado indexado (0 inteiro, 1 string)
    int tipoDado = arq_index->tipoDadoInt;

    //crio um vetor de funções que fazem busca binária
    FncBuscaBin fncsBuscaBin[] = {busca_bin_int, busca_bin_str};

    void *vetorIndex = escolhe_vet_indx(arq_index);//descubro qual é o vetor no qual devo buscar
    void *chave = escolhe_criterio_vet_vals(criterios, pos_chave, tipoDado);//descubro qual é a chave busca

    int qtd_reg_val=0;//guarda o numero de registros que satisfazem o criterio de busca do arquivo de indice
    int pos_prim = fncsBuscaBin[tipoDado](vetorIndex, arq_index->cabecalhoIndex, chave, &qtd_reg_val);
    //fncsBuscaBin retorna o primeiro registro que satisfaz o critério de busca binária no vetorIndex.
    //Caso nenhum satisfaça, retorna -1. Além disso, motifica a variável "qtd_reg_val", por
    //referência, para o número de resgistros que satisfazem o critério de busca.
    //Dessa forma, o intervalo por_prim até [pos_prim+qtd_reg_val-1] é constituído por
    //todos os registros que satisfazem o critério de busca.

    //Com as informações sobre o intervalo (no vetIndex) que satisfaz os critérios de busca, percorro o vetIndex.
    //crio um vetor de funções que pegam o byteOffset, para usar no 'percorrer_index()'
    FncGetByteOffSet fncsGetByteOffSet[] = {get_byteOffset_int, get_byteOffset_str};
    percorrer_index(fncsGetByteOffSet[tipoDado], pos_prim, qtd_reg_val, arq_dados, arq_index, criterios, alteracoes,acao, final);
}

void percorrer_index(FncGetByteOffSet get_byteOffset, int pos_prim, int qtd_reg_val, 
                    ArqDados_t *arq_dados, ArqIndex_t *arq_index,InfoBusca_t *criterios, InfoBusca_t *alteracoes,FncAcao acao, FncFinaliza final){
    
    int achei_reg_val = 0;
    //Flag que indica se algum registro satisafaz todos os critérios de busca.
    //Recebe -1 se nenhum satisfaz ou 1 se pelo menos 1 satisfaz

    if(pos_prim != -1){
        /*Como existe pelo menos 1 registro que satisfaz a busca, percorro o vet_indx_int para todos os 
        registros que satisfazem o criterio de busca do campo indexado e testo os outros criterios de busca*/

        for(int i=0; i<qtd_reg_val; i++){
            //com a pos do primeiro e a qtd de registros, eu pego o byteoffset de todos eles

            void *vetor_real = escolhe_vet_indx(arq_index);
            long int byteOffset = get_byteOffset(vetor_real,pos_prim+i);
            
            //pra cada byteoffset, eu checo todos os criterios
            dados_t *reg = alocar_dados();
            getRegistro(byteOffset, arq_dados->arqDados, reg);
            
            int satisfaz = testar_criterios(reg, criterios->nomes, criterios->vals_str, criterios->vals_int, criterios->qtd_crit);

            if(satisfaz){
                //se o reg satisfaz todos os criterios, realizo a ação
                acao(arq_dados, arq_index, reg, alteracoes,byteOffset);
                achei_reg_val = 1;
            }

            desalocar_registro(reg);
        }
    }

    if(achei_reg_val != 0){
        achei_reg_val = arq_index->qtdReg_vetTemp;
    }
    final(arq_index, achei_reg_val);
}

void busca_seq_dados(ArqDados_t *arq_dados, ArqIndex_t *arq_index,InfoBusca_t *criterios, InfoBusca_t *alteracoes,FncAcao acao, FncFinaliza final){
    
    int achei_reg_val = 0;
    //Flag que indica se algum registro satisafaz todos os critérios de busca.
    //Recebe -1 se nenhum satisfaz ou 1 se pelo menos 1 satisfaz

    dados_t *registro = alocar_dados();//aloco um memoria para leitura de um registro de dados
        
    long int byteOffSet_atual = len_cabecalho_dados();
    //Consegui_ler armazena -1 se não conseguiu ler e armazena o tamanho do registro caso consiga ler
    int consegui_ler = ler_bin_registro(registro, arq_dados->arqDados);

    int cont = 0;
    while(consegui_ler>0){
        //Se consegui ler, avalio os criterios de busca

        if(testar_criterios(registro,criterios->nomes,criterios->vals_str,criterios->vals_int,criterios->qtd_crit)){
            ++cont;
            //se o registro satisfaz todos os criterios, realizo a ação 
            achei_reg_val = 1;//achei pelo menos 1 registro que satisfaz os critérios

            acao(arq_dados, arq_index, registro, alteracoes, byteOffSet_atual);
        }
        byteOffSet_atual += consegui_ler;
        //sempre desaloco o registro, pois preciso desalocar os campos de tamanho variavel do registro
        desalocar_registro(registro);
        //Em seguida aloco outro registro, para fazer a leitura
        registro = alocar_dados();
        consegui_ler = ler_bin_registro(registro, arq_dados->arqDados);
    }
    //se não conseguiu ler, os campos de tamanho variavel nao foram alocados. Assim, desaloco apenas o ponteiro para o tipo dados_t

    free(registro);
    
    if(achei_reg_val != 0){
        achei_reg_val = arq_index->qtdReg_vetTemp;
    }
    final(arq_index, achei_reg_val);
}

int get_nroRegIndex(ArqIndex_t *arq_index){
    return get_qtdReg(arq_index->cabecalhoIndex);
}

void ordenaVetIndexFinal(void *arq_index, int qtdReg_ordenar){
    //Função que copia o vetor temporário para o original e depois ordeno o original
    if(qtdReg_ordenar > 0){
        //Para isso:
        //1-faço o casting do arq_index
        ArqIndex_t *arq_index_real = (ArqIndex_t*)arq_index;

        //2-realoco o vetor original para seu novo tamanho
        int qtdReg_antes = get_qtdReg(arq_index_real->cabecalhoIndex);
        int qtdReg_depois = arq_index_real->qtdReg_vetTemp;
        int acrescimo = qtdReg_depois - qtdReg_antes;
        realocar_vet_index(arq_index_real, qtdReg_antes, acrescimo);

        //3-modifico a qtdReg original (que está armazenada no cabeçalho do arquivo de index carregado em memória primária)
        set_qtdReg(arq_index_real->cabecalhoIndex, qtdReg_depois);

        //4-copio o vetor temporário para o original
        void *vetorDestino = escolhe_vet_indx(arq_index);
        FncCopiaVet fncsCopiaVet[] = {copiaVetInt, copiaVetStr};
        fncsCopiaVet[arq_index_real->tipoDadoInt](vetorDestino, arq_index_real->vetTemp, 0,qtdReg_depois-1,0,qtdReg_depois-1);

        //5-Agora, ordeno o novo
        ordenaVetIndex(arq_index_real, qtdReg_depois);
    }
}

int inserirRegStdin(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int qtdInserir){
    /*
        Função que lê um registro da entrada padrão (por meio do scanf) e adiciona-o
    no arquivo de dados e de index.

        Entradas: 
        -ArqDados_t na qual será inserido o registro no final
    do arquivo de dados.
        -ArqIndex_t no qual será inserido o registro lido (no vetor temporário carregado dentro da própria struct)
        -int qtdInserir é a quantidade de registros que serão inseridos

        Comportamento:
        -A função altera o vetor temporário armazenado na struct ArqIndex_t, bem como a quantidade
        de registros armazenados no vetor temporário (arq_index->qtdReg_vetTemp). Após utilizado o vetor temporário,
        altera a quantidade de registros no cabeçalho do arquivo de index (carregado em RAM) e adiciona o vetor 
        temporário ao vetor Original de registros. 
    */

    //1-escolher o tipo que será usado
    int tipo = arq_index->tipoDadoInt;
    //2-mover o cursor do arquivo de dados para o fim (onde será feita a escrita)
    fseek(arq_dados->arqDados, 0, SEEK_END);

    //3-alocar o vetor temporário para alocar os registros nele
    criarVetTemp(arq_index, qtdInserir);
    
    //4-selecionar os dados usados, bem como as funções
    dados_t *reg = alocar_dados();
    FncGetCampoIndexIndexado fncsGetCampoIndexado[] = {getCampoInt, getCampoStr};
    FncCampoNulo fncsCampoNulo[] = {campoNulo_int, campoNulo_str};

    //5-inserir os registros válidos no vetor temporário e no arquivo de dados
    int pos_inserir = 0;
    int contador = qtdInserir;
    while(contador>0){

        //5.1-ler o registro e ver se o campo indexado é nulo
        leRegStdin(reg);
        void *campoIndexado = fncsGetCampoIndexado[tipo](reg, arq_index->campoIndexado);
        int eh_campo_nulo = fncsCampoNulo[tipo](campoIndexado);

        if(eh_campo_nulo==0){
            //5.2-Se não é campo nulo, insiro completamente (no arquivo de dados e no vetor armazenado em arq_index)
            inserirReg(arq_dados, arq_index, reg, pos_inserir);
            pos_inserir++;
        }else{
            //5.3-Se o campo é nulo, insiro somente no arquivo de dados
            prepara_para_escrita(reg);
            escrever_bin_registro_dados(reg, arq_dados->arqDados, arq_dados->cabecalhoDados);
            cabecalho_nroRegArq_incrementar(arq_dados->cabecalhoDados, 1);
        }
        contador--;
    }

    //6-Inserir os registros, que foram adicionados, no vetor original. Para isso:
    //6.0-devo selecionar a função de cópia, a quantidade anterior e o vetorOriginal que será usado
    FncCopiaVet fncsCopiaVet[] = {copiaVetInt, copiaVetStr};
    int qtdRegOriginal = get_qtdReg(arq_index->cabecalhoIndex);
    //6.1-devo realocar o vetor original
    realocar_vet_index(arq_index, qtdRegOriginal, pos_inserir);
    //6.2-copiar os dados que foram inseridos no temporário para o original
    void *vetOriginal = escolhe_vet_indx(arq_index);
    fncsCopiaVet[tipo](vetOriginal,arq_index->vetTemp,qtdRegOriginal,qtdRegOriginal+pos_inserir-1,0,pos_inserir-1);
    set_qtdReg(arq_index->cabecalhoIndex, qtdRegOriginal+pos_inserir);
}

void inserirReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg, int pos){
    /*
        Função que insere um registro no final do arquivo de dados e tenta inserir o campo indexado e byteOffSet 
    no vetor temporário armazenado em arq_index->vetTemp.
        Entradas:
            -reg: registro que será inserido.
            -arq_index: possui o vetTemp o qual será possivelmente inserido o indexamento do registro
            -arq_dadso: possui o arquivo de dados no qual será inserido o registro recebido
        Comportamento:
            Ao longo da função, a quantidade de registros armazenados no vetor temporário (em arq_index->qtdReg_vetTemp)
            pode ser incrementada o cursor do arquivo de dados começa no final do arquivo e também termina no final do
            arquivo (pois o registro foi escrito). A variável proxByteOffset armazenada no cabeçalho do arquivo de dados
            será alterada, bem como a variável nroRegArq será incrementada.
    */
    int tipDado = arq_index->tipoDadoInt;

    FncCampoNulo fncsCampoNulo[] = {campoNulo_int,campoNulo_str};
    FncGetCampoIndexIndexado fncsGetCampo[] = {getCampoInt,getCampoStr};

    void *campoIndexado = malloc(sizeof(void));
    int eh_campo_nulo;
    
    //obtenho o campo indexado (int ou str)
    campoIndexado = fncsGetCampo[tipDado](reg, arq_index->campoIndexado);

    //Vejo se o campo é nulo ou não (para poder indexá-lo)
    eh_campo_nulo = fncsCampoNulo[tipDado](campoIndexado);

    if(eh_campo_nulo == 0){//Se não é campo nulo
        int antigoQtdReg = arq_index->qtdReg_vetTemp;
        arq_index->qtdReg_vetTemp++;

        long int byte_reg_inserir;
        byte_reg_inserir = get_proxByteOffset(arq_dados->cabecalhoDados);

        //Com o campo indexado e com o byteOffSet, consigo criar o indexamento do registro a ser inserido

        void *dadoInserir = malloc(sizeof(void));

        FncSetDadoIndx fncsSetDadoIndx[] = {setDadoIndxInt,setDadoIndxStr};
        FncSetVetIndx fncsSetVetIndx[] = {setVetIndx_int,setVetIndx_str};

        dadoInserir = escolhe_indx_dado(arq_index);

        fncsSetDadoIndx[tipDado](dadoInserir, byte_reg_inserir, campoIndexado);
        fncsSetVetIndx[tipDado](arq_index->vetTemp, pos, dadoInserir);
    }

    //Escrevo no registro de dados
    prepara_para_escrita(reg);
    escrever_bin_registro_dados(reg,arq_dados->arqDados,arq_dados->cabecalhoDados); 
    //incremento o nroRegArq
    cabecalho_nroRegArq_incrementar(arq_dados->cabecalhoDados, 1);
}

int modificaCampoIndexado(ArqIndex_t *arq_index, InfoBusca_t *altera){
    //Retorna 1 se o campoIndexado é alterado, 0 caso contrário
    for(int i = 0; i < altera->qtd_crit; ++i){
        if(strcmp(arq_index->campoIndexado, altera->nomes[i])==0){
            return 1;
        }
    }
    return 0;
}

int obterPosicaoRegVetIndx(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg_atual, long int byteOffSet){
    //Obter a posição do registro dentro do vetIndex a partir do byteOffSet e do campo indexado.

    int tipoDado = arq_index->tipoDadoInt;

    void *dadoIndex = escolhe_indx_dado(arq_index);
    
    FncGetCampoIndexIndexado fncsGetCampo[] = {getCampoInt, getCampoStr};
    void *chave = fncsGetCampo[tipoDado](reg_atual, arq_index->campoIndexado);

    FncSetDadoIndx fncsSetDadoIndx[] = {setDadoIndxInt, setDadoIndxStr};
    fncsSetDadoIndx[tipoDado](dadoIndex, byteOffSet, chave);

    int inicio = 0;
    int fim = arq_index->qtdReg_vetTemp - arq_index->nro_addVetTemp;

    FncComparacao fncsComparacao[] = {comparacao_vet_dados_indx_int_RegIndx, comparacao_vet_dados_indx_str_RegIndx};
    int pos = busca_bin_rec(arq_index->vetTemp, inicio, fim, dadoIndex, fncsComparacao[tipoDado]);

    return pos;
}

void modificaReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg_antigo, InfoBusca_t *alteracoes,long int byteOffSet){
    /*
        Função que modifica um registro e o reescreve no arquivo de dados e, possivelmente, no vetor
        temporário (armazenado em memória primária em arq_index->vetTemp).
        Entradas:
            -arq_dados: possui o arquivo de dados no qual será inserido o registro modificado
            -arq_index: possui o vetor temporário o qual pode ser modificado
            -reg_antigo: o registro antes das alterações
            -alteracoes: carrega as informações que serão alteradas no vetor
            -byteOffSet: o byteOffSet atual do registro 'reg_antigo'
        Comportamento:
            A função pode modificar a variável arq_index->qtdReg_vetTemp, a variável arq_index->nro_addVetTemp,
            bem como a quantidade de registros removidos (nroRegRem) dentro do cabeçalho do arquivo de dados
            (armazenado em memória primária). O cursor do arquivo de dados, no começo, se encontra no final
            registro lido (reg_antigo) e termina no mesmo lugar (final do registro) 
    */

    dados_t *reg_modificado = alocar_dados();
    copia_registro(reg_modificado, reg_antigo);

    //Aplica as alterações no registro que foi passado para função 
    fazAlteracoes(reg_modificado, alteracoes->nomes, alteracoes->vals_str, alteracoes->vals_int, alteracoes->qtd_crit);

    int campo_modificado_eh_nulo;//flag
    int qtd_reg = arq_index->qtdReg_vetTemp;
    int tipoDado = arq_index->tipoDadoInt;
    int tam_reg_antigo = len_reg_dados(reg_antigo);
    int tam_reg_modificado = len_reg_dados(reg_modificado);

    //Obtenho a posição do registro (reg_antigo) dentro do vetor temporário
    int pos = obterPosicaoRegVetIndx(arq_dados, arq_index, reg_antigo, byteOffSet);

    //Devo ver se o campo indexado é nulo. Para isso:
    void *campoIndexado;
    FncGetCampoIndexIndexado fncsGetCampoIndexado[] = {getCampoInt,getCampoStr};
    FncCampoNulo fncsCampoNulo[] = {campoNulo_int, campoNulo_str};
    //1-pego o novo campoIndexado (que foi modificado)
    campoIndexado = fncsGetCampoIndexado[tipoDado](reg_modificado, arq_index->campoIndexado);
    //2-vejo se esse campo é nulo
    campo_modificado_eh_nulo = fncsCampoNulo[tipoDado](campoIndexado);
    
    //flag que avisa se modificou o campo indexado
    int modifica_campo_indexado = modificaCampoIndexado(arq_index, alteracoes);
    
    //Devo voltar o cursor do arqDados para o começo do registro que acabei de ler.
    fseek(arq_dados->arqDados, byteOffSet, SEEK_SET);

    if(tam_reg_antigo >= tam_reg_modificado){//Se o registro cabe na posição atual...
        
        reescrever_registro_dados(reg_modificado, arq_dados->arqDados);
        completaRegistroComDollar(arq_dados->arqDados, tam_reg_antigo - tam_reg_modificado);
        
        if(modifica_campo_indexado){//Se modifica o campo indexado...
            
            if(pos != -1){//Se o registro tinha campo indexado...
                
                //Removo do vetTemp
                desindexaRegistro(arq_index, pos);
            }

            if(campo_modificado_eh_nulo == 0){//Se o campo modificado não é nulo

                //Reescrevo o indexamento no vetTemp
                //a qtd_reg pode ter mudado, pois posso ter desidexado o registro, então
                qtd_reg = arq_index->qtdReg_vetTemp;
                
                realocar_vetIndxTemp(arq_index, qtd_reg, 1);
                arq_index->qtdReg_vetTemp++;

                void *dadoIndx = escolhe_indx_dado(arq_index);
                FncSetVetIndx fncsSetVetIndx[] = {setVetIndx_int,setVetIndx_str};
                FncSetDadoIndx fncsSetDadoIndx[] = {setDadoIndxInt,setDadoIndxStr};

                fncsSetDadoIndx[tipoDado](dadoIndx, byteOffSet, campoIndexado);//configuro o novo dado
                fncsSetVetIndx[tipoDado](arq_index->vetTemp, qtd_reg, dadoIndx);//coloco na última posição
                arq_index->nro_addVetTemp++;
            }
        }

    }else{//Se não cabe
        
        //marco como removido
        escreverCampoRemovido(arq_dados->arqDados);
        //incremento o nro_RegRem
        setCabecalhoDados_nroRegRem(arq_dados->cabecalhoDados, get_nroRegRem(arq_dados->cabecalhoDados)+1);

        if(pos != -1){//Se o registro tinha campo indexado...
            
            //Removo do vetTemp
            desindexaRegistro(arq_index, pos);
        }
        
        //Escrevo no final. Para isso:
        //1- realoco se o campoIndexado não for nulo
        if(campo_modificado_eh_nulo == 0){//campo modificado não é nulo
            //incremento o número de registros

            //Já que o campo modificado não é nulo, devo realocar o vetor,
            //pois em inserirReg(), irei adicionar esse registro ao vetTemp
            realocar_vetIndxTemp(arq_index, arq_index->qtdReg_vetTemp, 1);
            arq_index->nro_addVetTemp++;
        }
        //2- vou para o final do arquivo de dados e faço o append da função 6
        fseek(arq_dados->arqDados, 0, SEEK_END);

        inserirReg(arq_dados, arq_index, reg_modificado, arq_index->qtdReg_vetTemp);

        //volto o cursor para o final do registro
        fseek(arq_dados->arqDados, tam_reg_antigo + byteOffSet, SEEK_SET);
    }
}

void deletarRegistro(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *registro, InfoBusca_t *ignorar, long int byteOffset){  

    //faço a remoção lógica do registro
    remocaoLogica(registro,arq_dados->cabecalhoDados);

    //Agora devo reescrever o campo que foi atualizado no arquivo de dados. Para isso preciso:

    //-retornar à posição inicial do registro no arquivo de dados;
    fseek(arq_dados->arqDados,byteOffset,SEEK_SET);

    //-e escrever efetivamente 
    /*aqui são reescritos até mesmo os campos que não sofrem alterações, 
    pois o cursor do arquivo deve chegar até o final do registro*/
    reescrever_registro_dados(registro, arq_dados->arqDados);

    /*Agora, devo remover o registro do arquivo de índice, se necessário. Para removê-lo de lá,
    preciso de sua posição no vetor de dados do arquivo de índice. Como o 'deletarRegistro()' 
    recebe apenas o byteOffset desse registro no arquivo de dados, devo buscar a posição dele 
    no vetor de dados do arquivo de índice.*/
    int pos = obterPosicaoRegVetIndx(arq_dados, arq_index, registro, byteOffset);
    if(pos != -1){
        //Se a posição é válida, significa que o registro está indexado. 
        //Assim, removo o registro do arquivo de índice a partir de sua posição no vetor de dados
        desindexaRegistro(arq_index,pos);
    }
    //se é inválida, o registro não está indexado, então não preciso fazer nada
}

void desindexaRegistro(ArqIndex_t *arq_index, int pos){
    //função que remove um registro do arquivo de índice por meio de informações temporárias.
    
    //decremento a quantidade de registros indexados no vetor temporário;
    int qtdReg_ant = arq_index->qtdReg_vetTemp;//quantidade de registros antes de remover 1
    arq_index->qtdReg_vetTemp = qtdReg_ant-1;//quantidade agora que removi 1

    //Agora quero fazer o shift das posições do vetor de dados, a partir da pos que se deseja remover
    //Para isso:

    //-defino um vetor de funções que fazem o shift para tipos específicos
    typedef void (*FncShiftar)(void *,int, int);
    FncShiftar fncsShiftar[] = {shiftarVetIndxInt,shiftarVetIndxStr};

    //-descubro qual é o tipo do vetor de dados que vou usar (0 é inteiro e 1 é string)
    int tipo_dado = arq_index->tipoDadoInt;

    //-por fim, chamo a função que faz o shift 
    fncsShiftar[tipo_dado](arq_index->vetTemp,pos,qtdReg_ant-1);

    //-em seguida, realoco o tamanho do vetor
    realocar_vetIndxTemp(arq_index, qtdReg_ant, (-1));
}

void copiaVetTemp(void *ponteiro, int ignorar){

    ArqIndex_t *arq_index = (ArqIndex_t *) ponteiro;

    //Agora que fiz as alterações no vetor temporário, devo passá-las para o original. Para isso:

    //-pego os valores de quantidade de registros no arquivo de indice (original e modificado)
    int qtdReg_ant = get_qtdReg(arq_index->cabecalhoIndex);
    int qtdReg_agr = arq_index->qtdReg_vetTemp;

    //-descubro a diferenca entre eles
    int acrescimo = qtdReg_agr - qtdReg_ant ;

    //-realoco o vetor original para o tamanho do modificado
    realocar_vet_index(arq_index,qtdReg_ant,acrescimo);

    //-copio o qtdReg do mofidicado para o original
    setCabecalhoIndex(arq_index->cabecalhoIndex,'0',qtdReg_agr);
    
    //Agora, devo copiar os valores do vetor, seja qual tipo ele for. Para isso:

    //-Defino um vetor de funções que fazem a copia para um tipo específico
    FncCopiaVet fncsCopiaValores[] = {copiaVetInt, copiaVetStr};
    //-descubro o tipo do vetor (0 int, 1 string)
    int tipo_dado = arq_index->tipoDadoInt;

    //-descubro qual é o vetor original
    void *vetor_original = escolhe_vet_indx(arq_index);

    //-Por fim, faço a cópia dos valores, independente do tipo dos vetores
    fncsCopiaValores[tipo_dado](vetor_original,arq_index->vetTemp, 0, qtdReg_agr-1, 0, qtdReg_agr-1);
}

void reiniciarCursorIndex(ArqIndex_t *arq_index){
    fseek(arq_index->arqIndex,0,SEEK_SET);
}

void reiniciarCursorDados(ArqDados_t *arq_dados){
    fseek(arq_dados->arqDados,0,SEEK_SET);
}

void alterarStatusIndex(ArqIndex_t *arq_index, int status){
    if(status == 1){
        setStatusIndex(arq_index->cabecalhoIndex,'1');
    }else if(status == 0){
        setStatusIndex(arq_index->cabecalhoIndex,'0');
    }
}

void alterarStatusDados(ArqDados_t *arq_dados, int status){
    if(status == 1){
        setStatusDados(arq_dados->cabecalhoDados,'1');
    }else if(status == 0){
        setStatusDados(arq_dados->cabecalhoDados,'0');
    }
}

void escreverStatusDados(ArqDados_t *arq_dados){
    fwriteStatusDados(arq_dados->arqDados, arq_dados->cabecalhoDados);
}

void escreverStatusIndex(ArqIndex_t *arq_index){
    fwriteStatusIndex(arq_index->arqIndex, arq_index->cabecalhoIndex);
}

void escreverCabecalhoDados(ArqDados_t *arq_dados){
    fwriteCabecalhoDados(arq_dados->arqDados, arq_dados->cabecalhoDados);
}

void escreverCabecalhoIndex(ArqIndex_t *arq_index){
    escreveCabecalhoIndex(arq_index->arqIndex, arq_index->cabecalhoIndex);
}