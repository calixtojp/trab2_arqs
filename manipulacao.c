#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao.h"
#include "auxiliares.h"
#include "arq_dados.h"
#include "arvore_b_estrela.h"

#define MAX_NOME_ARQ 50
#define TAM_MAX_NOME 100
#define TAM_MAX_VALOR 100
#define campoIndexado "idCrime"

//registros de ações a serem executadas dependendo da funcionalidade
typedef struct acoes_arq_dados{
    FncAcaoRegSeq reg; //ação que atua sobre um registro encontrado a partir da busca sequencial
    FncAcaoFinal final; //Ação executada depois das ações principais
}FncAcoesArqDados;

typedef struct acoes_arvore{
    FncAcaoRegArv reg; //ação que atua sobre um registro encontrado a partir da busca na árvore B*
    FncAcaoBranch branch; //Ação que atua sobre um branch (isto é, um nó e todos os seus descendentes) da árvore B*
}FncAcoesArvore;

struct ArqDados{
    char nomeArqDados[MAX_NOME_ARQ];//guarda o nome do arquivo de dados
    FILE *arqDados;
    cabecalho_t *cabecalhoDados;
    FncAcoesArqDados *acoesArqDados;//Struct de ponteiros de função (ações executadas).
};

struct Arvore{
    char nomeArqArvore[MAX_NOME_ARQ];//guarda o nome do arquivo que contém a árvore.
    FILE *arqArvore;
    cabecalho_arvore_t *cabecalhoArvore;
    FncAcoesArvore *acoesArvore;//Struct de ponteiros de função (ações executadas).
    int M;//Ordem da árvore
    int tam_pagina;//Tamanho de uma página de disco
};

struct InfoDados{//Struct auxiliar que guarda dados sobre um registro
    int qtd_crit;
    char **nomes;
    char **vals_str;
    int *vals_int;
};

ArqDados_t *alocar_arq_dados(void){
    ArqDados_t *arq_dados_main;
    arq_dados_main = malloc(sizeof(ArqDados_t));

    arq_dados_main->cabecalhoDados = alocar_cabecalho_dados();
    arq_dados_main->acoesArqDados = malloc(sizeof(FncAcoesArqDados));

    return arq_dados_main;
}

Arvore_t *alocar_arvore(void){
    Arvore_t *arvore_retorno = malloc(sizeof(Arvore_t));
    arvore_retorno->cabecalhoArvore = alocar_cabecalho_arvore();
    arvore_retorno->M = get_ordem_arvore();
    arvore_retorno->tam_pagina = get_tam_pagina();
    arvore_retorno->acoesArvore = malloc(sizeof(FncAcoesArvore));

    return arvore_retorno;
}

void setNoOpsArqDados(ArqDados_t *arq_dados){//Configura todas as ações como NoOps.
    arq_dados->acoesArqDados->reg = NoOpAcaoRegSeq;
    arq_dados->acoesArqDados->final = NoOpAcaoFinal;
}

void setNoOpsArvore(Arvore_t *arvore){//Configura todas as ações como NoOps.
    arvore->acoesArvore->reg = NoOpAcaoRegArv;
    arvore->acoesArvore->branch = NoOpAcaoBranch;
}

void configuraInsercao(ArqDados_t *arq_dados, Arvore_t *arvore){
    //Função que configura as ações executadas pela árvore e pelo arquivo de dados quando
    //há uma inserção de um registro qualquer na árvore.

    //Configuro todas as ações como NoOps.
    setNoOpsArqDados(arq_dados);
    setNoOpsArvore(arvore);

    //Agora, sobrescrevo as ações utilizadas na inserção.
    arvore->acoesArvore->branch = insercao_branch;
}

void configuraBusca(ArqDados_t *arq_dados, Arvore_t *arvore){
    //Função que configura as ações executadas pela árvore e pelo arquivo de dados quando
    //há uma busca sequencial no arquivo de dados ou uma busca utilizando a árvore.

    //Configuro todas as ações como NoOps.
    setNoOpsArqDados(arq_dados);
    setNoOpsArvore(arvore);

    //Agora, sobrescrevo as ações utilizadas na busca.
    arq_dados->acoesArqDados->reg = printa_busca;
    arq_dados->acoesArqDados->final = achouReg;
    arvore->acoesArvore->reg = validaPrinta;
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
    //Função que abre o arquivo que contém a árvore, considerando que seu nome já foi lido.

    arvore->arqArvore = fopen(arvore->nomeArqArvore, tipo_leitura);
    if(arvore->arqArvore == NULL){
        mensagem_erro();
    }
}

