#include <stdio.h>
#include <stdlib.h>
#include "auxiliares.h"
#include "arvore_b_estrela.h"
#include "auxiliares.h"

#define M 5
#define TAM_PAGINA 76

struct cabecalho_arvore{
    char status;
    int noRaiz;
    int RRNproxNo;
    int nroNiveis;
    int nroChaves;
    char lixo[59];
};

struct chave{
    int C; //Conteúdo do campo indexado
    long long Pr; //ByteOffSet do registro no arquivo de dados
};

struct no_arvore{
    int nivel;
    int n;
    int P[M];
    chave_t chaves[M-1];
};

typedef struct temp_no{
    int n;//quantidade de chaves no nó
    chave_t *chaves;
    int *ponteiros;
}temp_no_t;

cabecalho_arvore_t *alocar_cabecalho_arvore(void){
    cabecalho_arvore_t *cabecalho_retorno = malloc(sizeof(cabecalho_arvore_t));
    return cabecalho_retorno;
}

no_arvore_t *alocar_no(void){
    return malloc(sizeof(no_arvore_t));

}
void desalocar_no(no_arvore_t *no){
    free(no);
}

pagina_t *aloca_pagina(void){
    pagina_t *pagina = malloc(sizeof(pagina_t));
    pagina->no = alocar_no();
    return pagina;
}

void desaloca_pagina(pagina_t *pagina){
    desalocar_no(pagina->no);
    free(pagina);
}

chave_t *aloca_vet_chaves(int qtd_chaves){
    return malloc(sizeof(chave_t)*qtd_chaves);
}

void desaloca_vet_chaves(chave_t *vet_chaves){
    free(vet_chaves);
}

int *aloca_vet_ponteiros(int qtd_ponteiros){
    return malloc(sizeof(int)*qtd_ponteiros);
}

void desaloca_vet_ponteiros(int *vet_ponteiros){
    free(vet_ponteiros);
}

temp_no_t *aloca_temp_no(int n){
    temp_no_t *no_retorno = malloc(sizeof(temp_no_t));
    no_retorno->n = n;//número de chaves no nó
    no_retorno->chaves = aloca_vet_chaves(n);
    no_retorno->ponteiros = aloca_vet_ponteiros(n+2);
}

void desaloca_temp_no(temp_no_t *no){
    desaloca_vet_chaves(no->chaves);
    desaloca_vet_ponteiros(no->ponteiros);
    free(no);
}
chave_t *alocar_chave(){
    chave_t *chave = malloc(sizeof(chave_t));
    return chave;
}

void desalocar_chave(chave_t *chave){
    free(chave);
}


void mostra_cabecalho_arvore(cabecalho_arvore_t *cabecalho){
    printf("Cabeçalho da arvore: \n");
    printf("status=%c|noRaiz=%d|RRNproxNo=%d|nroNiveis=%d|nroChaves=%d\n"
        , cabecalho->status
        , cabecalho->noRaiz
        , cabecalho->RRNproxNo
        , cabecalho->nroNiveis
        , cabecalho->nroChaves
    );
}

void mostra_ponteiro(no_arvore_t *no, int pos_vet_ponteiros){
    printf("P%d=%d|", pos_vet_ponteiros, no->P[pos_vet_ponteiros]);
}

void mostra_chave(no_arvore_t *no, int pos_vet_chaves){
    printf("C%d:%d|Pr%d:%lld", pos_vet_chaves, no->chaves[pos_vet_chaves].C, pos_vet_chaves, no->chaves[pos_vet_chaves].Pr);
}

void mostrar_no(no_arvore_t *no){
    mostra_ponteiro(no, 0);

    for(int i = 0; i < no->n;++i){
        mostra_chave(no, i);
        mostra_ponteiro(no, i+1);
    }
    printf("\n");
}

char getStatusArvore(cabecalho_arvore_t *cabecalho){
    return cabecalho->status;
}

void setStatusArvore(cabecalho_arvore_t *cabecalho, char status){
    cabecalho->status = status;
}

int get_ordem_arvore(){
    return M;
}

int get_tam_pagina(){
    return TAM_PAGINA;
}

int get_noRaiz(cabecalho_arvore_t *cabecalho){
    return cabecalho->noRaiz;
}

