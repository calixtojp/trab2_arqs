#include <stdio.h>
#include <stdlib.h>
#include "auxiliares.h"
#include "arvore_b_estrela.h"

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

InfoInserida_t *alocar_InfoInserida(void){
    InfoInserida_t *info_retorno = malloc(sizeof(InfoInserida_t));
    info_retorno->chave = alocar_chave();
    info_retorno->ponteiro = malloc(sizeof(int));

    return info_retorno;
}

void desalocar_InfoInserida(InfoInserida_t *info){
    free(info->ponteiro);
    free(info->chave);
    free(info);
}

void set_InfoInserida(InfoInserida_t *info, int C, int Pr, int info_valida, int ponteiro_inserido){
    info->chave->C = C;
    info->chave->Pr = Pr;
    info->valida = info_valida;
    *(info->ponteiro) = ponteiro_inserido;
}

cabecalho_arvore_t *alocar_cabecalho_arvore(void){
    cabecalho_arvore_t *cabecalho_retorno = malloc(sizeof(cabecalho_arvore_t));
    
    return cabecalho_retorno;
}

no_arvore_t *alocar_no(void){
    //Função que aloca um nó com valores nulos
    no_arvore_t *no = malloc(sizeof(no_arvore_t));
    
    no->n = 0;

    for(int i=0; i<M-1; i++){
        no->chaves[i].C = -1;
        no->chaves[i].Pr = -1;
    }

    for(int i=0; i<M; i++){
        no->P[i] = -1;
    }    

    no->nivel = -1;

    return no;
}

void desalocar_no(no_arvore_t *no){
    free(no);
}

pagina_t *aloca_pagina(void){
    //aloco uma pagina com valores nulos
    pagina_t *pagina = malloc(sizeof(pagina_t));
    pagina->no = alocar_no();
    pagina->RRN_no = -1;
    return pagina;
}

