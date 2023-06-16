#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao.h"
#include "auxiliares.h"
#include "arq_dados.h"
#include "arvore_b_estrela.h"

#define MAX_NOME_ARQ 50
#define TAM_MAX_NOME 50
#define TAM_MAX_VALOR 50
#define campoIndexado "idCrime"
struct ArqDados{
    char nomeArqDados[MAX_NOME_ARQ];//guarda o nome do arquivo de dados
    FILE *arqDados;
    cabecalho_t *cabecalhoDados;
};

struct Arvore{
    char nomeArqArvore[MAX_NOME_ARQ];
    FILE *arqArvore;
    cabecalho_arvore_t *cabecalhoArvore;
    int M;
    int tam_pagina;    
};

struct InfoDados{
    int qtd_crit;
    char **nomes;
    char **vals_str;
    int *vals_int;
};

ArqDados_t *alocar_arq_dados(void){
    //Aloca o cabeçalho e o tipo ArqDados_t

    ArqDados_t *arq_dados_main;
    arq_dados_main = malloc(sizeof(ArqDados_t));

    arq_dados_main->cabecalhoDados = alocar_cabecalho_dados(); //MUDAR estou alocando o cabeçalho aqui, mas na função de leitura aloco novamente

    return arq_dados_main;
}

Arvore_t *alocar_arvore(void){
    Arvore_t *arvore_retorno = malloc(sizeof(Arvore_t));
    arvore_retorno->cabecalhoArvore = alocar_cabecalho_arvore();
    arvore_retorno->M = get_ordem_arvore();
    arvore_retorno->tam_pagina = get_tam_pagina();
    return arvore_retorno;
}

FncAcoes *alocar_acoes(){
    return malloc(sizeof(FncAcoes));
}

void desalocar_acoes(FncAcoes *acoes){
    free(acoes);
}

void set_acoes(FncAcoes *acoes, FncAcaoRegArv reg_arv, FncAcaoRegSeq reg_seq, FncAcaoNo no, FncAcaoFinal final){
    acoes->reg_arv = reg_arv;
    acoes->reg_seq = reg_seq;
    acoes->no = no;
    acoes->final = final;
}

void ler_nome_arq_dados(ArqDados_t *arq_dados){
    int retorno_scanf = scanf(" %s", arq_dados->nomeArqDados); 
    if(retorno_scanf != 1){
        mensagem_erro();
    }
}

void ler_nome_arvore(Arvore_t *arvore){
    int retorno_scanf = scanf(" %s", arvore->nomeArqArvore); 
    if(retorno_scanf != 1){
        mensagem_erro();
    }
}

void abrir_arq_dados(ArqDados_t *arq_dados, const char *tipo_leitura){
    arq_dados->arqDados = fopen(arq_dados->nomeArqDados, tipo_leitura);
    if(arq_dados->arqDados == NULL){
        mensagem_erro();
    }
}

void iniciar_arvore(Arvore_t *arvore, const char *tipo_leitura){
    arvore->arqArvore = fopen(arvore->nomeArqArvore, tipo_leitura);
    if(arvore->arqArvore == NULL){
        mensagem_erro();
    }
}

void desalocar_ArqDados(ArqDados_t *arq_dados){
    free(arq_dados->cabecalhoDados);
    free(arq_dados);
}

void desalocar_Arvore(Arvore_t *arvore){
    free(arvore->cabecalhoArvore);
    free(arvore);
}

void fechar_arq_dados(ArqDados_t *arq_dados){
    fclose(arq_dados->arqDados);
}

void fechar_arvore(Arvore_t *arvore){
    fclose(arvore->arqArvore);
}

void ler_cabecalho_dados(ArqDados_t *arq_dados){
    arq_dados->cabecalhoDados = ler_dados_cabecalho(arq_dados->arqDados);
}

void ler_cabecalho_arvore(Arvore_t *arvore){
    fluxo_CabecalhoArvore(arvore->arqArvore, arvore->cabecalhoArvore, meu_fread);
}

char *getNomeArqDados(ArqDados_t *arq_dados){
    return arq_dados->nomeArqDados;
}