void set_noRaiz(cabecalho_arvore_t *cabecalho, int nova_raiz){
    cabecalho->noRaiz = nova_raiz;
}

int get_nChaves(no_arvore_t *no){
    return no->n;
}

void set_nChaves(no_arvore_t *no, int nova_nChaves){
    no->n = nova_nChaves;
}

int get_nroNiveis(cabecalho_arvore_t *cabecalho){
    return cabecalho->nroNiveis;
}

void set_nroNiveis(cabecalho_arvore_t *cabecalho, int nova_nroNiveis){
    cabecalho->nroNiveis = nova_nroNiveis;
}

void fluxo_StatusArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo){
    funcFluxo(&cabecalho->status,sizeof(char),1,arqArvore);
}

void fluxo_CabecalhoArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo){
    //Função que faz escreve ou lê o cabecalho do arquivo da árvore B*, a dependeder do 'funcFluxo'
    funcFluxo(&(cabecalho->status), sizeof(char), 1, arqArvore);
    funcFluxo(&(cabecalho->noRaiz), sizeof(int), 1, arqArvore);
    funcFluxo(&(cabecalho->RRNproxNo), sizeof(int), 1, arqArvore);    
    funcFluxo(&(cabecalho->nroNiveis), sizeof(int), 1, arqArvore);    
    funcFluxo(&(cabecalho->nroChaves), sizeof(int), 1, arqArvore);    
    char lixo = '$';
    for(int i = 0; i < 59; ++i){
        funcFluxo(&lixo, sizeof(char), 1, arqArvore);
    }
}

void fluxo_ponteiro_no(FILE *arqArvore, no_arvore_t *no, int pos_vet_ponteiros, FncFluxoMemSec funcFluxo){
    funcFluxo(&(no->P[pos_vet_ponteiros]), sizeof(int), 1, arqArvore);
}

void fluxo_chave_no(FILE *arqArvore, no_arvore_t *no, int pos_vet_chaves, FncFluxoMemSec funcFluxo){
    funcFluxo(&((no->chaves[pos_vet_chaves]).C), sizeof(int), 1, arqArvore);
    funcFluxo(&((no->chaves[pos_vet_chaves]).Pr), sizeof(long long), 1, arqArvore);
}

void fluxo_no(FILE *arqArvore, no_arvore_t *no_operar, FncFluxoMemSec funcFluxo){
    //escrever ou ler o nível e o 'n'
    funcFluxo(&(no_operar->nivel), sizeof(int), 1, arqArvore);
    funcFluxo(&(no_operar->n), sizeof(int), 1, arqArvore);

    //escrever ou ler o primeiro ponteiro do no
    fluxo_ponteiro_no(arqArvore, no_operar, 0, funcFluxo);      
    //escrever ou ler os demais conjuntos de ponteiro e chave
    for(int i = 0; i < M - 1; ++i){
        fluxo_chave_no(arqArvore, no_operar, i, funcFluxo);
        fluxo_ponteiro_no(arqArvore, no_operar, i+1, funcFluxo);
    }
}

long long int buscaBinNo(no_arvore_t *no_atual, int ini, int fim, int chave, int *P){
    //busca binária recursiva para nós da árvore B*

    /*O ponteiro P (int *P) é o valor do RRN do próximo nó para o qual a função de busca
    deve ir, caso a chave buscada não seja encontrada no nó atual (no_arvore_t *no_atual). 
    Como não é possível retornar dois valores em C, esse valor é passado por referência.*/

    /*O retorno da função é o byteOffset, no arquivo de dados, do registro que satisfaz 
    o critério de busca.*/


    if(ini > fim){//critério de parada
        //Se o valor não existe, retorno um byteOffset inválido.
        return -1;
    }

    int meio = (ini+fim)/2;
    // printf("ini:%d|mei:%d|fim:%d\n", ini, meio, fim);

    if((no_atual->chaves[meio]).C == chave){
        //Se achei:
        //-Coloco um valor nulo no ponteiro para outro nó da árvore, pois não é necessário usá-lo
        *P = -1;
        //-Retorno o byteOffset do registro no arquivo de dados
        return (no_atual->chaves[meio]).Pr;
    }else if(chave < (no_atual->chaves[meio]).C){
        //Se o valor que estou buscando é menor que o atual:
        //-Preparo a busca recursiva até o atual-1
        fim = meio-1;
        //-Atribuo, ao ponteiro P, o ponteiro à esquerda do valor atual 
        *P = no_atual->P[meio];
        // printf("*P esquerda:%d\n", *P);
        //-Chamo a função recursivamente com os valores atualizados
        return buscaBinNo(no_atual, ini, fim, chave, P);
    }else{
        //Se o valor que estou buscando é maior que o atual:
        //-Preparo a busca recursiva a partir do atual+1
        ini = meio+1;
        //-Atribuo, ao ponteiro P, o ponteiro à direita do valor atual 
        *P = no_atual->P[meio+1];
        // printf("*P direita:%d\n", *P);
        //-Chamo a função recursivamente com os valores atualizados
        return buscaBinNo(no_atual, ini, fim, chave, P);
    }
}