void desaloca_pagina(pagina_t *pagina){
    if(pagina != NULL){
        desalocar_no(pagina->no);
        free(pagina);
    }
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

chave_t *alocar_chave(){
    chave_t *chave = malloc(sizeof(chave_t));
    return chave;
}

void desalocar_chave(chave_t *chave){
    free(chave);
}

void mostra_cabecalho_arvore(cabecalho_arvore_t *cabecalho){
    //Função de debug
    printf("Cabeçalho da arvore: \n");
    printf("status=%c|noRaiz=%d|RRNproxNo=%d|nroNiveis=%d|nroChaves=%d\n"
        , cabecalho->status
        , cabecalho->noRaiz
        , cabecalho->RRNproxNo
        , cabecalho->nroNiveis
        , cabecalho->nroChaves
    );
}

void mostra_vet_ponteiros(int *ponteiros, int tam){
    //Função de debud
    for(int i=0; i<tam; i++){
        printf("\tP%d:%d\n",i+1,ponteiros[i]);
    }
    printf("\n");
}

void mostra_ponteiro(no_arvore_t *no, int pos_vet_ponteiros){
    printf("\tP%d=%d|\n", pos_vet_ponteiros, no->P[pos_vet_ponteiros]);
}

void mostra_chave(chave_t *chave, int pos){
    printf("\tC%d:%d|Pr%d:%lld\n", pos, chave[pos].C, pos, chave[pos].Pr);
}

void mostra_vet_chaves(chave_t *vet, int tam){
    for(int i = 0; i < tam; ++i){
        mostra_chave(vet, i);
    }
    printf("\n");
}

void mostrar_no(no_arvore_t *no){
    
    printf("N:%d | Nivel: %d\n",no->n, no->nivel);
    mostra_ponteiro(no, 0);

    for(int i = 0; i < M-1;++i){
        mostra_chave(no->chaves, i);
        mostra_ponteiro(no, i+1);
    }
    printf("\n");
}

void mostrar_pagina(pagina_t *pgn){
    printf("RRN:%d\n", pgn->RRN_no);
    mostrar_no(pgn->no);
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

void insereChave(no_arvore_t *no, int pos1, void *chaves_param, int pos2){
    //Função que insere uma chave em um nó em uma posição específica.
    chave_t *chaves = (chave_t *) chaves_param;

    if(chaves != NULL){
        //se há um valor para ser copiado, copio
        ((no->chaves)[pos1]).C = chaves[pos2].C;
        ((no->chaves)[pos1]).Pr = chaves[pos2].Pr;
    }else{
        //senão, insiro valores nulos
        ((no->chaves)[pos1]).C = -1;
        ((no->chaves)[pos1]).Pr = -1;
    }
}

void inserePonteiro(no_arvore_t *no, int pos1, void *pont_param, int pos2){
    //Função que insere um ponteiro em um nó em uma posição específica.

    int *ponteiros = (int *) pont_param;

    if(ponteiros != NULL){
        //se há um valor para ser copiado, copio
        (no->P)[pos1] = ponteiros[pos2];
    }else{
        //senão, insiro valores nulos
        (no->P)[pos1] = -1;
    }
}

void setVetNo(no_arvore_t *no, void *vetor, int ini_chaves, int fim_chaves, int tam_max, FncInsereNo insere){
    //preenche um dos vetores do nó a partir do vetor passado por parâmetro.
    //Qual vetor do nó receberá as informações é definido pelo parâmetro 'FncInsereNo insere'.
    int i;
    for(i=ini_chaves; i<=fim_chaves; i++){
        insere(no,i-ini_chaves,vetor,i);
    }

    //Se sobrarem posições vazias no vetor do nó, completa-se com valores nulos
    while(i-ini_chaves<tam_max){
        insere(no,i-ini_chaves,NULL,-1);
        i++;
    }
}

void insere_chave_em_vet_chaves(chave_t *vet_chaves, chave_t *chave_inserir, int pos_inserir){
    (vet_chaves[pos_inserir]).C = chave_inserir->C;
    (vet_chaves[pos_inserir]).Pr = chave_inserir->Pr;
}

void insere_ponteiro_em_vet(int *vet_ponteiros, int tam_vet_ponteiros, int pos, int ponteiro){
    for(int i = tam_vet_ponteiros-1; i > pos; --i){
        //Faço uma shifitada de todos os ponteiros à direita da posição 'pos'
        vet_ponteiros[i] = vet_ponteiros[i-1]; 
    }

    vet_ponteiros[pos] = ponteiro;//depois insiro esse ponteiro em sua posição;
}

void copia_vet_chaves(chave_t *origem, chave_t *destino,
                                int ini_origem, int fim_origem, int ini_destino, int fim_destino){

    int cont_origem = ini_origem;
    int cont_destino = ini_destino;
    while(cont_origem <= fim_origem){
        insere_chave_em_vet_chaves(destino, &(origem[cont_origem]), cont_destino);
        ++cont_destino;++cont_origem;        
    }    
}

void copia_vet_ponteiros(int *origem, int *destino,
                                int ini_origem, int fim_origem, int ini_destino, int fim_destino){
                                    
    int cont_origem = ini_origem;
    int cont_destino = ini_destino;
    while(cont_origem <= fim_origem){
        destino[cont_destino] = origem[cont_origem];
        ++cont_destino;++cont_origem;
    }    
}

void set_noRaiz(cabecalho_arvore_t *cabecalho, int nova_raiz){
    cabecalho->noRaiz = nova_raiz;
}

int get_nroChaves(cabecalho_arvore_t *cabecalho){
    return cabecalho->nroChaves;
}

void set_nroChaves(cabecalho_arvore_t *cabecalho, int nroChaves){
    cabecalho->nroChaves = nroChaves;
}

int get_RRNproxNo(cabecalho_arvore_t *cabecalho){
    return cabecalho->RRNproxNo;
}

void set_RRNproxNo(cabecalho_arvore_t *cabecalho, int RRN){
    cabecalho->RRNproxNo = RRN;
}

void set_lixoCabecalho(cabecalho_arvore_t *cabecalho){
    for(int i = 0; i < 59; ++i){
        cabecalho->lixo[i] = '$';
    }
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

int get_nivel_no(no_arvore_t *no){
    return no->nivel;
}

void set_nivel_no(no_arvore_t *no, int nivel){
    no->nivel = nivel;
}

int get_chaves_C(chave_t *chave){
    return chave->C;
}

void set_chaves_C(chave_t *chave, int C){
    chave->C = C;
}

long int get_chaves_Pr(chave_t *chave){
    return chave->Pr;
}

void set_chaves_Pr(chave_t *chave, int Pr){
    chave->Pr = Pr;
}

void fluxo_StatusArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo){
    //Escreve ou lê o cabeçalho da árvore.
    funcFluxo(&cabecalho->status,sizeof(char),1,arqArvore);
}

void fluxo_CabecalhoArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho, FncFluxoMemSec funcFluxo){
    //Escreve ou lê o cabecalho do arquivo da árvore B*, a dependeder do 'funcFluxo'
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
        //-Chamo a função recursivamente com os valores atualizados
        return buscaBinNo(no_atual, ini, fim, chave, P);
    }else{
        //Se o valor que estou buscando é maior que o atual:
        //-Preparo a busca recursiva a partir do atual+1
        ini = meio+1;
        //-Atribuo, ao ponteiro P, o ponteiro à direita do valor atual 
        *P = no_atual->P[meio+1];
        //-Chamo a função recursivamente com os valores atualizados
        return buscaBinNo(no_atual, ini, fim, chave, P);
    }
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

int insere_ordenado_vet_chaves(chave_t *vet_chaves, chave_t *chave_inserir, int tam_vet_chaves_antes){
    //Insere  uma chave (em um vetor de chaves) em sua posição ideal, de modo a manter a ordem.
    //Retorna a posição em que a chave foi inserida
    if(tam_vet_chaves_antes == 0){//vetor vazio, insiro na primeira posição
        insere_chave_em_vet_chaves(vet_chaves, chave_inserir, 0);
        return 0;
    }else{
        int fez_shiftada = -1;//flag que indica se houve shiftada
        for(int i = 0; i < tam_vet_chaves_antes; ++i){
            if(compara_chaves(chave_inserir, &(vet_chaves[i])) < 0){
                //Se a chave que vou inserir for menor que a 'i', então vou inseri-la
                //na posição i e shifitar todas as demais para a posição à direita no nó
                fez_shiftada = 1; //altero a flag

                //Começo pela última chave e vou até a chave adjacente à direita de 'i'
                for(int j = tam_vet_chaves_antes; j > i; --j){
                    // insiro a chave da posição j-1 na posição j
                    insere_chave_em_vet_chaves(vet_chaves, &(vet_chaves[j-1]), j);
                }
                //insere chave em sua posição (i)
                insere_chave_em_vet_chaves(vet_chaves, chave_inserir, i);
                return i;
            }
        }
        if(fez_shiftada == -1){
            //Se não fez shiftada, então a chave que vou inserir é a maior chave do nó,
            //então insiro ela na ultima posição
            insere_chave_em_vet_chaves(vet_chaves, chave_inserir, tam_vet_chaves_antes);
            return tam_vet_chaves_antes;
        }
    }
}

void insere_ordenado_no(FILE *arq, pagina_t *pgn, InfoInserida_t *info){
    //Insere uma chave (em um nó) em sua posição ideal, de modo a manter a ordem. 
    int pos = insere_ordenado_vet_chaves(pgn->no->chaves, info->chave, pgn->no->n);
    pgn->no->n++;

    insere_ponteiro_em_vet(pgn->no->P,(pgn->no->n)+1,pos+1,*(info->ponteiro));

    fseek(arq, ((pgn->RRN_no)+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arq, pgn->no, meu_fwrite);

    //Já que consegui inserir ordenado no nó, garanto que não há promoção de chaves para nós acima.
    //Então devo indicar para os nós acima que a informação de inserção é inválida,
    //pois já foi inserida nesse nó.
    info->valida = -1;
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

pagina_t *get_pagina_irma(FILE *arqArvore,pagina_t *pgn_mae, pagina_t *pgn_atual, FncGetRRNirma retorna_irma){   
    //Retorna o ponteiro para a página lida caso consiga ler a página irmã.
    //Retorna NULL caso não consiga ler a página irmã.

    //Obter o RRN da página irmã
    int RRN_irma = get_RRN_irma(pgn_mae->no, pgn_atual->RRN_no, retorna_irma);
    if(RRN_irma == -1){//Não tem irmã
        return NULL;
    }

    pagina_t *pagina_irma = aloca_pagina();//aloco a página
    pagina_irma->RRN_no = RRN_irma;//configuro o RRN

    fseek(arqArvore, (RRN_irma+1) * TAM_PAGINA, SEEK_SET);//posiciono o cursor para leitura do nó
    fluxo_no(arqArvore, pagina_irma->no, meu_fread);//leio o nó

    return pagina_irma;   
}

int get_pos_chave_mae(pagina_t *pgn_mae, pagina_t *pgn_esq){
    //faz busca sequencial no vetor de ponteiros do nó mãe pelo RRN do filho à esquerda
    //A posição do RRN do filho da esquerda é igual à pos da chave mãe

    for(int i=0; i<(pgn_mae->no->n)+1; i++){
        if(compara_ponteiros(pgn_mae->no->P, i, &(pgn_esq->RRN_no)) == 0){
            return i;
        }
    }

    return -1;
}

void redistribui_paginas(FILE *arqArvore, pagina_t *pgn_mae, pagina_t *pgn_esq, pagina_t *pgn_dir,
                                                                                InfoInserida_t *info){

    //Crio um vetor temporário de ponteiros e de chaves
    int tam_ponteiros_vet_temp = pgn_esq->no->n + pgn_dir->no->n + 3;//esq + dir + ponteiro_inserido
    int tam_chaves_vet_temp = pgn_esq->no->n + pgn_dir->no->n + 2;//esq + dir + chave_inserida + chave_mae

    int ponteiros_vet_temp[tam_ponteiros_vet_temp];
    chave_t chaves_vet_temp[tam_chaves_vet_temp];

    //Obtenho a posição da chave mãe.
    int pos_chave_mae = get_pos_chave_mae(pgn_mae, pgn_esq);

    //Coloco as chaves em seu vetor temporário, de modo a guardar a posição em que insiro a chave_promovida.
    //Para isso, devo:
    int pos_inserir_ponteiro;
    {
        //1-Colocar as chaves da página à esquerda
        int i;
        for(i = 0; i < pgn_esq->no->n; ++i){
            insere_ordenado_vet_chaves(chaves_vet_temp, &(pgn_esq->no->chaves[i]), i);
        }

        //2-Coloar as chaves da página à direita
        for(int j = 0; j < pgn_dir->no->n; ++j, ++i){
            insere_ordenado_vet_chaves(chaves_vet_temp, &(pgn_dir->no->chaves[j]), i);
        }

        //3-Colocar a chave_mae.
        insere_ordenado_vet_chaves(chaves_vet_temp, &(pgn_mae->no->chaves[pos_chave_mae]), tam_chaves_vet_temp-2);

        //4-Colocar a chave_promovida e guardar sua posição de inserção.
        pos_inserir_ponteiro = insere_ordenado_vet_chaves(chaves_vet_temp, info->chave, tam_chaves_vet_temp-1);
    }

    //Agora devo inserir os ponteiros. Para isso, devo:
    {
        //1-inserir os ponteiros da página à esquerda
        int i;
        for(i = 0; i < pgn_esq->no->n + 1; ++i){
            ponteiros_vet_temp[i] = pgn_esq->no->P[i];
        }
        //2-inserir os ponteiros da página à direita
        for(int j = 0; j < pgn_dir->no->n + 1; ++j, ++i){
            ponteiros_vet_temp[i] = pgn_dir->no->P[j];
        }

        //3-Por fim, inserir o ponteiro promovido, de movo a deslocar todos os demais ponteiros à direita em uma posição
        insere_ponteiro_em_vet(ponteiros_vet_temp, tam_ponteiros_vet_temp, pos_inserir_ponteiro+1, *(info->ponteiro));
    }

    //Com os os ponteiros e chaves em ordem, sobrescrevo a informações dos nós. Para isso:

    //1-Obtenho a posição da chave_mae
    int meio = (tam_chaves_vet_temp)/2;

    //2-Sobrescrevo a chave_mae com a chave encontrada no meio do vetor de chaves
    insere_chave_em_vet_chaves(pgn_mae->no->chaves, &(chaves_vet_temp[meio]), pos_chave_mae);

    //3-Sobrescrevo as chaves da página à esquerda
    setVetNo(pgn_esq->no, chaves_vet_temp, 0, meio-1, M-1, insereChave);

    //4-Sobrescrevo as chaves da página à direita
    setVetNo(pgn_dir->no, chaves_vet_temp, meio+1, tam_chaves_vet_temp-1, M-1, insereChave);

    //5-Sobrescrevo os ponteiros da página à esquerda
    setVetNo(pgn_esq->no, ponteiros_vet_temp, 0, meio, M, inserePonteiro);

    //6-Sobrescrevo os ponteiros da página à direita
    setVetNo(pgn_dir->no, ponteiros_vet_temp, meio+1, tam_ponteiros_vet_temp-1, M, inserePonteiro);

    //7-Modifico a informação acerca da ocupação das páginas
    pgn_esq->no->n = meio;
    pgn_dir->no->n = tam_chaves_vet_temp-meio-1;

    //Escrever os nós que foram modificados
    fseek(arqArvore, (pgn_mae->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_mae->no, meu_fwrite);
    fseek(arqArvore, (pgn_esq->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_esq->no, meu_fwrite);
    fseek(arqArvore, (pgn_dir->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_dir->no, meu_fwrite);
}

result_redistribuicao_t escolheIrma(pagina_t **pgn_irma, pagina_t *pgn_irma_esq, pagina_t *pgn_irma_dir){
    //Função que escolhe qual página será utilizada no split 2 para 3.

    if(pgn_irma_dir != NULL){
        *pgn_irma = pgn_irma_dir;
        desaloca_pagina(pgn_irma_esq);
        return retorna_dir;
    }else{
        *pgn_irma = pgn_irma_esq;
        desaloca_pagina(pgn_irma_dir);
        return retorna_esq;
    }
}

result_redistribuicao_t redistribuicao(FILE *arqArvore, pagina_t *pgn_mae, pagina_t *pgn_atual, 
                                                        pagina_t **pgn_irma, InfoInserida_t *info){

    //  Função que tenta realizar a redistribuição. Essa função recebe um ponteiro "pgn_irma" o qual
    //pode ser modificado caso a redistribuição não tenha sucesso. Nesse caso, como tenho a página
    //irmã à direita e à esquerda carregada em RAM, faço essa operação para evitar fseeks desnecessários.
    //  Desse modo, caso não consiga realizar a redistribuição, o split_2_para_3 terá informação sobre
    //qual página irmã será utilizada para realizar o split.

    //Primeiramente, encontrar a página irmã à esquerda.
    pagina_t *pgn_irma_esq = get_pagina_irma(arqArvore,pgn_mae, pgn_atual, retorna_irma_esq);
    if(pgn_irma_esq != NULL && (pgn_irma_esq->no->n < M-1)){
        //Se consegui obter a página irmã à esquerda e ela não está cheia
        //então faço a redistribuição com ela.
        redistribui_paginas(arqArvore, pgn_mae, pgn_irma_esq, pgn_atual, info);

        //retorno a página utilizada por referência
        *pgn_irma = pgn_irma_esq;

        //retorno que a redistribuição foi concluída.
        return redistribuiu;
    }
    
    //Depois, tento com a página da direita    
    pagina_t *pgn_irma_dir = get_pagina_irma(arqArvore,pgn_mae, pgn_atual, retorna_irma_dir);
    if(pgn_irma_dir != NULL && (pgn_irma_dir->no->n < M-1)){
        //Se consegui obter a página irmã à direita e ela não está cheia
        //então faço a redistribuição com ela.
        redistribui_paginas(arqArvore, pgn_mae, pgn_atual, pgn_irma_dir, info);

        //retorno a página utilizada por referência
        *pgn_irma = pgn_irma_dir;

        //desaloco a página inútil
        desaloca_pagina(pgn_irma_esq);

        //retorno que a redistribuição foi concluída.
        return redistribuiu;
    }
    
    /*Dado que não foi feita a redistribuição, sabe-se que deve ser feito o split 2 para 3.  
    Então, devo escolher uma das irmãs para que o split ocorra*/

    return escolheIrma(pgn_irma, pgn_irma_esq, pgn_irma_dir);
}

void split_1_para_2(FILE *arqArvore, cabecalho_arvore_t *cabecalho, pagina_t *pgn_atual, InfoInserida_t *info){
    
    //Crio vetores auxiliares que contém as chaves e ponteiros do nó atual e da info que desejo inserir
    chave_t *vet_chaves = aloca_vet_chaves(M);
    int *vet_ponteiros = aloca_vet_ponteiros(M+1);
    
    //preenchimento do vetor de chaves.
    for(int i=0; i<M-1; i++){
        insere_chave_em_vet_chaves(vet_chaves, &((pgn_atual->no->chaves)[i]), i);
    }

    int pos = insere_ordenado_vet_chaves(vet_chaves,info->chave,M-1);
    //'int pos' recebe a posição em que a chave a ser inserida ocupa no vetor de chaves

    //preenchimento do vetor de ponteiros
    /*o ponteiro que acompanha a chave a ser inserida deve ser inserido 
    na posição que ele ocupa no vet_chaves mais 1, ou seja, pos+1*/
    copia_vet_ponteiros(pgn_atual->no->P, vet_ponteiros, 0, M, 0, M);
    insere_ponteiro_em_vet(vet_ponteiros,M+1,pos+1,*(info->ponteiro));

    //Posição do meio do vetor de chaves, que contém o valor a ser promovido
    int meio = M/2;
    
    //atualiza-se o nó atual com os valores menores que o promovido (do meio)
    setVetNo(pgn_atual->no,vet_chaves,0,meio-1, M-1, insereChave);
    setVetNo(pgn_atual->no, vet_ponteiros,0,meio, M, inserePonteiro);
    pgn_atual->no->n = (M+1)/2 - 1;

    //cria-se um nó a direita do atual, com os valores maiores que o promovido
    pagina_t *pgn_dir = aloca_pagina();
    setVetNo(pgn_dir->no,vet_chaves,meio+1,M-1, M-1, insereChave);
    setVetNo(pgn_dir->no,vet_ponteiros,meio+1,M, M, inserePonteiro);
    pgn_dir->no->n = (M+1)/2 - 1;
    pgn_dir->RRN_no = cabecalho->RRNproxNo;
    pgn_dir->no->nivel = cabecalho->nroNiveis;

    //Como foi criado um novo nó, atualizo o RRN do próximo nó 
    cabecalho->RRNproxNo++;

    //Vou para o RRN da pagina atual para reescrevê-la (soma-se 1 para pular o cabecalho)
    fseek(arqArvore, (pgn_atual->RRN_no + 1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_atual->no, meu_fwrite);

    //Agora vou ao final do arquivo, para a escrever a página criada
    fseek(arqArvore, 0, SEEK_END);
    fluxo_no(arqArvore, pgn_dir->no, meu_fwrite);
    
    //cria-se um novo nó raiz, com o valor médio do vetor de chaves
    pagina_t *pgn_mae = aloca_pagina();
    //copio o valor do meio para o nó mãe
    setVetNo(pgn_mae->no,vet_chaves,meio,meio, M-1, insereChave);
    /*Defino o ponteiro à esquerda do valor promovido como sendo 
    o RRN do nó atual e o da direita o RRN do nó à direita do atual*/
    pgn_mae->no->P[0] = pgn_atual->RRN_no; 
    pgn_mae->no->P[1] = pgn_dir->RRN_no;
    //
    pgn_mae->no->n = 1;
    cabecalho->nroNiveis++;
    pgn_mae->no->nivel = cabecalho->nroNiveis;
    pgn_mae->RRN_no = cabecalho->RRNproxNo;
    //Como foi criado um novo nó, atualizo o RRN do próximo nó
    cabecalho->RRNproxNo++;

    //Escrevo a nova raiz no fim do arquivo
    fluxo_no(arqArvore, pgn_mae->no, meu_fwrite);

    //atualizo a raiz 
    cabecalho->noRaiz = pgn_mae->RRN_no;

    //desaloco as paginas criadas
    desaloca_pagina(pgn_dir);
    desaloca_pagina(pgn_mae);

    //informo que não há dado para ser inserido na próxima volta da recursão
    info->valida = -1;
}

void split_2_para_3(FILE *arqArvore, cabecalho_arvore_t *cabecalho,
                        pagina_t *pgn_mae, pagina_t *pgn_esq, pagina_t *pgn_dir,InfoInserida_t *info){

    int tam_chaves_vet_temp = (M-1)*2 + 2;//chaves da direita + esquerda + chave_inserir + chave_mae
    int tam_ponteiros_vet_temp = 2*M + 1;//ponteiros da direita + esquerda + ponteiro_inserir
    chave_t *chaves_vet_temp = aloca_vet_chaves(tam_chaves_vet_temp);
    int *ponteiros_vet_temp = aloca_vet_ponteiros(tam_ponteiros_vet_temp);

    //Colo os dados das chaves no vetor de chaves temporário. Para isso,

    //1-Obtenho a posição da chave_mae no vetor de chaves do nó mãe
    int pos_chave_mae = get_pos_chave_mae(pgn_mae, pgn_esq);
    
    //2-Inserir as chaves do nó à esquerda
    copia_vet_chaves(pgn_esq->no->chaves, chaves_vet_temp, 0, M-2, 0, M-2);

    //3-Inserir a chave_mae
    insere_chave_em_vet_chaves(chaves_vet_temp, &(pgn_mae->no->chaves[pos_chave_mae]), M-1);

    //4-Inserir as chaves do nó à direita
    copia_vet_chaves(pgn_dir->no->chaves, chaves_vet_temp, 0, M-2, M, tam_chaves_vet_temp-2); 

    //5-Inserir a chave da InfoInserida e obter a posição em que foi inserida
    int pos_inserir_ponteiro = insere_ordenado_vet_chaves(chaves_vet_temp, info->chave, tam_chaves_vet_temp-1);

    //Agora, devo preencher os dados relativos aos ponteiros
    copia_vet_ponteiros(pgn_esq->no->P, ponteiros_vet_temp, 0, M-1, 0, M-1);
    copia_vet_ponteiros(pgn_dir->no->P, ponteiros_vet_temp, 0, M-1, M, tam_ponteiros_vet_temp-2);
    insere_ponteiro_em_vet(ponteiros_vet_temp, tam_ponteiros_vet_temp, pos_inserir_ponteiro+1, *(info->ponteiro));

    //Escrevo os vetores temporários nos nós, em suas devidas posições. Para isso:
    int pos_promovido_esq = tam_ponteiros_vet_temp/3;//1-Calculo o valor da primeira chave que será promovida
    int pos_promovido_dir = pos_promovido_esq*2 + 1;//2-Calculo o valor da segunda chave que será promovida
    //4-Devo criar uma página que irá conter os maiores valores (chaves_vet_temp e de ponteiros_vet_temp)
    pagina_t *pgn_nova = aloca_pagina();
    pgn_nova->no->n = tam_chaves_vet_temp - pos_promovido_dir - 1;
    pgn_nova->no->nivel = pgn_esq->no->nivel;
    pgn_nova->RRN_no = cabecalho->RRNproxNo;
    (cabecalho->RRNproxNo)++;

    //5-Escrever as chaves:
        //5.1-Coloco as menores chaves no nó da esquerda
    setVetNo(pgn_esq->no, chaves_vet_temp, 0, pos_promovido_esq-1, M-1, insereChave);
    pgn_esq->no->n = pos_promovido_esq; //atualizo o numero de chaves do nó

        //5.2-Coloco as chaves intermediárias no nó da direita
    setVetNo(pgn_dir->no, chaves_vet_temp, pos_promovido_esq+1, pos_promovido_dir-1, M-1, insereChave);
    pgn_dir->no->n = pos_promovido_esq; //atualizo o numero de chaves do nó
    
        //5.3-Coloco as maiores chaves no nó criado
    setVetNo(pgn_nova->no, chaves_vet_temp, pos_promovido_dir+1, tam_chaves_vet_temp-1, M-1, insereChave);
    
        //5.4-Coloco a primeira chave promovida, no nó mãe
    insere_chave_em_vet_chaves(pgn_mae->no->chaves, &(chaves_vet_temp[pos_promovido_esq]), pos_chave_mae);

    //6-Escrever os ponteiros:

        //6.1-Coloco os menores ponteiros no nó da esquerda
    setVetNo(pgn_esq->no, ponteiros_vet_temp, 0, pos_promovido_esq, M, inserePonteiro);

        //6.2-Coloco os ponteiros intermediárias no nó da direita
    setVetNo(pgn_dir->no, ponteiros_vet_temp, pos_promovido_esq+1, pos_promovido_dir, M, inserePonteiro);

        //6.3-Coloco os maiores ponteiros no nó criado
    setVetNo(pgn_nova->no, ponteiros_vet_temp, pos_promovido_dir+1, tam_ponteiros_vet_temp-1, M, inserePonteiro);

    //Escrever os nós que foram modificados
    fseek(arqArvore, (pgn_mae->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_mae->no, meu_fwrite);
    fseek(arqArvore, (pgn_esq->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_esq->no, meu_fwrite);
    fseek(arqArvore, (pgn_dir->RRN_no+1)*TAM_PAGINA, SEEK_SET);
    fluxo_no(arqArvore, pgn_dir->no, meu_fwrite);
    fseek(arqArvore, (pgn_nova->RRN_no+1)*TAM_PAGINA, SEEK_SET);//Escrevo a nova página no 
    fluxo_no(arqArvore, pgn_nova->no, meu_fwrite);

    //Por fim, devo fazer com que a próxima chave a ser inserida seja a chave promovida à direita
    info->valida = 1;
    *(info->chave) = chaves_vet_temp[pos_promovido_dir];
    *(info->ponteiro) = pgn_nova->RRN_no;

    desaloca_pagina(pgn_nova);
    desaloca_vet_chaves(chaves_vet_temp);
    desaloca_vet_ponteiros(ponteiros_vet_temp);
}

int ehFolha(pagina_t *pgn_atual){
    //Função que verifica se um nó é folha com base em seu nível. 
    //Se é folha, retorna 1, se não retorna 0.

    if(pgn_atual->no->nivel == 1){
        return 1;
    }

    return 0;
}
