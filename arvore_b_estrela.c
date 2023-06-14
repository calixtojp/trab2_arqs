#include <stdio.h>
#include <stdlib.h>
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

int compara_chaves(chave_t *chave_a, chave_t *chave_b){
    return chave_a->C - chave_b->C;
}

void insere_chave_em_vet_chaves(chave_t *vet_chaves, chave_t *chave_inserir, int pos_inserir){
    (vet_chaves[pos_inserir]).C = chave_inserir->C;
    (vet_chaves[pos_inserir]).Pr = chave_inserir->Pr;
}

void insere_ordenado_vet_chaves(chave_t *vet_chaves, chave_t *chave_inserir, int tam_vet_chaves){//[0, 3, 4] <- 2 => [0, 2, 3, 4]
    //Insere  uma chave (em um vetor de chaves) em sua posição ideal, de modo a manter a ordem.
    if(tam_vet_chaves == 0){//vetor vazio, insiro na primeira posição
        insere_chave_em_vet_chaves(vet_chaves, chave_inserir, 0);
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
                break;
            }
        }
        if(fez_shifitada == -1){//Se não fez shifitada, então a chave que vou inserir é a maior chave do nó
            insere_chave_em_vet_chaves(vet_chaves, chave_inserir, tam_vet_chaves);//então insiro ela na ultima posição
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

no_arvore_t *get_pagina_irma(FILE *arqArvore,no_arvore_t *no_mae, no_arvore_t *no_filho, FncGetRRNirma retorna_irma){
    int menor_C_filho = (no_filho->chaves)[0].C;//menor chave do filho

    //Obter o RRN do filho.
    int RRN_no_filho;
    buscaBinNo(no_mae, 0, no_mae->n - 1, menor_C_filho, &RRN_no_filho);

    //Obter o RRN da página irmã
    int RRN_irma = get_RRN_irma(no_mae, RRN_no_filho, retorna_irma);
    if(RRN_irma == -1){//Não tem irmã
        return NULL;
    }

    no_arvore_t *no_irma = alocar_no();//aloco o nó
    fseek(arqArvore, RRN_irma+1, SEEK_SET);//posiciono o cursor para leitura do nó
    fluxo_no(arqArvore, no_irma, meu_fread);//leio o nó

    return no_irma;//retorno o nó
}

int redistribuicao(FILE *arqArvore, no_arvore_t *no_mae, no_arvore_t *no_filho, chave_t *chave_inserir){
    //Primeiramente, encontrar a página irmã à esquerda
    no_arvore_t *no_irma = get_pagina_irma(arqArvore,no_mae, no_filho, retorna_irma_esq);
    if(no_irma = NULL){//Se não conseguiu obter a página irmã à esquerda

    }
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