int get_nivel_no(no_arvore_t *no){
    return no->nivel;
}

void set_nivel_no(no_arvore_t *no, int nivel){
    no->nivel = nivel;
}

int compara_ponteiros(void *vet_ponteiros, int pos_comparar, void *chave_comparar){
    int *vet_ponteiros_real = (int*)vet_ponteiros;
    int chave_pos = vet_ponteiros_real[pos_comparar];
    int chave_compara_real = *((int*)chave_comparar);
    return chave_pos - chave_compara_real;
}

int compara_chaves(chave_t *chave_a, chave_t *chave_b){
    return chave_a->C - chave_b->C;
}

void copia_vet_chaves(chave_t *origem, chave_t *destino, int ini_origem, int fim_origem, int ini_destino, int fim_destino){
    int cont_origem = ini_origem;
    int cont_destino = ini_destino;
    while(cont_origem <= fim_origem){
        insere_chave_em_vet_chaves(destino, &(origem[cont_origem]), cont_destino);
        ++cont_destino;++cont_origem;        
    }    
}

void copia_vet_ponteiros(int *origem, int *destino, int ini_origem, int fim_origem, int ini_destino, int fim_destino){
    int cont_origem = ini_origem;
    int cont_destino = ini_destino;
    while(cont_origem <= fim_origem){
        destino[cont_destino] = origem[cont_origem];
        ++cont_destino;++cont_origem;
    }    
}

void insere_chave_em_vet_chaves(chave_t *vet_chaves, chave_t *chave_inserir, int pos_inserir){
    (vet_chaves[pos_inserir]).C = chave_inserir->C;
    (vet_chaves[pos_inserir]).Pr = chave_inserir->Pr;
}

int insere_ponteiro_ordenado(int *vet_ponteiros, int tam_vet_ponteiros,int ponteiro_inserir){
    int fez_shifitada = -1;//flag que indica se houve shifitada
    for(int i = 0; i < tam_vet_ponteiros; ++i){
        if(ponteiro_inserir < vet_ponteiros[i]){
            //Se a o ponteiro que vou inserir for menor que a 'i', então vou inseri-lo
            //na posição i e shifitar todos os demais para a posição à direita no vetor
            fez_shifitada = 1; //altero a flag
            for(int j = tam_vet_ponteiros; j > i; --j){//Começo pelo último ponteiro e vou até o ponteiro adjacente à direita de 'i'
                // insiro o ponteiro da posição j-1 na posição j
                vet_ponteiros[j] = vet_ponteiros[j-1];
            }
            //insere ponteiro em sua posição (i)
            vet_ponteiros[i] = ponteiro_inserir;
            return i;
        }
    }
    if(fez_shifitada == -1){//Se não fez shifitada, então o ponteiro que vou inserir é o maior do vetor
        vet_ponteiros[tam_vet_ponteiros] = ponteiro_inserir;//então insiro na ultima posição
        return tam_vet_ponteiros;
    }
}

