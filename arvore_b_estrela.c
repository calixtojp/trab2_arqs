#include <stdio.h>
#include <stdlib.h>
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

void ler_cabecalho_arq_arvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho){
    fread(&(cabecalho->status), sizeof(char), 1, arqArvore);
    fread(&(cabecalho->noRaiz), sizeof(int), 1, arqArvore);
    fread(&(cabecalho->RRNproxNo), sizeof(int), 1, arqArvore);
    fread(&(cabecalho->nroNiveis), sizeof(int), 1, arqArvore);
    fread(&(cabecalho->nroChaves), sizeof(int), 1, arqArvore);
    for(int i = 0; i < 59; ++i){
        fread(&(cabecalho->lixo[i]), sizeof(char), 1, arqArvore);
    }
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
    return cabecalho->nroNiveis = nova_nroNiveis;
}
void fwriteStatusArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho){
    fwrite(&cabecalho->status,sizeof(char),1,arqArvore);
}

void fwriteCabecalhoArvore(FILE *arqArvore, cabecalho_arvore_t *cabecalho){
    fwrite(&(cabecalho->status), sizeof(char), 1, arqArvore);
    fwrite(&(cabecalho->noRaiz), sizeof(int), 1, arqArvore);
    fwrite(&(cabecalho->RRNproxNo), sizeof(int), 1, arqArvore);    
    fwrite(&(cabecalho->nroNiveis), sizeof(int), 1, arqArvore);    
    fwrite(&(cabecalho->nroChaves), sizeof(int), 1, arqArvore);    
    char lixo = '$';
    for(int i = 0; i < 59; ++i){
        fwrite(&lixo, sizeof(char), 1, arqArvore);
    }
}

void ler_chave(FILE *arqArvore, no_arvore_t *no, int pos_vet_chaves){
    fread(&((no->chaves[pos_vet_chaves]).C), sizeof(int), 1, arqArvore);
    fread(&((no->chaves[pos_vet_chaves]).Pr), sizeof(long long), 1, arqArvore);
}

void ler_ponteiro(FILE *arqArvore, no_arvore_t *no, int pos_vet_ponteiros){
    fread(&(no->P[pos_vet_ponteiros]), sizeof(int), 1, arqArvore);
}

void ler_pagina_disco(FILE *arqArvore, no_arvore_t *no){

    //ler o nível e o 'n'
    fread(&(no->nivel), sizeof(int), 1, arqArvore);
    fread(&(no->n), sizeof(int), 1, arqArvore);

    //ler o primeiro ponteiro do no
    ler_ponteiro(arqArvore, no, 0);      
    //ler os demais conjuntos de ponteiro e chave
    for(int i = 0; i < M - 1; ++i){
        ler_chave(arqArvore, no, i);
        ler_ponteiro(arqArvore, no, i+1);
    }
}

long long int busca_bin_no(no_arvore_t *no_atual, int ini, int fim, int chave, int *P){
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
        return busca_bin_no(no_atual, ini, fim, chave, P);
    }else{
        //Se o valor que estou buscando é maior que o atual:
        //-Preparo a busca recursiva a partir do atual+1
        ini = meio+1;
        //-Atribuo, ao ponteiro P, o ponteiro à direita do valor atual 
        *P = no_atual->P[meio+1];
        // printf("*P direita:%d\n", *P);
        //-Chamo a função recursivamente com os valores atualizados
        return busca_bin_no(no_atual, ini, fim, chave, P);
    }
}

void set_no(no_arvore_t *no_configurar, int nChaves, int nivel){
    no_configurar->n = nChaves;
    no_configurar->nivel = nivel;
}

int compara_chaves(chave_t *chave_a, chave_t *chave_b){
    return chave_a->C - chave_b->C;
}

void insere_chave_em_no(no_arvore_t *no_inserir, chave_t *chave_inserir, int pos_inserir){
    (no_inserir->chaves[pos_inserir]).C = chave_inserir->C;
    (no_inserir->chaves[pos_inserir]).Pr = chave_inserir->Pr;
}

void insere_ordenado(no_arvore_t *no_inserir, chave_t *chave_inserir){
    if(no_inserir->n == 0){//Se é um nó vazio, insiro na primeira posição
        insere_chave_em_no(no_inserir, chave_inserir, 0);
    }else{
        int n = get_nChaves(no_inserir);
        int ultima_pos_no = n;//não há necessidade prática da variável, é tão somente para facilitar o entendimento
        int fez_shifitada = -1;//flag que indica se houve shifitada
        for(int i = 0; i < n; ++i){
            if(compara_chaves(chave_inserir, &(no_inserir->chaves[i])) < 0){
                //Se a chave que vou inserir for menor que a 'i', então vou inseri-la
                //na posição i e shifitar todas as demais para a posição à direita no nó
                fez_shifitada = 1; //altero a flag
                for(int j = ultima_pos_no; j > i; --j){//Começo pela última chave e vou até a chave adjacente à direita de 'i'
                    // insiro a chave da posição j-1 na posição j
                    insere_chave_em_no(no_inserir, &(no_inserir->chaves[j-1]), j);
                }
                insere_chave_em_no(no_inserir, chave_inserir, i);
                break;
            }
        }
        if(fez_shifitada == -1){//Se não fez shifitada, então a chave que vou inserir é a maior chave do nó
            insere_chave_em_no(no_inserir, chave_inserir, ultima_pos_no);//então insiro ela na ultima posição
        }
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