char *getNomeArvore(Arvore_t *arvore){
    return arvore->nomeArqArvore;
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

void levaFinalCursorDados(ArqDados_t *arq_dados){
    fseek(arq_dados->arqDados, 0, SEEK_END);
}

InfoDados_t *alocar_InfoDados(int qtd_crit){
    InfoDados_t *informacao = malloc(sizeof(InfoDados_t));
    informacao->nomes = alocar_vetor_string(qtd_crit,TAM_MAX_NOME);
    informacao->vals_str = alocar_vetor_string(qtd_crit, TAM_MAX_VALOR);
    informacao->vals_int = malloc(sizeof(int)*qtd_crit);
    informacao->qtd_crit = qtd_crit;

    return informacao;
}

InfoDados_t *ler_criterios_busca(){
    int m;
    scanf("%d",&m);

    InfoDados_t *criterios = alocar_InfoDados(m);

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
    return criterios;
}

void desalocar_InfoDados(InfoDados_t *informacao){
    desalocar_vetor_string(informacao->nomes,informacao->qtd_crit);
    desalocar_vetor_string(informacao->vals_str,informacao->qtd_crit);
    free(informacao->vals_int);
    free(informacao);
}
int testarStatusArvore(Arvore_t *arvore){

	//funcao que retorna 1 caso o arquivo esteja consistente e 0 caso esteja inconsistente
    if(getStatusArvore(arvore->cabecalhoArvore) == '1'){
        return 1;
    }
    return 0;
}

int testarStatusDados(ArqDados_t *arq_dados){
	//funcao que retorna 1 caso o arquivo esteja consistente e 0 caso esteja inconsistente
	if(getStatusDados(arq_dados->cabecalhoDados) == '1'){
		return 1;
	}
	return 0;
}

int busca_por_indexado(InfoDados_t *criterios){
    /*Função que, se o vetor de nomes (lido da entrada da busca) 
    contiver o nome do campo indexado (idCrime), retorna o índice dessa string no vetor. 
    Caso contrário, retorna -1.*/

    /*Essa função determina se deve ser realizada 
    busca pela árvore B* ou sequencial no arquivo de dados.*/

    for(int i=0; i<criterios->qtd_crit; i++){
        if(strcmp(campoIndexado,criterios->nomes[i])==0){
            return i;
        }
    }
    return -1;
}

void buscaArvoreRec(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, FncAcoes *acoes, int *chave, 
                        chave_t *chave_promovida, int RRN_atual){
    /*Função recursiva que faz a busca de um valor de idCrime em uma árvore B* cujo campo indexado 
    é idCrime. Esse valor buscado é o 'int chave'. O parâmetro 'int RRN_atual' representa o RRN 
    do nó que está sendo verificado. Quando encontrado um nó dessa árvore que contém o idCrime 
    buscado, pode-se fazer uma ação sobre todo o nó ou sobre o registro que possui esse idCrime.*/

    if(RRN_atual == -1){//critério de parada
        //Nó inexistente, ou seja, não há registro com o idCrime buscado

        //chamo a ação com um byteOffset nulo
        acoes->reg_arv(arq_dados, criterios, -1);
        return;
    }

    //Leio o nó que quero verificar
    fseek(arvore->arqArvore, (RRN_atual+1) * arvore->tam_pagina, SEEK_SET);
    no_arvore_t *no_aux = alocar_no();
    fluxo_no(arvore->arqArvore, no_aux, meu_fread);

    /*Defino P como sendo o ponteiro para o próximo nó (na próxima
    chamada recursiva dessa função, ele será o 'RRN_atual').
    Atribuo um valor nulo de início.*/
    int P = -1;
    
    //Defino Pr como sendo o byteOffset do registro encontrado.
    long long int Pr =  -1;

    /*Busco a chave no nó atual. Se encontrar, coloco seu byteOffset em 'Pr'. Se não, coloco
    o RRN do próximo nó no qual devo buscar em 'P'*/
    Pr = buscaBinNo(no_aux, 0, get_nChaves(no_aux) - 1, *chave, &P);

    if(Pr == -1){
        //se não encontrou o valor buscado no nó lido, 
        //chamo a função recursivamente para o próximo nó (valor armazenado em P)
        buscaArvoreRec(arq_dados, arvore, criterios, acoes, chave, chave_promovida, P);
        /*insercao(RRN_atual, RRN_anterior, &chave_inserir, &chave_promovida, ponteiro_promovido)*/  
    }else{
        //Se encontrou, retorno o byteOffset
        acoes->reg_arv(arq_dados, criterios, Pr);
    }

    desalocar_no(no_aux);
}

void buscaArvore(ArqDados_t *arq_dados, Arvore_t *arvore, int pos_crit, InfoDados_t *criterios, FncAcoes *acoes){
    int RRN_raiz = get_noRaiz(arvore->cabecalhoArvore);

    chave_t *chave = alocar_chave();

    //chamo a função recursiva de busca na árvore B*, a partir do nó raiz
    buscaArvoreRec(arq_dados, arvore, criterios, acoes, &(criterios->vals_int[pos_crit]), chave, RRN_raiz);

    desalocar_chave(chave);
}

void buscaSeqDados(ArqDados_t *arq_dados, Arvore_t *arvore,InfoDados_t *criterios, FncAcoes *acoes){
    //função que faz busca sequencial no arquivo de dados

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

        if(testarCriterios(registro,criterios->nomes,criterios->vals_str,criterios->vals_int,criterios->qtd_crit)){
            ++cont;
            //se o registro satisfaz todos os criterios, realizo a ação 
            achei_reg_val = 1;//achei pelo menos 1 registro que satisfaz os critérios

            acoes->reg_seq(registro);
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
    
    acoes->final(achei_reg_val);
}

void processaRegistros(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, FncAcoes *acoes){
    /*Funcao que define se a busca sera binaria na arvore B* ou sequencial no arquivo de dados e encontra os registros. 
    Depois, usa a FncAcao acao, e a FncFinaliza final para processá-los.*/

    int posicao_criterio = busca_por_indexado(criterios);
    
    if(posicao_criterio >= 0 && get_noRaiz(arvore->cabecalhoArvore) != -1){
        /*se a árvore B* não é vazia e indexa algum dos critérios de busca, então faço a busca usando-a.*/
        buscaArvore(arq_dados,arvore,posicao_criterio,criterios,acoes);
    }else{
        //se não, faz-se busca sequencial no arquivo de dados
    
        //Para isso, deve-se reiniciar o ponteiro do arquivo de dados para o primeiro registro de dados (pulando o cabecalho) 
        //para fazer um novo processamento, pois não há garantia de que o ponteiro esteja corretamente posicionado
        fseek(arq_dados->arqDados,len_cabecalho_dados(),SEEK_SET);
        //em seguida, chama-se a função que realiza a busca sequencial
        buscaSeqDados(arq_dados, arvore, criterios, acoes);
    }
}

/*void insercao(Arvore_t *arvore, no_arvore_t *no_atual, no_arvore_t *no_anterior, chave_t *chave_inserir, int *ponteiro_promovido){

    // INSERCAO(RRN_atual, RRN_anterior, *chave_inserir, int *ponteiro_promovido):
    // Se é árvore vazia:
    //     cria raiz
    //     insere_ordenado(RRN_atual, chave_inserir, -1)
    // Se não:
    //     Se cabe no nó:
    //         insere_ordenado(RRN_atual, chave_inserir, ponteiro_promovido):
    //     Se não:
    //         se é raiz:

    //             split_1_pra_2(RRN_atual, chave_inserir, ponteiro_promovido)
    //         se não:
    //             redistribuiu = redistribuir()
    //             se não redistribuiu:
    //                 split_2_pra_3

    
    if(get_nroNiveis(arvore->cabecalhoArvore) == 0){//Se a árvore for vazia
        printf("arvore vazia\n");
        set_nroNiveis(arvore->cabecalhoArvore, 1);//configuro a nova altura da árvore

        //configuro o novo nó raiz,
        no_arvore_t *no_raiz = alocar_no();
        set_nivel_no(no_raiz, get_nroNiveis(arvore->cabecalhoArvore));//o qual possui o nível mais alto
        set_nChaves(no_raiz, 0);//e inicialmente não possui nenhuma chave.

        insere_ordenado_no(chave_inserir, no_raiz);//escrevo as informações do nó

        fluxo_no(arvore->arqArvore, no_raiz, meu_fwrite);//escrevo o nó em memória secundária
    }else{//Se a árvore não é vazia
        printf("arvore não está vazia\n");
        if(get_nChaves(no_atual) < get_ordem_arvore()-1){//Se a nova chave cabe no no_atual
            printf("cabe no nó atual\n");
            insere_ordenado_no(no_atual, chave_inserir);
        }else{//Se a nova chave não cabe no no_atual
            printf("não cabe no nó atual\n");
            if(get_nivel_no(no_atual) == get_nroNiveis(arvore->cabecalhoArvore)){//Se estou inserindo no nó raiz
                //Faço o split_1_para_2
                split_1_para_2();
            }else{//Se estou inserindo em qualquer nó que não no raiz
                //tento a redistribuição
                int conseguiu_redistribuir = redistribuicao(arvore->arqArvore, no_anterior, no_atual, chave_inserir);
                if(!conseguiu_redistribuir){//Se não conseguiu redistribuir,
                    //Faço o split_2_para_3
                    split_2_para_3();
                }

            }
        }
    }
}
*/

/*---------------------------------------AÇÕES-------------------------------------------*/
void achouReg(int flag){
    /*Funcao do tipo FncAcaoFinal que, após testar todos os registros do arquivo de dados, 
    informa que não existe registro que satisfaz os critérios de busca, caso isso seja verdade*/

    if(flag <= 0){
        printf("Registro inexistente.\n");
    }
}

void validaPrinta(ArqDados_t *arq_dados, InfoDados_t *criterios, long int byteOffset){
    /*Função do tipo FncAcaoRegArv que valida os critérios de busca por um registro. 
    Como essa função só é chamada dentro da função buscaArvoreRec(), e a árvore B* é 
    indexada apenas por idCrime, sabemos que a busca foi feita sobre o idCrime, que não 
    permite repetição. Ou seja, só há um registro que satisfaz o critério de busca por 
    idCrime, logo só falta confirmar se ele satisfaz os outros critérios. Se ele satisfaz, 
    ele deve ser printado. Se não satisfaz, posso afirmar que não existe registro que 
    satisfaça os critérios de busca.*/
    
    if(byteOffset != -1){
        fseek(arq_dados->arqDados, byteOffset, SEEK_SET);
        dados_t *registro = alocar_dados();
        ler_bin_registro(registro, arq_dados->arqDados);

        if(testarCriterios(registro, criterios->nomes, criterios->vals_str, criterios->vals_int, criterios->qtd_crit)){
            //se o registro satisfaz os critérios de busca, printo ele.
            mostrar_campos(registro);
            return;
        }
    }

    /*se o byteOffset é inválido, ou seja, não encontrado nenhum registro com o idCrime buscado,
    ou o registro encontrado não satisfaz os demais critérios de busca,
    indico que não há registro que satisfaz a busca*/
    printf("Registro inexistente.\n");
}

void printa_busca(dados_t *registro){
    //Função do tipo FncAcaoRegSeq que printa os campos de um registro
    
    mostrar_campos(registro);
}

void NoOpAcaoNo(int ignorar){
    //função do tipo FncAcaoNo que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoFinal(void){
    //função do tipo FncAcaoFinal que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoRegArv(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3){
    //função do tipo FncAcaoRegArv que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoRegSeq(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3){
    //função do tipo FncAcaoRegSeq que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void insere_reg(ArqDados_t *arq_dados, Arvore_t *arvore, dados_t *registro){
    printf("vou inserir o registro:\n");
    mostrar_campos(registro);
}
// int inserirRegStdin(ArqDados_t *arq_dados, ArqIndex_t *arq_index, int qtdInserir){
//     /*
//         Função que lê um registro da entrada padrão (por meio do scanf) e adiciona-o
//     no arquivo de dados e de index.

//         Entradas: 
//         -ArqDados_t na qual será inserido o registro no final
//     do arquivo de dados.
//         -ArqIndex_t no qual será inserido o registro lido (no vetor temporário carregado dentro da própria struct)
//         -int qtdInserir é a quantidade de registros que serão inseridos

//         Comportamento:
//         -A função altera o vetor temporário armazenado na struct ArqIndex_t, bem como a quantidade
//         de registros armazenados no vetor temporário (arq_index->qtdReg_vetTemp). Após utilizado o vetor temporário,
//         altera a quantidade de registros no cabeçalho do arquivo de index (carregado em RAM) e adiciona o vetor 
//         temporário ao vetor Original de registros. 
//     */

//     //1-escolher o tipo que será usado
//     int tipo = arq_index->tipoDadoInt;
//     //2-mover o cursor do arquivo de dados para o fim (onde será feita a escrita)
//     fseek(arq_dados->arqDados, 0, SEEK_END);

//     //3-alocar o vetor temporário para alocar os registros nele
//     criarVetTemp(arq_index, qtdInserir);
    
//     //4-selecionar os dados usados, bem como as funções
//     dados_t *reg = alocar_dados();
//     FncGetCampoIndexIndexado fncsGetCampoIndexado[] = {getCampoInt, getCampoStr};
//     FncCampoNulo fncsCampoNulo[] = {campoNulo_int, campoNulo_str};

//     //5-inserir os registros válidos no vetor temporário e no arquivo de dados
//     int pos_inserir = 0;
//     int contador = qtdInserir;
//     while(contador>0){

//         //5.1-ler o registro e ver se o campo indexado é nulo
//         leRegStdin(reg);
//         void *campoIndexado = fncsGetCampoIndexado[tipo](reg, arq_index->campoIndexado);
//         int eh_campo_nulo = fncsCampoNulo[tipo](campoIndexado);

//         if(eh_campo_nulo==0){
//             //5.2-Se não é campo nulo, insiro completamente (no arquivo de dados e no vetor armazenado em arq_index)
//             inserirReg(arq_dados, arq_index, reg, pos_inserir);
//             pos_inserir++;
//         }else{
//             //5.3-Se o campo é nulo, insiro somente no arquivo de dados
//             prepara_para_escrita(reg);
//             escrever_bin_registro_dados(reg, arq_dados->arqDados, arq_dados->cabecalhoDados);
//             cabecalho_nroRegArq_incrementar(arq_dados->cabecalhoDados, 1);
//         }
//         contador--;
//     }

//     //6-Inserir os registros, que foram adicionados, no vetor original. Para isso:
//     //6.0-devo selecionar a função de cópia, a quantidade anterior e o vetorOriginal que será usado
//     FncCopiaVet fncsCopiaVet[] = {copiaVetInt, copiaVetStr};
//     int qtdRegOriginal = get_qtdReg(arq_index->cabecalhoIndex);
//     //6.1-devo realocar o vetor original
//     realocar_vet_index(arq_index, qtdRegOriginal, pos_inserir);
//     //6.2-copiar os dados que foram inseridos no temporário para o original
//     void *vetOriginal = escolhe_vet_indx(arq_index);
//     fncsCopiaVet[tipo](vetOriginal,arq_index->vetTemp,qtdRegOriginal,qtdRegOriginal+pos_inserir-1,0,pos_inserir-1);
//     set_qtdReg(arq_index->cabecalhoIndex, qtdRegOriginal+pos_inserir);
// }

// void inserirReg(ArqDados_t *arq_dados, ArqIndex_t *arq_index, dados_t *reg, int pos){
//     /*
//         Função que insere um registro no final do arquivo de dados e tenta inserir o campo indexado e byteOffSet 
//     no vetor temporário armazenado em arq_index->vetTemp.
//         Entradas:
//             -reg: registro que será inserido.
//             -arq_index: possui o vetTemp o qual será possivelmente inserido o indexamento do registro
//             -arq_dadso: possui o arquivo de dados no qual será inserido o registro recebido
//         Comportamento:
//             Ao longo da função, a quantidade de registros armazenados no vetor temporário (em arq_index->qtdReg_vetTemp)
//             pode ser incrementada o cursor do arquivo de dados começa no final do arquivo e também termina no final do
//             arquivo (pois o registro foi escrito). A variável proxByteOffset armazenada no cabeçalho do arquivo de dados
//             será alterada, bem como a variável nroRegArq será incrementada.
//     */
//     int tipDado = arq_index->tipoDadoInt;

//     FncCampoNulo fncsCampoNulo[] = {campoNulo_int,campoNulo_str};
//     FncGetCampoIndexIndexado fncsGetCampo[] = {getCampoInt,getCampoStr};

//     void *campoIndexado = malloc(sizeof(void));
//     int eh_campo_nulo;
    
//     //obtenho o campo indexado (int ou str)
//     campoIndexado = fncsGetCampo[tipDado](reg, arq_index->campoIndexado);

//     //Vejo se o campo é nulo ou não (para poder indexá-lo)
//     eh_campo_nulo = fncsCampoNulo[tipDado](campoIndexado);

//     if(eh_campo_nulo == 0){//Se não é campo nulo
//         int antigoQtdReg = arq_index->qtdReg_vetTemp;
//         arq_index->qtdReg_vetTemp++;

//         long int byte_reg_inserir;
//         byte_reg_inserir = get_proxByteOffset(arq_dados->cabecalhoDados);

//         //Com o campo indexado e com o byteOffSet, consigo criar o indexamento do registro a ser inserido

//         void *dadoInserir = malloc(sizeof(void));

//         FncSetDadoIndx fncsSetDadoIndx[] = {setDadoIndxInt,setDadoIndxStr};
//         FncSetVetIndx fncsSetVetIndx[] = {setVetIndx_int,setVetIndx_str};

//         dadoInserir = escolhe_indx_dado(arq_index);

//         fncsSetDadoIndx[tipDado](dadoInserir, byte_reg_inserir, campoIndexado);
//         fncsSetVetIndx[tipDado](arq_index->vetTemp, pos, dadoInserir);
//     }

//     //Escrevo no registro de dados
//     prepara_para_escrita(reg);
//     escrever_bin_registro_dados(reg,arq_dados->arqDados,arq_dados->cabecalhoDados); 
//     //incremento o nroRegArq
//     cabecalho_nroRegArq_incrementar(arq_dados->cabecalhoDados, 1);
// }

void reiniciarCursorDados(ArqDados_t *arq_dados){
    fseek(arq_dados->arqDados,0,SEEK_SET);
}

void reiniciarCursorArvore(Arvore_t *arvore){
    fseek(arvore->arqArvore, 0, SEEK_SET);
}

void alterarStatusDados(ArqDados_t *arq_dados, int status){
    if(status == 1){
        setStatusDados(arq_dados->cabecalhoDados,'1');
    }else if(status == 0){
        setStatusDados(arq_dados->cabecalhoDados,'0');
    }
}

void alterarStatusArvore(Arvore_t *arvore, int status){
    if(status == 1){
        setStatusArvore(arvore->cabecalhoArvore,'1');
    }else if(status == 0){
        setStatusArvore(arvore->cabecalhoArvore,'0');
    }
}

void escreverStatusDados(ArqDados_t *arq_dados){
    fwriteStatusDados(arq_dados->arqDados, arq_dados->cabecalhoDados);
}

void escreverStatusArvore(Arvore_t *arvore){
    fluxo_StatusArvore(arvore->arqArvore, arvore->cabecalhoArvore, meu_fwrite);
}

void escreverCabecalhoDados(ArqDados_t *arq_dados){
    fwriteCabecalhoDados(arq_dados->arqDados, arq_dados->cabecalhoDados);
}

void escreverCabecalhoArvore(Arvore_t *arvore){
    fluxo_CabecalhoArvore(arvore->arqArvore, arvore->cabecalhoArvore, meu_fwrite);
}