int insere_ordenado_vet_chaves(chave_t *vet_chaves, chave_t *chave_inserir, int tam_vet_chaves){//[0, 3, 4] <- 2 => [0, 2, 3, 4]
    //Insere  uma chave (em um vetor de chaves) em sua posição ideal, de modo a manter a ordem.
    //Retorna a posição em que a chave foi inserida
    if(tam_vet_chaves == 0){//vetor vazio, insiro na primeira posição
        insere_chave_em_vet_chaves(vet_chaves, chave_inserir, 0);
        return 0;
    }else{
        int fez_shifitada = -1;//flag que indica se houve shifitada
        for(int i = 0; i < tam_vet_chaves; ++i){
            if(compara_chaves(chave_inserir, &(vet_chaves[i])) < 0){
                //Se a chave que vou inserir for menor que a 'i', então vou inseri-la
                //na posição i e shifitar todas as demais para a posição à direita no nó
                fez_shifitada = 1; //altero a flag
                for(int j = tam_vet_chaves; j > i; --j){//Começo pela última chave e vou até a chave adjacente à direita de 'i'
                    // insiro a chave da posição j-1 na posição j
                    insere_chave_em_vet_chaves(vet_chaves, &(vet_chaves[j-1]), j);
                }
                //insere chave em sua posição (i)
                insere_chave_em_vet_chaves(vet_chaves, chave_inserir, i);
                return i;
            }
        }
        if(fez_shifitada == -1){//Se não fez shifitada, então a chave que vou inserir é a maior chave do nó
            insere_chave_em_vet_chaves(vet_chaves, chave_inserir, tam_vet_chaves);//então insiro ela na ultima posição
            return tam_vet_chaves;
        }
    }
}

void insere_ordenado_no(no_arvore_t *no_inserir, chave_t *chave_inserir){
    //Insere uma chave (em um nó) em sua posição ideal, de modo a manter a ordem. 
    insere_ordenado_vet_chaves(no_inserir->chaves, chave_inserir, no_inserir->n);
}

int retorna_irma_esq(int pos_vet_P, no_arvore_t *no){
    if(pos_vet_P == 0){//significa que não há uma irmã à esquerda
        return -1;
    }

    return no->P[pos_vet_P-1];
}

int retorna_irma_dir(int pos_vet_P, no_arvore_t *no){
    if(pos_vet_P == no->n){//significa que não há uma irmã à direita
        return -1;
    }

    return no->P[pos_vet_P+1];
}

int get_RRN_irma(no_arvore_t *no_mae, int RRN_filho, FncGetRRNirma retorna_irma){
    for(int pos_vet_P = 0; pos_vet_P < no_mae->n + 1; ++pos_vet_P){//itera o vetor de ponteiros
        if(no_mae->P[pos_vet_P] == RRN_filho){
            return retorna_irma(pos_vet_P, no_mae);
        }
    }
}

no_arvore_t *get_pagina_irma(FILE *arqArvore,pagina_t *pgn_mae, pagina_t *pgn_atual, FncGetRRNirma retorna_irma){   
    //Retorna o ponteiro para a página lida caso consiga ler a página irmã.
    //Retorna NULL caso não consiga ler a página irmã.

    //Obter o RRN da página irmã
    int RRN_irma = get_RRN_irma(pgn_mae->no, pgn_atual->RRN_no, retorna_irma);
    if(RRN_irma == -1){//Não tem irmã
        return NULL;
    }

    pagina_t *pagina_irma = aloca_pagina();//aloco a página
    pagina_irma->RRN_no = RRN_irma;//configuro o RRN
    no_arvore_t *no_irma = alocar_no();//aloco o nó

    fseek(arqArvore, (RRN_irma+1) * TAM_PAGINA, SEEK_SET);//posiciono o cursor para leitura do nó
    fluxo_no(arqArvore, pagina_irma->no, meu_fread);//leio o nó

    return pagina_irma;//retorno o nó   
}

int get_pos_chave_mae(pagina_t *pgn_mae, pagina_t *pgn_esq){
    return busca_bin_rec(pgn_mae->no->P, 0, pgn_mae->no->n, &(pgn_esq->RRN_no), compara_ponteiros);
}

void insere_ponteiro_em_vet(int *vet_ponteiros, int tam_vet_ponteiros, int pos, int ponteiro){
    for(int i = tam_vet_ponteiros-1; i > pos; --i){
        vet_ponteiros[i] = vet_ponteiros[i-1]; //Faço uma shifitada de todos os ponteiros à direita da posição 'pos'
    }
    vet_ponteiros[pos] = ponteiro;//depois insiro esse ponteiro em sua posição;
}

