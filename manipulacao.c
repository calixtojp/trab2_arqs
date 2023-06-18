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

void set_acoes(FncAcoes *acoes, FncAcaoRegArv reg_arv, FncAcaoRegSeq reg_seq, FncAcaoBranch no, FncAcaoFinal final){
    acoes->reg_arvore = reg_arv;
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

int arvore_vazia(Arvore_t *arvore){//Função que verifica se a árvore está vazia
    if(get_nroNiveis(arvore->cabecalhoArvore) == 0){
        return 1;
    }else{
        return 0;
    }
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

InfoDados_t *ler_dados_registro(int(*metodoLeitura)(dados_t *, FILE *), ArqDados_t *arq_dados){
    InfoDados_t *info = alocar_InfoDados(6);
    dados_t *reg = alocar_dados();
    metodoLeitura(reg, arq_dados->arqDados);
    printf("reg lido:\n");
    mostrar_campos(reg);

    regDados_para_vetores(reg, info->nomes, info->vals_int, info->vals_str);

    desalocar_registro(reg);

    return info;
}

void mostrar_info_dados(InfoDados_t *criterios){
    for(int i=0; i<criterios->qtd_crit; ++i){
        printf("%s: str:%s | int:%d\n", criterios->nomes[i], criterios->vals_str[i], criterios->vals_int[i]);
    }
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

void buscaArvoreRec(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, FncAcoes *acoes, 
                    void *info_aux, int chave_busca, pagina_t *pgn_mae, int RRN_atual){
    /*Função recursiva que faz a busca de um valor de idCrime em uma árvore B* cujo campo indexado 
    é idCrime. Esse valor buscado é o 'int chave'. O parâmetro 'int RRN_atual' representa o RRN 
    do nó que está sendo verificado. Quando encontrado um nó dessa árvore que contém o idCrime 
    buscado, pode-se fazer uma ação sobre todo o nó ou sobre o registro que possui esse idCrime.*/

    if(RRN_atual == -1){//critério de parada
        //Nó inexistente, ou seja, não há registro com o idCrime buscado

        //chamo a ação com um byteOffset nulo
        acoes->reg_arvore(arq_dados, criterios, -1);
        return;
    }

    //Leio a página que quero verificar
    pagina_t *pgn_atual = aloca_pagina();//crio a página
    pgn_atual->RRN_no = RRN_atual;//configuro seu RRN
    fseek(arvore->arqArvore, (RRN_atual+1) * arvore->tam_pagina, SEEK_SET);//movo o cursor para ler
    fluxo_no(arvore->arqArvore, pgn_atual->no, meu_fread);//realizo a leitura

    /*Defino P como sendo o ponteiro para o próximo nó (na próxima
    chamada recursiva dessa função, ele será o 'RRN_atual').
    Atribuo um valor nulo de início.*/
    int P = -1;
    
    //Defino Pr como sendo o byteOffset do registro encontrado.
    long long int Pr =  -1;

    /*Busco a chave no nó atual. Se encontrar, coloco seu byteOffset em 'Pr'. Se não, coloco
    o RRN do próximo nó no qual devo buscar em 'P'*/
    Pr = buscaBinNo(pgn_atual->no, 0, get_nChaves(pgn_atual->no) - 1, chave_busca, &P);

    if(Pr == -1){
        //se não encontrou o valor buscado no nó lido, 
        //chamo a função recursivamente para o próximo nó (valor armazenado em P)
        buscaArvoreRec(arq_dados, arvore, criterios, acoes, info_aux, chave_busca, pgn_atual, P);
        // insercao(arvore, pgn_mae, pgn_atual, info_aux);
        acoes->no(arvore, pgn_mae, pgn_atual, info_aux);
    }else{
        //Se encontrou, retorno o byteOffset
        acoes->reg_arvore(arq_dados, criterios, Pr);
    }

    desaloca_pagina(pgn_atual);
}

void buscaArvore(ArqDados_t *arq_dados, Arvore_t *arvore, int pos_crit, InfoDados_t *criterios, FncAcoes *acoes, void *info_aux){
    int RRN_raiz = get_noRaiz(arvore->cabecalhoArvore);

    //chamo a função recursiva de busca na árvore B*, a partir do nó raiz
    buscaArvoreRec(arq_dados, arvore, criterios, acoes, info_aux, criterios->vals_int[pos_crit], NULL, RRN_raiz);
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

void processaRegistros(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, FncAcoes *acoes, void *info_aux){
    /*Funcao que define se a busca sera binaria na arvore B* ou sequencial no arquivo de dados e encontra os registros. 
    Depois, usa a FncAcao acao, e a FncFinaliza final para processá-los.*/

    int posicao_criterio = busca_por_indexado(criterios);
    
    if(posicao_criterio >= 0 && get_noRaiz(arvore->cabecalhoArvore) != -1){
        /*se a árvore B* não é vazia e indexa algum dos critérios de busca, então faço a busca usando-a.*/
        buscaArvore(arq_dados,arvore,posicao_criterio,criterios,acoes,info_aux);
    }else{
        //se não, faz-se busca sequencial no arquivo de dados
    
        //Para isso, deve-se reiniciar o ponteiro do arquivo de dados para o primeiro registro de dados (pulando o cabecalho) 
        //para fazer um novo processamento, pois não há garantia de que o ponteiro esteja corretamente posicionado
        fseek(arq_dados->arqDados,len_cabecalho_dados(),SEEK_SET);
        //em seguida, chama-se a função que realiza a busca sequencial
        buscaSeqDados(arq_dados, arvore, criterios, acoes);
    }
}

void insercao_arqDados(ArqDados_t *arq_dados, InfoDados_t *info_inserir){
    
    //Insiro o registro no arquivo de dados. Para isso:
    dados_t *reg_inserir = alocar_dados();//1-Devo criar o registro que irei inserir, a partir dos critérios de busca.
    //2- Passo o dados de info_inserir para o registro
    vetores_para_regDados(reg_inserir, info_inserir->nomes, info_inserir->vals_str, info_inserir->vals_int, info_inserir->qtd_crit);
    printf("Registro que irei inserir no arquivo de dados:\n");
    mostrar_campos(reg_inserir);

    prepara_para_escrita(reg_inserir);//3-Preparar o registro para escrita
    escrever_bin_registro_dados(reg_inserir,arq_dados->arqDados,arq_dados->cabecalhoDados);//4-Escrever o registro
    cabecalho_nroRegArq_incrementar(arq_dados->cabecalhoDados, 1);//5-incremento o nroRegArq

    desalocar_registro(reg_inserir);
}

/*---------------------------------------AÇÕES-------------------------------------------*/
void insercao_arvore(Arvore_t *arvore, pagina_t *pgn_mae, pagina_t *pgn_atual, void *info_aux){

    //Nessa ação, a info_aux será a informação inserida. Logo, devo fazer o casting.
    InfoInserida_t *info_inserir = (InfoInserida_t *) info_aux;

    if(info_inserir->info_valida != 1){//Se não é uma informação de inserção válida, só dou return.
        return;
    }

    //Insiro o registro no arquivo árvore.
    if(get_nroNiveis(arvore->cabecalhoArvore) == 0){//Se a árvore for vazia
        printf("arvore vazia\n");
        set_nroNiveis(arvore->cabecalhoArvore, 1);//configuro a nova altura da árvore

        //configuro o novo nó raiz,
        no_arvore_t *no_raiz = alocar_no();
        set_nivel_no(no_raiz, get_nroNiveis(arvore->cabecalhoArvore));//o qual possui o nível mais alto
        set_nChaves(no_raiz, 0);//e inicialmente não possui nenhuma chave.

        insere_ordenado_no(no_raiz, info_inserir->chave_inserida);//escrevo as informações do nó

        fluxo_no(arvore->arqArvore, no_raiz, meu_fwrite);//escrevo o nó em memória secundária
    }else{//Se a árvore não é vazia
        printf("arvore não está vazia\n");
        if(get_nChaves(pgn_atual->no) < get_ordem_arvore()-1){//Se a nova chave cabe na página atual
            printf("cabe no nó atual\n");
            insere_ordenado_no(pgn_atual->no, info_inserir->chave_inserida);
            
            //Já que consegui inserir ordenado no nó, garanto que não há promoção de chaves para os nós acima.
            //Então devo indicar para os nós acima que a informação de inserção é inválida, pois já foi inserida nesse nó.
            info_inserir->info_valida = -1;//
        }else{//Se a nova chave não cabe na página atual
            printf("não cabe no nó atual\n");
            if(get_nivel_no(pgn_atual->no) == get_nroNiveis(arvore->cabecalhoArvore)){//Se estou inserindo no nó raiz
                //Faço o split_1_para_2
                split_1_para_2(arvore->arqArvore,arvore->cabecalhoArvore,pgn_atual,info_inserir);
            }else{//Se estou inserindo em qualquer nó que não no raiz
                //tento a redistribuição
                pagina_t *pgn_irma;
                result_redistribuicao_t resultado = redistribuicao(arvore->arqArvore, pgn_mae, pgn_atual, pgn_irma,info_inserir);

                if(resultado == redistribuiu){//Se conseguiu redistribuir
                    //Então devo indicar para os nós acima que a informação de inserção é inválida, pois já foi inserida nesse nó.
                    info_inserir->info_valida = -1;
                }else{//Se não conseguiu redistribuir:
                    if(resultado == retorna_esq){//Ou seja, o ponteiro pgn_irma aponta para página irmã à esquerda
                        split_2_para_3(arvore->arqArvore, arvore->cabecalhoArvore, pgn_mae, pgn_irma, pgn_atual, info_inserir);
                    }else{//Ou seja (resultado == retorna_dir), o ponteiro pgn_irma aponta para página irmã à direita
                        split_2_para_3(arvore->arqArvore, arvore->cabecalhoArvore, pgn_mae, pgn_atual, pgn_irma, info_inserir);
                    }
                }

                desaloca_pagina(pgn_irma);
            }
        }
    }
}

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

void NoOpAcaoBranch(Arvore_t *ignorar1, pagina_t *ignorar2, pagina_t *ignorar3, void *ignorar4){
    //função do tipo FncAcaoNo que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoFinal(int){
    //função do tipo FncAcaoFinal que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoRegArv(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3){
    //função do tipo FncAcaoRegArv que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoRegSeq(dados_t *ignorar1){
    //função do tipo FncAcaoRegSeq que não faz nada, para os casos em que funções desse tipo não serão usadas
}

/*---------------------------------------DEMAIS FUNÇÕEs-------------------------------------------*/

InfoInserida_t *criar_InfoInserida(InfoDados_t *info_dados){
    InfoInserida_t *inserida_retorno = alocar_InfoInserida();
    inserida_retorno->info_valida = 1;//Configuro como uma informação válida
    for(int i = 0; i < info_dados->qtd_crit; ++i){//Laço que acha o idCrime e escreve-o na chave_t.
        if(strcmp(info_dados->nomes[i], "idCrime")==0){
            set_chaves_C(inserida_retorno->chave_inserida, info_dados->vals_int[i]);
            set_chaves_Pr(inserida_retorno->chave_inserida, -1);
            break;
        }
    }

    return inserida_retorno;
}

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