void desalocar_ArqDados(ArqDados_t *arq_dados){
    free(arq_dados->cabecalhoDados);
    free(arq_dados->acoesArqDados);
    free(arq_dados);
}

void desalocar_Arvore(Arvore_t *arvore){
    free(arvore->cabecalhoArvore);
    free(arvore->acoesArvore);
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

InfoDados_t *ler_dados_registro(FncMetodoLeitura metodoLeitura, ArqDados_t *arq_dados, int *tam_reg){
    /*  Obtem informações de um InfoDados_t com dados 
    vindo do Stdin ou dados vindo de um arquivo de dados.
        Essa função erá retornar o Info_dados_t com os dados do registro ou 
    irá retornar NULL caso verifique que o registro lido é removido logicamente.
        Além disso a função modifica o argumento 'tam_reg' passando o tamanho do registro lido.
    */

    //Aloco o tipo que será retornado, de modo a conter espaço para informações de todos 
    //os campos de um registro (6 campos).
    InfoDados_t *info = alocar_InfoDados(6);

    //Faço a leitura utilizando a forma de leitura selecionada e armazeno esses dados em um registro.
    dados_t *reg = alocar_dados();
    *tam_reg = metodoLeitura(reg, arq_dados->arqDados);

    //Em seguida, passo os dados do registro para os vetores do InfoDados_t.
    regDados_para_vetores(reg, info->nomes, info->vals_int, info->vals_str);

    //Se o registro é removido, então a informação sobre esse registro é inválida,
    //portanto retorno o valor NULL para sinalizar isso.
    if(get_registro_removido(reg)){
        desalocar_registro(reg);
        return NULL;
    }

    desalocar_registro(reg);
    return info;
}

void mostrar_info_dados(InfoDados_t *criterios){
    for(int i=0; i<criterios->qtd_crit; ++i){
        printf("%s: str:%s | int:%d\n",
            criterios->nomes[i],
            criterios->vals_str[i],
            criterios->vals_int[i]
        );
    }
}

void desalocar_InfoDados(InfoDados_t *informacao){
    desalocar_vetor_string(informacao->nomes,informacao->qtd_crit);
    desalocar_vetor_string(informacao->vals_str,informacao->qtd_crit);
    free(informacao->vals_int);
    free(informacao);
}

int testarStatusArvore(Arvore_t *arvore){
	//Função que retorna 1 caso o arquivo esteja consistente e 0 caso esteja inconsistente
    if(getStatusArvore(arvore->cabecalhoArvore) == '1'){
        return 1;
    }
    return 0;
}

int testarStatusDados(ArqDados_t *arq_dados){
	//Função que retorna 1 caso o arquivo esteja consistente e 0 caso esteja inconsistente
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

void buscaArvoreRec(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios,
                    void *info_aux, int chave_busca, pagina_t *pgn_mae, int RRN_atual){
    /*Função recursiva que faz a busca de um valor de idCrime em uma árvore B* cujo campo indexado 
    é idCrime. Esse valor buscado é o 'int chave'. O parâmetro 'int RRN_atual' representa o RRN 
    do nó que está sendo verificado. Quando encontrado um nó dessa árvore que contém o idCrime 
    buscado, pode-se fazer uma ação sobre todos os nós que formaram o caminho de busca entre a raiz 
    e esse nó ou sobre o registro que possui esse idCrime.*/

    //critério de parada
    if(RRN_atual == -1){
        //Nó inexistente, ou seja, não há registro com o idCrime buscado

        //chamo a ação passando um byteOffset nulo
        arvore->acoesArvore->reg(arq_dados, criterios, -1);
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
        buscaArvoreRec(arq_dados, arvore, criterios, info_aux, chave_busca, pgn_atual, P);

        //Realizo a ação sobre o nó encontrado, na volta da recursão.
        arvore->acoesArvore->branch(arvore, pgn_mae, pgn_atual, info_aux);
    }else{
        //Se encontrou, realizo uma ação sobre esse registro
        arvore->acoesArvore->reg(arq_dados, criterios, Pr);
    }

    desaloca_pagina(pgn_atual);
}

void buscaArvore(ArqDados_t *arq_dados, Arvore_t *arvore, int pos_crit,
                                                        InfoDados_t *criterios, void *info_aux){

    //Função que realiza a busca por um registro que contém os critérios do argumento da função.
    //A função faz isso chamando a sua equivalente recursiva inicialmente com o nó raiz.

    int RRN_raiz = get_noRaiz(arvore->cabecalhoArvore);

    //chamo a função recursiva de busca na árvore B*, a partir do nó raiz
    buscaArvoreRec(arq_dados, arvore, criterios, info_aux,
    criterios->vals_int[pos_crit], NULL, RRN_raiz);
}

void buscaSeqDados(ArqDados_t *arq_dados, Arvore_t *arvore,InfoDados_t *criterios){
    //função que faz busca sequencial no arquivo de dados

    int achei_reg_val = 0;
    //Flag que indica se algum registro satisafaz todos os critérios de busca.
    //Recebe -1 se nenhum satisfaz ou 1 se pelo menos 1 satisfaz

    dados_t *registro = alocar_dados();//aloco um memoria para leitura de um registro de dados
    
    //Consegui_ler armazena -1 se não conseguiu ler
    // e armazena o tamanho do registro caso consiga ler
    int consegui_ler = ler_bin_registro(registro, arq_dados->arqDados);

    int cont = 0;
    while(consegui_ler>0){
        //Se consegui ler, avalio os criterios de busca

        if(testarCriterios(registro,criterios->nomes,criterios->vals_str,criterios->vals_int,criterios->qtd_crit)){
            ++cont;
            //se o registro satisfaz todos os criterios, realizo a ação 
            achei_reg_val = 1;//achei pelo menos 1 registro que satisfaz os critérios

            arq_dados->acoesArqDados->reg(registro);
        }
        //sempre desaloco o registro, pois preciso 
        //desalocar os campos de tamanho váriavel do registro
        desalocar_registro(registro);
        //Em seguida aloco outro registro, para fazer a leitura
        registro = alocar_dados();
        consegui_ler = ler_bin_registro(registro, arq_dados->arqDados);
    }
    //se não conseguiu ler, os campos de tamanho variavel nao foram alocados.
    // Assim, desaloco apenas o ponteiro para o tipo dados_t

    free(registro);
    
    arq_dados->acoesArqDados->final(achei_reg_val);
}

void processaRegistros(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios, void *info_aux){
    /*Funcao que define se a busca sera binaria na arvore B* ou sequencial no arquivo de dados
    e encontra os registros. Depois, usa as ações da árvore ou do arquivo de dados para processá-los.*/

    int posicao_criterio = busca_por_indexado(criterios);
    
    if(posicao_criterio >= 0 && get_noRaiz(arvore->cabecalhoArvore) != -1){
        /*se a árvore B* não é vazia e indexa algum dos critérios
         de busca, então faço a busca usando-a.*/
        buscaArvore(arq_dados,arvore,posicao_criterio,criterios,info_aux);
    }else{
        //se não, faz-se busca sequencial no arquivo de dados
    
        /*Para isso, deve-se reiniciar o ponteiro do arquivo de dados para o primeiro registro
         de dados (pulando o cabecalho) para fazer um novo processamento, pois não há garantia 
         de que o ponteiro esteja corretamente posicionado*/
        fseek(arq_dados->arqDados,len_cabecalho_dados(),SEEK_SET);
        //em seguida, chama-se a função que realiza a busca sequencial
        buscaSeqDados(arq_dados, arvore, criterios);
    }
}

void insercao_arqDados(ArqDados_t *arq_dados, InfoDados_t *info_inserir){
    //Realiza a inserção de um registro (cujuas informações estão no info_inserior) no arquivo de dados
    
    //Insiro o registro no arquivo de dados. Para isso:
    //1-Devo criar o registro que irei inserir, a partir dos critérios de busca.
    dados_t *reg_inserir = alocar_dados();
    //2- Passo o dados de info_inserir para o registro
    vetores_para_regDados(
        reg_inserir,
        info_inserir->nomes,
        info_inserir->vals_str,
        info_inserir->vals_int,
        info_inserir->qtd_crit
    );

    //3-Preparar o registro para escrita
    prepara_para_escrita(reg_inserir);
    //4-Escrever o registro
    escrever_bin_registro_dados(reg_inserir,arq_dados->arqDados,arq_dados->cabecalhoDados);
    //5-incremento o nroRegArq
    cabecalho_nroRegArq_incrementar(arq_dados->cabecalhoDados, 1);

    desalocar_registro(reg_inserir);
}

void cria_raiz(ArqDados_t *arq_dados, Arvore_t *arvore, long int byteOffSet, InfoDados_t *info_dados_inserir){
    InfoInserida_t *dado_inserir_arvore = alocar_InfoInserida();
    dado_inserir_arvore = criar_InfoInserida(arq_dados, info_dados_inserir, byteOffSet);

    //configuro a nova altura da árvore
    set_nroNiveis(arvore->cabecalhoArvore, 1);

    //configuro o novo nó raiz,
    pagina_t *pgn_raiz = aloca_pagina();
    pgn_raiz->RRN_no = 0;
    //o qual possui o nível mais alto
    set_nivel_no(pgn_raiz->no, get_nroNiveis(arvore->cabecalhoArvore));
    //e inicialmente não possui nenhuma chave.
    set_nChaves(pgn_raiz->no, 0);

    //atualizo o cabecalho
    set_noRaiz(arvore->cabecalhoArvore,0);
    set_RRNproxNo(arvore->cabecalhoArvore, get_RRNproxNo(arvore->cabecalhoArvore)+1);
    set_nroChaves(arvore->cabecalhoArvore, 1);

    //escrevo as informações do nó
    insere_ordenado_no(arvore->arqArvore, pgn_raiz, dado_inserir_arvore);

    desaloca_pagina(pgn_raiz);

    desalocar_InfoInserida(dado_inserir_arvore);
}

void insercao_arvore(ArqDados_t *arq_dados, Arvore_t *arvore, long int byteOffSet, InfoDados_t *info_dados_inserir){
    //Realiza a inserção em uma árvore por meio da busca do processaRegistros().
    //Para isso, devo:

    //-Criar a informação que será inserida
    InfoInserida_t *info_arvore_inserir = alocar_InfoInserida();
    info_arvore_inserir = criar_InfoInserida(arq_dados, info_dados_inserir, byteOffSet);
    //-Chamar o processaRegistros com a info_aux sendo a informação que será inserida
    processaRegistros(arq_dados, arvore, info_dados_inserir, info_arvore_inserir);
    desalocar_InfoInserida(info_arvore_inserir);
}

void buscaRegistro(ArqDados_t *arq_dados, Arvore_t *arvore, InfoDados_t *criterios){
    //Realiza a busca por um registro utilizando a busca do processaRegistros()
    //Como não é necessário uma info_aux, passo NULL.
    processaRegistros(arq_dados,arvore,criterios, NULL);
}

/*---------------------------------------AÇÕES-------------------------------------------*/
void insercao_branch(Arvore_t *arvore, pagina_t *pgn_mae, pagina_t *pgn_atual, void *info_aux){
    //Nessa ação, a info_aux será a informação inserida. Logo, devo fazer o casting.
    InfoInserida_t *info_inserir = (InfoInserida_t *) info_aux;
    
    //Se não é uma informação de inserção válida, só dou return.
    if(info_inserir->valida != 1){
        return;
    }

    //Se a nova chave cabe na página atual
    if(get_nChaves(pgn_atual->no) < get_ordem_arvore()-1){

        insere_ordenado_no(arvore->arqArvore, pgn_atual, info_inserir);
        
    }else{//Se a nova chave não cabe na página atual
        if(get_nivel_no(pgn_atual->no) == get_nroNiveis(arvore->cabecalhoArvore)){//Se estou inserindo no nó raiz
            //Faço o split_1_para_2
            split_1_para_2(arvore->arqArvore,arvore->cabecalhoArvore,pgn_atual,info_inserir);
        }else{//Se estou inserindo em qualquer nó que não no raiz

            //tento a redistribuição
            pagina_t *pgn_irma = NULL;
            result_redistribuicao_t resultado;
            resultado = redistribuicao(
                arvore->arqArvore,
                pgn_mae,
                pgn_atual,
                &pgn_irma,
                info_inserir
            );

            if(resultado == redistribuiu){//Se conseguiu redistribuir

                //Então devo indicar para os nós acima que a informação de inserção
                // é inválida, pois já foi inserida nesse nó.
                info_inserir->valida = -1;

            }else{//Se não conseguiu redistribuir:

                if(resultado == retorna_esq){
                    //Ou seja, o ponteiro pgn_irma aponta para página irmã à esquerda
                    split_2_para_3(
                        arvore->arqArvore,
                        arvore->cabecalhoArvore,
                        pgn_mae, pgn_irma, pgn_atual,
                        info_inserir
                    );

                }else{
                    //Ou seja (resultado == retorna_dir),
                    // o ponteiro pgn_irma aponta para página irmã à direita
                    split_2_para_3(
                        arvore->arqArvore,
                        arvore->cabecalhoArvore,
                        pgn_mae, pgn_atual, pgn_irma,
                        info_inserir
                    );

                }
            }
            desaloca_pagina(pgn_irma);
        }
    }

    if(ehFolha(pgn_atual)){
        /*se pagina em que se insere é uma folha, entao a chave a ser inserida é nova, e não 
        o resultado de uma promoção. Logo, devo incrementar o numero de chaves */
        set_nroChaves(arvore->cabecalhoArvore, get_nroChaves(arvore->cabecalhoArvore)+1);
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
    //função do tipo FncAcaoNBranch que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoFinal(int ignorar){
    //função do tipo FncAcaoFinal que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoRegArv(ArqDados_t *ignorar1, InfoDados_t *ignorar2, long int ignorar3){
    //função do tipo FncAcaoRegArv que não faz nada, para os casos em que funções desse tipo não serão usadas
}

void NoOpAcaoRegSeq(dados_t *ignorar1){
    //função do tipo FncAcaoRegSeq que não faz nada, para os casos em que funções desse tipo não serão usadas
}

/*---------------------------------------DEMAIS FUNÇÕES-------------------------------------------*/
long int getProxByteOffSet(ArqDados_t *arq_dados){ 
    //Função que permite que o 'funcionalidades.c' acesse uma função do 'arq_dados.c'.
    return get_proxByteOffset(arq_dados->cabecalhoDados);
}

InfoInserida_t *criar_InfoInserida(ArqDados_t *arq_dados, InfoDados_t *info_dados, long int byteOffset){
    //Cria uma informação que será inserida na árvore a partir
    // dos dados contidos no InfoDados_t e no byteOffSet passados como argumento.

    InfoInserida_t *inserida_retorno = alocar_InfoInserida();
    inserida_retorno->valida = 1;//Configuro como uma informação válida
    for(int i = 0; i < info_dados->qtd_crit; ++i){//Laço que acha o idCrime e escreve-o na chave_t.
        if(strcmp(info_dados->nomes[i], campoIndexado)==0){
            set_chaves_C(inserida_retorno->chave, info_dados->vals_int[i]);
            set_chaves_Pr(inserida_retorno->chave, byteOffset);
            break;
        }
    }
    *(inserida_retorno->ponteiro) = -1;

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

void escreverCabecalhoArvore(Arvore_t *arvore){//Escreve o cabeçalho da árvore
    fluxo_CabecalhoArvore(arvore->arqArvore, arvore->cabecalhoArvore, meu_fwrite);
}

void setCabecalhoArvoreNulo(Arvore_t *arvore){
    //Configura um cabeçalho como nulo - isto é, com valores respectivos a uma árvore vazia.
    set_noRaiz(arvore->cabecalhoArvore, -1);
    set_RRNproxNo(arvore->cabecalhoArvore, 0);
    set_nroNiveis(arvore->cabecalhoArvore, 0);
    set_nroChaves(arvore->cabecalhoArvore, 0);
    set_lixoCabecalho(arvore->cabecalhoArvore);
}

int validaInfoDados(InfoDados_t *info){
    //Testa se as informações são válidas. Se sim retorna 1, se não retorna 0.
    if(info == NULL){
        return 0;
    }else{
        return 1;
    }
}

int lerRegDoArqDados(dados_t *reg, FILE *arq){
    /*'funcionlidades.c' não pode ter acesso direto às funções do 'arq_dados.c'. 
    Como 'ler_bin_registro()' é do 'arq_dados.c', faz-se necessário uma máscara para que o 
    'funcionalidades.c' realize as ações de leitura.*/
    return ler_bin_registro(reg, arq);
}

int lerRegDoStdin(dados_t *reg, FILE *arq){
    /*'funcionlidades.c' não pode ter acesso direto às funções do 'arq_dados.c'. 
    Como 'leRegStdin()' é do 'arq_dados.c', faz-se necessário uma máscara para que o 
    'funcionalidades.c' realize as ações de leitura.*/
    return leRegStdin(reg);
}