void redistribui_paginas(FILE *arqArvore, pagina_t *pgn_mae, pagina_t *pgn_esq, pagina_t *pgn_dir, InfoInserida_t *info){

    //Crio um vetor temporário de ponteiros e de chaves
    int tam_ponteiros_vet_temp = pgn_esq->no->n + pgn_dir->no->n + 3;//esq + dir + ponteiro_inserido
    int tam_chaves_vet_temp = pgn_esq->no->n + pgn_dir->no->n + 2;//esq + dir + chave_inserida + chave_mae

    int ponteiros_vet_temp[tam_ponteiros_vet_temp];
    chave_t chaves_vet_temp[tam_chaves_vet_temp];

    //Obtenho a posição da chave mãe.
    int pos_chave_mae = get_pos_chave_mae(pgn_mae, pgn_esq);

    //Coloco as chaves em seu vetor temporário, de modo a guardar a posição em que insiro a chave_promovida. Para isso, devo:
    int pos_inserir_ponteiro;
    {
        //1-Colocar as chaves da página à esquerda
        int i;
        for(i = 0; i < pgn_esq->no->n; ++i){
            insere_ordenado_vet_chaves(chaves_vet_temp, &(pgn_esq->no->chaves[i]), i+1);
        }

        //2-Coloar as chaves da página à direita
        for(int j = 0; j < pgn_dir->no->n; ++j, ++i){
            insere_ordenado_vet_chaves(chaves_vet_temp, &(pgn_dir->no->chaves[j]), i+1);
        }

        //3-Colocar a chave_mae.
        insere_ordenado_vet_chaves(chaves_vet_temp, &(pgn_mae->no->chaves[pos_chave_mae]), tam_chaves_vet_temp-1);

        //4-Colocar a chave_promovida e guardar sua posição de inserção.
        pos_inserir_ponteiro = insere_ordenado_vet_chaves(chaves_vet_temp, info->chave_inserida, tam_chaves_vet_temp);
    }

    //Agora devo inserir os ponteiros. Para isso, devo:
    {
        //1-inserir os ponteiros da página à esquerda
        int i;
        for(i = 0; i < pgn_esq->no->n + 1; ++i){
            ponteiros_vet_temp[i] = pgn_esq->no->P[i];
        }

        //2-inserir os ponteiros da página à direita
        for(int j = 0; j < pgn_esq->no->n + 1; ++j, ++i){
            ponteiros_vet_temp[i] = pgn_dir->no->P[j];
        }

        //3-Por fim, inserir o ponteiro promovido, de movo a deslocar todos os demais ponteiros à direita em uma posição
        insere_ponteiro_em_vet(ponteiros_vet_temp, tam_ponteiros_vet_temp, pos_inserir_ponteiro+1, *(info->ponteiro_inserido));

    }

    //Com os os ponteiros e chaves em ordem, sobrescrevo a informações dos nós. Para isso:

    //1-Obtenho a posição da chave_mae
    int meio = (tam_chaves_vet_temp)/2;

    //2-Sobrescrevo a chave_mae com a chave encontrada no meio do vetor de chaves
    insere_chave_em_vet_chaves(pgn_mae->no->chaves, &(chaves_vet_temp[meio]), meio);

    //3-Sobrescrevo as chaves da página à esquerda
    copia_vet_chaves(chaves_vet_temp, pgn_esq->no->chaves, 0, meio-1, 0, meio-1);

    //4-Sobrescrevo as chaves da página à direita
    copia_vet_chaves(chaves_vet_temp, pgn_dir->no->chaves, meio+1, tam_chaves_vet_temp-1, 0, tam_chaves_vet_temp-meio-2);

    //5-Sobrescrevo os ponteiros da página à esquerda
    copia_vet_ponteiros(ponteiros_vet_temp, pgn_esq->no->P, 0, meio, 0, meio);

    //6-Sobrescrevo os ponteiros da página à direita
    copia_vet_ponteiros(ponteiros_vet_temp, pgn_dir->no->P, meio, tam_ponteiros_vet_temp-1, 0, tam_ponteiros_vet_temp-meio-3);  

    //7-Modifico a informação acerca da ocupação das páginas
    pgn_esq->no->n = meio;
    pgn_dir->no->n = tam_chaves_vet_temp-meio-1;

    //Escrever os nós que foram modificados
    fseek(arqArvore, (pgn_mae->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_mae->no, fwrite);
    fseek(arqArvore, (pgn_esq->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_esq->no, fwrite);
    fseek(arqArvore, (pgn_dir->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_dir->no, fwrite);
}

result_redistribuicao_t escolheIrma(pagina_t *pgn_irma, pagina_t *pgn_irma_esq, pagina_t *pgn_irma_dir){
    if(pgn_irma_dir != NULL){
        pgn_irma = pgn_irma_dir;
        return retorna_dir;
    }else{
        pgn_irma = pgn_irma_esq;
        return retorna_esq;
    }
}

result_redistribuicao_t redistribuicao(FILE *arqArvore, pagina_t *pgn_mae, pagina_t *pgn_atual, pagina_t *pgn_irma, InfoInserida_t *info){

    //Primeiramente, encontrar a página irmã à esquerda.
    pagina_t *pgn_irma_esq = get_pagina_irma(arqArvore,pgn_mae, pgn_atual, retorna_irma_esq);
    if(pgn_irma_esq != NULL && (pgn_irma_esq->no->n < M-1)){//Se consegui obter a página irmã à esquerda e ela não está cheia
        //então faço a redistribuição com ela.
        redistribui_paginas(arqArvore, pgn_mae, pgn_irma_esq, pgn_atual, info);

        //retorno que a redistribuição foi concluída.
        return redistribuiu;
    }
    
    //Depois, tento com a página da direita    
    pagina_t *pgn_irma_dir = get_pagina_irma(arqArvore,pgn_mae, pgn_atual, retorna_irma_dir);
    if(pgn_irma_dir != NULL && (pgn_irma_dir->no->n < M-1)){//Se consegui obter a página irmã à direita e ela não está cheia
        //então faço a redistribuição com ela.
        redistribui_paginas(arqArvore, pgn_mae, pgn_atual, pgn_irma_dir, info);

        //retorno que a redistribuição foi concluída.
        return redistribuiu;
    }
    
    /*Dado que não foi feita a redistribuição, sabe-se que deve ser feito o split 2 para 3.  
    Então, devo escolher uma das irmãs para que o split ocorra*/

    return escolheIrma(pgn_irma, pgn_irma_esq, pgn_irma_dir);
}

void split_1_para_2(FILE *arqArvore, cabecalho_arvore_t *cabecalho, pagina_t *pgn_mae, pagina_t *pgn_atual, chave_t *chave_inserir, int *ponteiro_promovido){
    
}

    /*
    INSERCAO(RRN_atual, RRN_anterior, *chave_inserir, int *ponteiro_promovido):
    Se é árvore vazia:
        cria raiz
        insere_ordenado(RRN_atual, chave_inserir, -1)
    Se não:
        Se cabe no nó:
            insere_ordenado(RRN_atual, chave_inserir, ponteiro_promovido):
        Se não:
            se é raiz:

                split_1_pra_2(RRN_atual, chave_inserir, ponteiro_promovido)
            se não:
                redistribuiu = redistribuir()
                se não redistribuiu:
                    split_2_pra_3




    SPLIT_1_2(RRN_atual, chave_inserir, ponteiro_promovido): MUDAR: passar a página já lida no lugar do RRN
        vet_temp = cria_vet_temp_chaves(no_t no_atual, chave);MUDAR: criar um no gigante
        cria_vet_temp_ponteiros(no_t no_atual, ponteiro_promovido);
        chave_meio = achar_chave_meio(vet_temp);
        pagina_dir = cria_pagina_direita();
        set_pagina(pagina_atual, 0, chave_meio)
        set_pagina(pagina_dir, chave_meio+1, M)
        volta_uma_pagina()
        fwrite_pagina(pagina_atual)
        vai_pro_fim()
        fwrite_pagina(pagina_dir)
        pagina_mae = criar_pagina()
        pagina_mae.insere(chave_meio)
        pagina_mae.insere_ponteiro(RRN_atual)
        pagina_mae.insere_ponteiro(RRN_pagdir)
        fwrite_pagina(pagina_mae)

        // ...
        // Pagia Atual
        // ...
        // pagina_dir
        // pagina_mae

        header.noRaiz = RRN_pagina_mae


    SPLIT_2_3(no_esq, no_dir, no_mae, chave_inserir, ponterio_promovido):
        
        vet_temp = cria_vet_temp_chaves(no_esq, no_dir, chave_inserir)
        cria_vet_temp_ponteiros(no_esq, no_dir, no_mae, ponteiro_promovido)
    */
