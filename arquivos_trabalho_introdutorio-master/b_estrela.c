#include "b_estrela.h"
#include "util.h"


/* Cabeçalho das funções internas */

void inicializar_arvore(Arvore_B *arvore);
void inicializar_ramo(No *meu_no);

void split_1pra2(Arvore_B *arvore, Chave chave_nova, int novo_no);
void split_2pra3(FILE *arq_arv, Arvore_B *arvore, Chave *chave_nova, int *rrn_novo, int rrn_atual, int rrn_pai);

int busca_binariaValor(Chave *chaves, int valor_buscado, int esquerdo, int direito);
int busca_binariaDesc(No ramo, int valor_buscado, int esquerdo, int direito);

void inserir_ordenadoChave(Chave *chaves, Chave chave_nova, int tamanho);
void inserir_ordenadoDesc(No *ramos, int *desc_novo, int desc_pos);
void inserir_chave(No* ramos, Chave *chave_nova, int *rrn_novo);
void inserir_vetor(No *rrn_atual, Chave *chaves, int *vet_desc, int qtd, int inicio);
int  inserir_chaveNo(FILE *arq_arv, Arvore_B *arvore, Chave *chave_nova, int *rrn_novo, int rrn_atual, int rrn_pai);
int *inserir_descVetor(int *vet_antigoDesc, int novo_desc, int posicao, int tamanho);

int encontrar_irmaViavel(FILE *arq_arv, No *ramos, int rrn_pai, int pos_noAtual);
int encontrar_posPonteiro(No *ramos, int rrn_pai, int rrn_atual);

int redistribuir(No *ramos, Chave chave_nova, int *descendente_novo, int rrn_pai, int rrn_atual, int rrn_irmaViavel);
int redistribuicao_possivel(FILE *arq_arv, No *ramos, int rrn_pai, int rrn_atual);

Chave *criar_chaves(int tamanho);
bool   no_estaCheio(No *ramos, int pos_ramo);
void   unir_descendentes(int *descendentes, No ramo_esq, No ramo_dir);
void   copiar_ramo(No *destino, No fonte);
void   ler_ramoIndice(FILE *arq_arv, No *ramos, int pos_ramo, int RRN);
void   escrever_ramoArvore(FILE *arq_arv, No *ramos, int RRN ,int ramo_pos);


/* Funções disponíveis ao usuário */

Arvore_B* criar_arvore(){
    /* Funcao para criar uma estrutura de Árvore-B* e inicializar seus campos com valores padrão */
    Arvore_B *arvore = malloc(sizeof(Arvore_B));
    inicializar_arvore(arvore);
    return arvore;
}

void inserir_chaveArvore(FILE *arq_arv, Arvore_B *arvore, int id, long long int byteOffSet){
    /* 
    Função principal que adiciona um id e um byteOffSet especificados no arquivo
    de índice em Árvore-B*
    */

    // Caso seja a primeira inserção em uma árvore
    if(arvore->raiz == -1){
        // Atualizando campos da árvore
        arvore->raiz = 0; // Raiz se torna o ramo de RRN 0
        arvore->RRNproxNo = 1;
        arvore->nroNiveis = 1;
        // Posição 0 no vetor de ramos 'ramo_atual' se torna a raiz e seu nível é inicializado para 1
        arvore->ramos[ramo_atual].nivel = 1;
    }

    // Inicializando uma nova chave com o id e byteOffSet informados
    Chave chave_nova;
    chave_nova.id = id;
    chave_nova.byteOffset= byteOffSet;
    
    // Chamada da função que percorrerá a árvore em busca do local ideal para se inserir a nova chave
    int rrn_novo = -1, rrn_pai = arvore->raiz;
    inserir_chaveNo(arq_arv, arvore, &chave_nova, &rrn_novo, rrn_pai, rrn_pai);

    // Incrementando número de chaves da árvore
    arvore->nroChaves++;
}

void escrever_arvore(FILE *arq_bin, FILE *arq_arv, Arvore_B *arvore, Cabecalho *meu_cabecalho){
    /* 
    Função que dado um arquivo binário de registros (arq_bin), preenche um arquivo
    de índice em Árvore-B* (arq_arv)
    */

    Registro *meu_registro;
    // Bytes lidos começa em 17, pois pula o tamanho do cabeçalho do arquivo binário de registros
    long long int bytes_lidos = 17, byteOffset;

    // Percorrendo todos os registros do arquivo binário de registros
    for(int i = 0; i < meu_cabecalho->nroRegArq; i++){
        // Armazenando byteOffset do registro lido
        byteOffset = bytes_lidos; 
        meu_registro = ler_registro_binario(arq_bin,&bytes_lidos);

        // Caso o registro não esteja removido, é adicionado no arquivo de índice
        if(meu_registro->removido == nao_removido){
            inserir_chaveArvore(arq_arv, arvore, meu_registro->idCrime, byteOffset);
        }

        // Desalocando memória para próxima iteração
        liberar_registro(meu_registro);
    }

    /*
    Após todos os registros terem sido escritos, retorna ao começo do arquivo e
    escreve o cabeçalho da árvore-B* no arquivo de índice
    */
    fseek(arq_arv,0,SEEK_SET);
    escrever_cabecalhoArvore(arq_arv,arvore);
}

void liberar_arvore(Arvore_B *arv){
    /* Liberar memoria ocupada pela arvore */
    if(arv == NULL) return;
    free(arv->ramos);
    free(arv);
}

long long int buscar_valorRamo(FILE *arq_arv, No *ramos, int rrn_atual, int valor_buscado){
    /* 
    Função recursiva que busca um respectivo valor (valor_buscado) em um ramo da 
    Árvore-B*, em um arquivo de índice (arq_arv), dado seu RRN (rrn_atual)
    */
    
    // Caso base, elemento não encontrado, buscou nas folhas e não achou 
    if(rrn_atual == -1){
        return -1;
    }
    
    // Armazenando um ramo por vez em memória primária, na posição 0 do vetor de ramos (ramo_atual)
    ler_ramoIndice(arq_arv,ramos,ramo_atual,rrn_atual);

    // Buscando id desejado no ramo que foi armazenado em memória primária
    int pos_chave;
    pos_chave = busca_binariaValor(ramos[ramo_atual].chaves, valor_buscado, 0, (qtd_chavesMax - 1));

    // Encontrou chave, retornando o byteOffset do registro para posterior print
    if(pos_chave != -1){
        return ramos[ramo_atual].chaves[pos_chave].byteOffset;
    }

    /*
    Não encontrou, realiza busca recursiva no descendente para encontrar id desejado
    Ex:
    Dado um ramo x, buscando pelo id 4:
    . Vetor de chaves (id):            1  5  7  9 
                                     /   \  \  \  \
    . Vetor de descendentes (rrn):  0     3  5   6  7

    . O descendente que deve ser seguido é o de RRN igual a 3, pois o id buscado
    4 é maior que o id 1 e menor que o id 5, então será o desdendente entre essas 
    duas chaves
    */

    else{
        int pos_descendente;
        pos_descendente = busca_binariaDesc(ramos[ramo_atual],valor_buscado,0,qtd_chavesMax-1);
        return buscar_valorRamo(arq_arv,ramos,pos_descendente,valor_buscado);
    }
}

void escrever_cabecalhoArvore(FILE *arq_arv, Arvore_B *arvore){
    /* Função que escrever em um arquivo de índice os campos de uma estrutura Árvore-B* */
    
    // Checando validade dos ponteiros
    if(arq_arv == NULL || arvore == NULL) return;

    // Volta para o início do arquivo se já não estiver lá
    if(ftell(arq_arv) != 0){
        fseek(arq_arv, 0, SEEK_SET);
    }

    // Escrevendo status
    fwrite(&(arvore->status),sizeof(char),1,arq_arv);
    
    // Escrevendo os campos da estrutura
    fwrite(&(arvore->raiz),sizeof(int),1,arq_arv);
    fwrite(&(arvore->RRNproxNo),sizeof(int),1,arq_arv);
    fwrite(&(arvore->nroNiveis),sizeof(int),1,arq_arv);
    fwrite(&(arvore->nroChaves),sizeof(int),1,arq_arv);

    // Escrevendo lixo para completar o tamanho fixo de 76 bytes
    for(int i = 0; i < 59; i++){
        fwrite(enchimento,sizeof(char),1,arq_arv);
    }
}

bool ler_cabecalhoArvore(FILE *arq_indice, Arvore_B *arvore){
    /* 
    Função que lê o cabeçalho de um arquivo de índice, em Árvore-B*, e checa 
    sua consistência e, caso consistênte, lê as propriedades da árvore 
    */

    /**
     * return true;  --> foi possível ler (arquivo consistente, existe, etc)
     * return false; --> não foi possível ler (arquivo inconsistente, não existe, etc)
    */

    // Falha no ponteiro pro arquivo
    if(arq_indice == NULL){
        liberar_arvore(arvore);
        return false;
    }

    // Lendo status (1 byte)
    fread(&(arvore->status), sizeof(char), 1, arq_indice);
    
    // Checando consistência
    if(arvore->status == inconsistente){
        liberar_arvore(arvore);
        fclose(arq_indice);
        return false;
    }

    // Lendo propriedades da Árvore-B*
    fread(&(arvore->raiz), sizeof(int), 1, arq_indice);
    fread(&(arvore->RRNproxNo), sizeof(int), 1, arq_indice);
    fread(&(arvore->nroNiveis), sizeof(int), 1, arq_indice);
    fread(&(arvore->nroChaves), sizeof(int), 1, arq_indice);

    // Descartando lixo presente no início do arquivo
    char lixo[59];
    fread(lixo, sizeof(char), 59, arq_indice);

    return true;
}


/* Implementação das funções internas */

void inicializar_arvore(Arvore_B *arvore){
    /* Funcao que inicializa os campos de uma estrutura do tipo árvore-B* */

    /* 
        Usamos memset para zerar todos os campos da arvore e evitar que ocorram
        warnings do valgrind 
    */
    memset(arvore, 0, sizeof(Arvore_B));
   
    // Inicializando campos conforme as especificacoes do trabalho
    arvore->status = INCONSISTENTE;
    arvore->raiz = -1;
    arvore->RRNproxNo = 0;
    arvore->nroNiveis = 0;
    arvore->nroChaves = 0;
    arvore->ramos = NULL;
    
    /* 
        Alocando espaço para quantidade máxima de ramos permitido em memória primária 
        por vez (4), estabelecemos, por convenção, que a posicao 0 será usada para 
        armanezar o ramo atual, a posicao 1 para o ramo pai, a 2 para a irma esquerda,
        ou o novo no criado no split 2 pra 3, e a 3 para a irma direita

        Ex:
        arvore->ramos[4] = {ramo_atual,ramo_pai,ramo_irmaEsq,ramo_irmaDir}
    */

    // Alocando memória para o vetor de ramos e inicializando cada ramo
    arvore->ramos = malloc(4 * sizeof(No));
    for(int i = 0; i < 4; i++){
        inicializar_ramo(&arvore->ramos[i]);
        arvore->ramos[i].nivel = 0;
    }
}
void inicializar_ramo(No *meu_no){
    /* Funcao que inicializa as chaves e os descendentes de um ramo */

    // Todas as chaves recebem valor -1
    for(int i = 0; i < qtd_chavesMax; i++){
        meu_no->chaves[i].id = -1;
        meu_no->chaves[i].byteOffset = -1;
    }

    // Todas os descendentes recebem valor -1
    for(int i = 0; i < qtd_descendentesMax; i++){
        meu_no->descendentes[i] = -1;
    }

    // Zera quantidade de chaves
    meu_no->qtd_chaves = 0;
}

void split_1pra2(Arvore_B *arvore, Chave chave_nova, int novo_no){
    /* 
    Essa funcao faz o split 1 para 2 da Árvore-B* seguindo a seguinte logica:
    
    . Junto todas as chaves(as que estavam na raiz e a nova) em um vetor ordenado
    para saber em qual posicao cada chave vai ficar em qual lugar 
    . Coloco as duas primeiras chaves no ramo antigo, as duas ultimas no ramo novo, 
    e promovo a chave do meio para um novo ramo raiz.
    
    A manipulacao das chaves e ponteiros sera explicada mais abaixo
    */

    int ramo_raiz = ramo_atual;

    // Vetor que armazenara todas as chaves sendo 5 no total, a nova a ser inserida(1) e as antigas da raiz(4)
    Chave *vetor_chaves = criar_chaves(qtd_chavesMax+1);
    
    // Coloca no vetor_chaves todas as chaves do ramo raiz de forma ordenada
    for(int i = 0; i < qtd_chavesMax; i++){
        inserir_ordenadoChave(vetor_chaves,arvore->ramos[ramo_raiz].chaves[i], qtd_chavesMax+1);
    }

    // Insiro a nova chave no vetor chaves de maneira ordenada
    inserir_ordenadoChave(vetor_chaves, chave_nova, qtd_chavesMax + 1);

    /* 
       Os ponteiros no geral se mantém estáveis, acompanhando a chave em que estavam, 
       exceto quando há influência do novo ramo. Observamos que como o novo ramo 
       sempre será inserido á direita, o ponteiro do novo ramo irá ficar à direita 
       da nova chave que estamos inserindo.

       Ex: 
       . Dado um ramo raiz: p0 10 p1 15 p2 29 p3 37 p4 
       Desejamos inserir o valor 12 com o ponteiro p5
                                                  x  x
       
       . O ramo da esquerda ficara assim p0 10 p1 12 p5 (note o novo ponteiro a 
       direita do novo valor)

       . Novo ramo raiz ficara px 15 py(sendo x o numero da raiz original e y o 
       numero do novo bloco criado no split)
       
       . O ramo da direita: p2 29 p3 37 p4(note que se manteve os ponteiros da 
       direita e da esquerda deles)
       
       Obs: 
       . Se o ramo novo inserido for o que está sendo promovido, eu iria alterar 
       o primeiro ramo a direita dele no vetor original(p0 10 p1 15 p2 29 p3 37 p4)
       ou seja, alteraria o p3. 
       . Note que essa posicao que sera alterada é igual á posicao no ramo do novo 
       elemento inserido + 1 (se o elemento novo foi inserido na segunda posicao 
       do vetor total, a posicao do ponteiro sera o terceiro ponteiro do vetor total),
       isso é calculado abaixo e salvo em posicao_novo
    */
    int posicao_novo = busca_binariaValor(vetor_chaves, chave_nova.id, 0, qtd_chavesMax) + 1; 

    // Esse será o vetor contendo todos os ponteiros, incluindo o ponteiro novo ja na posicao correta
    int *vetor_desc = inserir_descVetor(arvore->ramos[ramo_raiz].descendentes, novo_no, posicao_novo, qtd_descendentesMax);

    /* 
    Tendo tudo pronto, agora inseriremos tudo na posicao correta. A primeira 
    metade dos elementos fica no ramo da esquerda 
    */
    int metade = (qtd_chavesMax+1) / 2;

    // Zero o ramo atual para apagar as informacoes antigas
    inicializar_ramo(&(arvore->ramos[ramo_raiz]));

    // O parametro 0 indica que comecamos do inicio do vetor e metade indica que iremos inserir metade(2) chaves
    inserir_vetor(&(arvore->ramos[ramo_raiz]), vetor_chaves, vetor_desc, 0, metade);

    // Promovo o elemento do meio, criando um novo ramo raiz
    inicializar_ramo(&arvore->ramos[ramo_pai]);
    arvore->ramos[ramo_pai].qtd_chaves = 1;
    inserir_ordenadoChave(arvore->ramos[ramo_pai].chaves, vetor_chaves[metade], qtd_chavesMax + 1);

    /* 
        Inserindo as chaves no novo ramo. Novamente inserimos metade chaves mas comecando 
        do vetor a partir da posicao metade+1(3) pois o elemento do meio ja foi inserido
    */
    inicializar_ramo(&(arvore->ramos[ramo_irmaDir]));
    inserir_vetor(&(arvore->ramos[ramo_irmaDir]), vetor_chaves, vetor_desc, metade+1, metade);

    // A irma direita, novo ramo criado, fica no mesmo nivel da raiz antiga
    arvore->ramos[ramo_irmaDir].nivel = arvore->ramos[ramo_raiz].nivel;
    
    // Coloco os ponteiros para os nós filhos
    arvore->ramos[ramo_pai].descendentes[0] = arvore->raiz; // Aponta para raiz antiga
    arvore->ramos[ramo_pai].descendentes[1] = arvore->RRNproxNo; // Aponta para novo no

    // Nova raiz um nivel acima da raiz anterior
    arvore->ramos[ramo_pai].nivel = arvore->ramos[ramo_raiz].nivel+1;

    // Atualizo o cabecalho da arvore
    arvore->raiz = arvore->RRNproxNo + 1; // RRN da nova raiz
    arvore->nroNiveis++;

    // Libero memoria alocada
    free(vetor_desc);
    free(vetor_chaves);
}
void split_2pra3(FILE *arq_arv, Arvore_B *arvore, Chave *chave_nova, int *rrn_novo, int rrn_atual, int rrn_pai){
    // Funcao responsavel por realizar o split_2pra3 da arvore b estrela
    // Achar posição da irmã que ocorerrá split 2-3
    int pos_noAtual = encontrar_posPonteiro(arvore->ramos,ramo_pai,rrn_atual);
    int irma_dir;
    
    // Caso o ramo atual for o descendente mais a direita de seu pai
    if(pos_noAtual == qtd_descendentesMax-1 || arvore->ramos[ramo_pai].descendentes[pos_noAtual+1] == -1){
        // Realiza split 2-3 com irmã da esquerda, o que equivale ao split 2-3 se a irmã da esquerda fosse o ramo atual
        // Tornando o ramo atual igual a sua irmã da esquerda
        
        // Muda o RRN do no atual para o da sua esquerda e o no da direita para o no atual

        irma_dir = rrn_atual;
        rrn_atual = arvore->ramos[ramo_pai].descendentes[--pos_noAtual];

        /* EXPLICAR */
        // Copiar ramo atual para ramo direito 
        copiar_ramo(&arvore->ramos[ramo_irmaDir],arvore->ramos[ramo_atual]);
        // Copia o ramo da esquerda para o ramo atual
        copiar_ramo(&arvore->ramos[ramo_atual],arvore->ramos[ramo_irmaEsq]);
    }
    else{
        // Caso normal, encontro leio a irma direita para a memoria
        irma_dir = arvore->ramos[ramo_pai].descendentes[pos_noAtual + 1];
        ler_ramoIndice(arq_arv,arvore->ramos,ramo_irmaDir,irma_dir);
    }

    // A posicao da chave no pai que liga os dois irmaos é numericamente igual a pos do no da esquerda, ou seja, o no atual
    int pos_noPai = pos_noAtual;

    /* 
        Criando um vetor de chaves que armazenará todas as chaves envolvidas no split 2-3 
        sendo 10 no total (2 nós cheios (4 + 4), chave pai (1), nova chave (1))
    */
    int qtd_chavesTotal = 2*(qtd_chavesMax) + 2;
    Chave *vetor_chaves = criar_chaves(qtd_chavesTotal);
    
    // Preenchendo o vetor de chaves ordenadamente
    // Inserimos no vetor chaves cada chave do nó que causou a distribuição
    for(int i = 0; i < qtd_chavesMax; i++){
        inserir_ordenadoChave(vetor_chaves,arvore->ramos[ramo_atual].chaves[i], qtd_chavesTotal);
    }

    // Inserimos no vetor chaves cada chave da irmã que participará da redistribuição
    for(int i = 0; i < qtd_chavesMax; i++){
        inserir_ordenadoChave(vetor_chaves,arvore->ramos[ramo_irmaDir].chaves[i], qtd_chavesTotal);
    }

    // Inserindo no vetor chaves a chave pai
    inserir_ordenadoChave(vetor_chaves,arvore->ramos[ramo_pai].chaves[pos_noPai], qtd_chavesTotal);

    // Inserindo no vetor chaves a nova chave inserida
    inserir_ordenadoChave(vetor_chaves,*chave_nova,qtd_chavesTotal);

    /* 
        Criando o vetor de descendentes de tamanho 11 (5 de cada ramo anterior 
        + 1 do descendente que vem junto da nova chave)
    */    
    int *vetor_descendentes;
    int qtd_descendentesTotal = qtd_descendentesMax*2 + 1;
    vetor_descendentes = malloc(qtd_descendentesTotal * sizeof(int));

    /* 
        Junta os descendentes dos dois ramos filhos no vetor descendentes, 
        colocando primeiro o esquerdo(atual) e depois o direito
    */
    unir_descendentes(vetor_descendentes,arvore->ramos[ramo_atual],arvore->ramos[ramo_irmaDir]);
    
    /* 
       Os ponteiros no geral se mantém estáveis, acompanhando a chave em que estavam, exceto quando
       há influência do novo nó. Observamos que como o novo nó sempre será inserido á direita, o ponteiro
       do novo nó irá ficar à direita da nova chave que estamos inserindo, por exemplo, temos uma raiz 
       p0 10 p1 15 p2 29 p3 37 p4 e vamos inserir o valor 12 com o ponteiro p5, o no da esquerda ficara
       assim p0 10 p1 12 p5(note o novo ponteiro a direita do novo valor), nova raiz ficara px 15 py(sendo x 
       o numero da raiz original e y o numero do novo bloco criado no split) e bloco da direita p2 29 p3 37 p4(
       note que se manteve os ponteiros da direita e da esquerda deles). P.s: Se o no novo inserido for o que
       sendo promovido, eu iria alterar o primeiro nó a direita dele no vetor original(p0 10 p1 15 p2 29 p3 37 p4)
       ou seja, alteraria o p3. Note que essa posicao que sera alterada é igual á posicao no nó do novo elemento
       inserido + 1(se o elemento novo foi inserido na segunda posicao do vetor total, a posicao do ponteiro sera
       o terceiro ponteiro do vetor total), isso é calculado abaixo e salvo em posicao_novo
    */
    int posicao_novo = busca_binariaValor(vetor_chaves, (*chave_nova).id, 0, (qtd_chavesTotal - 1)) + 1;
    int *temp;//Utilizado para nao perder o endereço de vetor_descentes e ocorrer leak de memoria
    
    //Insiro o novo descendente na posição calculada acima, shiftando os valoresnecessários para a direita
    temp = inserir_descVetor(vetor_descendentes, *rrn_novo, posicao_novo,qtd_descendentesTotal);
    free(vetor_descendentes);
    vetor_descendentes = temp;

    // Reorganizar e inserir chaves nos nós filhos
    // No split 2 pra 3 as chaves se dividem em tercos
    int terco = qtd_chavesTotal/3;

    // Zerando e inserindo no ramo atual
    inicializar_ramo(&arvore->ramos[ramo_atual]);
    // O 0 indica que comecamos a partir do inicio do vetor
    inserir_vetor(&arvore->ramos[ramo_atual],vetor_chaves,vetor_descendentes,0,terco);

    // Primeira promoção do split 2-3 para o ramo pai  
    arvore->ramos[ramo_pai].chaves[pos_noPai].id = vetor_chaves[terco].id;
    arvore->ramos[ramo_pai].chaves[pos_noPai].byteOffset = vetor_chaves[terco].byteOffset;

    // Zerando e inserindo o segundo terco (4 ate 6) no nó irmã
    inicializar_ramo(&arvore->ramos[ramo_irmaDir]);
    /* 
        O terco+1 indica que comecamos a partir do terco+1 do vetor, pois ja 
        inserimos o primeiro terco e teve a promocao da posicao terco(3)
    */
    inserir_vetor(&arvore->ramos[ramo_irmaDir],vetor_chaves,vetor_descendentes,terco+1,terco);

    /*
        Segunda promoção do split 2-3 para o nó pai sera realizada por meio de uma 
        nova insercao apos retornarmos dessa funcao, por enquanto so armazenamos os valores
    */
    (*chave_nova).id = vetor_chaves[2*terco+1].id;
    (*chave_nova).byteOffset = vetor_chaves[2*terco+1].byteOffset;
    /* 
        Salvo o RRN do novo no que sera criado abaixo para ser inserido junto da 
        segunda promocao apos o retorno do split
    */
    *rrn_novo = arvore->RRNproxNo;
    // Criando e inserindo o ultimo terco (8 e 9) no novo nó

    inicializar_ramo(&arvore->ramos[ramo_irmaEsq]);
    /* 
        O 2*terco+2 indica que comecamos a partir do 2*terco+2 do vetor, pois ja 
        inserimos o primeiro e o segundo terco e teve a promocao da posicao terco(3)
        e da poiscao 2 * terco + 1(7)
    */
    inserir_vetor(&arvore->ramos[ramo_irmaEsq],vetor_chaves,vetor_descendentes,2*terco+2,terco-1);
    // Nivel do novo no é o mesmo do no original que sofreu o split
    arvore->ramos[ramo_irmaEsq].nivel = arvore->ramos[ramo_atual].nivel;

    // Salvo os nos ja completos na memoria
    escrever_ramoArvore(arq_arv,arvore->ramos,rrn_atual,ramo_atual);
    escrever_ramoArvore(arq_arv,arvore->ramos,irma_dir,ramo_irmaDir);
    escrever_ramoArvore(arq_arv,arvore->ramos,*rrn_novo,ramo_irmaEsq);
    // Atualizo o cabecalho
    arvore->RRNproxNo++;

    // Libero memoria
    free(vetor_chaves);
    free(vetor_descendentes);
}
int busca_binariaValor(Chave *chaves, int valor_buscado, int esquerdo, int direito){
    /* Função que realiza a busca binária por algum valor */

    if(esquerdo > direito) return -1;
    
    int meio = (direito + esquerdo)/2;
    if(chaves[meio].id == valor_buscado){
        return meio;
    }
    else if(chaves[meio].id > valor_buscado){
        return busca_binariaValor(chaves,valor_buscado,esquerdo,(meio-1));
    }
    else{
        return busca_binariaValor(chaves,valor_buscado,(meio+1),direito);
    }
}

int busca_binariaDesc(No ramo, int valor_buscado, int esquerdo, int direito){
    /* Busca binária recursiva em busca do descendente */
    int meio = (direito + esquerdo)/2;
    // Significa que a busca binária chegou em um valor próximo ao que buscamos
    if(esquerdo >= direito){
        // Verificamos se o valor buscado estaria a esquerda ou direita do valor da busca
        if(valor_buscado < ramo.chaves[meio].id || ramo.chaves[meio].id == -1){
            /* 
                Se for menor, a posição do descendete será numericamente igual à posição
                final da busca do vetor(o equivalente ao descendente esquerdo)
            */
            return ramo.descendentes[meio];
        }
        // Caso contrário será o descendente direito, ou seja, a posição do valor no vetor + 1
        return ramo.descendentes[meio+1];
    }
    
    if(ramo.chaves[meio].id > valor_buscado){
        return busca_binariaDesc(ramo,valor_buscado,esquerdo,(meio-1));
    }
    else{
        return busca_binariaDesc(ramo,valor_buscado,(meio+1),direito);
    }
}

void inserir_ordenadoChave(Chave *chaves, Chave chave_nova, int tamanho){
    /* Funcao para inserir uma chave de forma ordenada em um vetor de chaves */

    int temp_id;
    long long int temp_byteOffset;
    for(int i = 0; i < tamanho; i++){
        // Se chegou em uma posicao vaga, apenas insere nela
        if(chaves[i].id == -1){
            chaves[i].id = chave_nova.id;
            chaves[i].byteOffset = chave_nova.byteOffset;
            break;
        }
        //Se e uma posicao nao vazia, troca os valores e continua tentando inserir a chave trocada
        if(chaves[i].id > chave_nova.id){

            temp_byteOffset = chaves[i].byteOffset;
            temp_id = chaves[i].id;

            chaves[i].id = chave_nova.id;
            chaves[i].byteOffset = chave_nova.byteOffset;
            
            chave_nova.id = temp_id;
            chave_nova.byteOffset = temp_byteOffset;
        } 
    }
}
void inserir_ordenadoDesc(No *ramos, int *desc_novo, int desc_pos){
    /* 
    Função que insere um ponteiro para algum descendente na posição especificada
    por 'desc_pos' no vetor de descendentes de um dado ramo
    */
    int desc_temp;
    for(int i = desc_pos+1; i < qtd_descendentesMax; i++){
        // Dando 'shift' nos outros descendentes do vetor
        desc_temp = ramos[ramo_atual].descendentes[i];
        ramos[ramo_atual].descendentes[i] = *desc_novo;
        *desc_novo = desc_temp;
    } 
}
void inserir_chave(No* ramos, Chave *chave_nova, int *rrn_novo){
    /* 
    Função que insere uma chave nova e um ponteiro para descendente, se for
    especificado, em um dado ramo 
    */

    // Inserindo a chave na posição correta no ramo
    inserir_ordenadoChave(ramos[ramo_atual].chaves, *chave_nova, qtd_chavesMax);
    
    // Inserindo o ponteiro na posição correta no ramo
    int desc_pos = busca_binariaValor(ramos[ramo_atual].chaves, (*chave_nova).id, 0, (qtd_chavesMax - 1));
    inserir_ordenadoDesc(ramos, rrn_novo, desc_pos);

    // Incrementando quantidade de chaves no ramo
    ramos[ramo_atual].qtd_chaves++;   
}

void inserir_vetor(No *rrn_atual, Chave *chaves, int *vet_desc, int inicio, int qtd){
    /*
    Funcao que preenche um ramo de uma Árvore-B*, inserindo as chaves e os descendentes,
    providos por um vetor de chaves e de descendentes
    
    Parâmetros:
    . rrn_atual : ramo em que será inserido
    . chaves   : vetor de chaves
    . vet_desc : vetor de descendentes
    . qtd      : quantidae de chaves que serão inseridos no ramo
    . inicio   : a partir de qual índice vamos percorrer o vetor de chaves e de descendentes
    */

    for(int i = 0; i < qtd; i++){
        // Inserindo chaves
        rrn_atual->chaves[i].id          = chaves[i + inicio].id;
        rrn_atual->chaves[i].byteOffset  = chaves[i + inicio].byteOffset;
        
        // Inserindo descendentes
        rrn_atual->descendentes[i]       = vet_desc[i + inicio];
    }
    // Inserindo último descendente
    rrn_atual->descendentes[qtd] = vet_desc[qtd + inicio];

    // Atualizando quantidade de chaves do ramo 
    rrn_atual->qtd_chaves = qtd;
}
int inserir_chaveNo(FILE *arq_arv, Arvore_B *arvore, Chave *chave_nova, int *rrn_novo, int rrn_atual, int rrn_pai){
    /*
    Função recursiva responsável por percorrer a árvore do arquivo de índice em busca
    do ramo ideal para inserir a nova chave. No total, essa função trata 4 cenários
    possíveis:
    . Consegue inserir
    . Redistribuição
    . Split 1 pra 2
    . Split 2 pra 3
    */

    
    /*
    . Função 'ler_ramoIndice' traz um ramo especificado de um arquivo de índice em
    Árvore-B* para memória primária. 
    . É necessário esse 'if', pois como a função é recursiva, nem sempre será 
    necessário ler outro ramo do arquivo e trazer para memória primária
    */ 
    if( (arvore->RRNproxNo > rrn_atual) && (arvore->nroChaves != 0) && (*rrn_novo == -1) ){
        ler_ramoIndice(arq_arv,arvore->ramos,ramo_atual,rrn_atual);
    }
    

    /*

    A inserção segue o seguinte esquema:
    
    . Um ramo é trazido do arquivo de índice para memória primária
    
    . Caso seja um ramo folha: tentamos inserir a nova chave, se houver espaço,
    inserimos a nova chave (respeitando as regras de ordem e afins) e escrevemos 
    no arquivo de índice no local correto. Caso não haja espaço, poderá haver
    redistribuição, split 1 pra 2 e split 2 pra 3 
    
    . Caso não seja um ramo folha: a função entrará em recursão até que um ramo 
    ideal seja trazido para memória primária para a inserção da nova chave 
    
    Obs: no caso de ocorrência de redistribuição, split 1 pra 2, ou split 2 pra 3,
    será necessário haver mais de um ramo em memória primária, porém desenvolvemos
    um algoritmo em que nunca há mais de 4 ramos simultaneamente em memória primária

    */

    
    // Verificando se o ramo é folha
    if(arvore->ramos[ramo_atual].nivel == 1 || *rrn_novo != -1){
        // Verifica se há espaço no ramo
        if(arvore->ramos[ramo_atual].qtd_chaves < qtd_chavesMax){
            
            // Caso haja espaço, insere a chave nova no ramo
            inserir_chave(arvore->ramos,chave_nova,rrn_novo);
            /*
            Escreve no arquivo de índice, no rrn especificado por 'rrn_atual', o
            ramo presente no vetor de ramos da árvore, na posição 'ramo_atual'
            */
            escrever_ramoArvore(arq_arv,arvore->ramos,rrn_atual,ramo_atual);

            return inseriu;
        }
        /* 
        Caso não haja espaço no ramo folha, tentamos realizar a redistribuição uniforme como
        especificado pela descrição do trabalho
        */
        else{            
            /*
            Verificamos se há redistribuição é possível (há espaço em alguma das 
            irmãs do ramo atual, priorizando a irmã da esquerda)
            */
            int rrn_irmaViavel = redistribuicao_possivel(arq_arv,arvore->ramos,rrn_pai,rrn_atual);
    
            // Caso seja possível, 'rrn_irmaViavel' armazena o RRN da irma que participará da redistribuição
            if(rrn_irmaViavel != -42){
                /*
                Ocorre a redistribuição e armazenamos em qual posição no vetor de 
                ramos está o ramo da irma que deve ser escrita no arquivo de índice 
                */
                int pos_ramoIrma = redistribuir(arvore->ramos,*chave_nova,rrn_novo,rrn_pai,rrn_atual,rrn_irmaViavel);
                
                // Escrevendo no arquivo de índice todos os ramos que participaram da redistribuição
                escrever_ramoArvore(arq_arv,arvore->ramos,rrn_atual,ramo_atual);
                escrever_ramoArvore(arq_arv,arvore->ramos,rrn_pai,ramo_pai);
                escrever_ramoArvore(arq_arv,arvore->ramos,rrn_irmaViavel,pos_ramoIrma);

                return inseriu;
            }
            // Caso a redistribuição não seja possível (ambas as irmãs cheias), realizararemos algum dos splits
            else{
                // Split 1-2
                if(rrn_atual == arvore->raiz){
                    split_1pra2(arvore, *chave_nova, *rrn_novo);

                    // Escrevendo no arquivo de índice todos os ramos que participaram do split
                    escrever_ramoArvore(arq_arv,arvore->ramos,rrn_atual,ramo_atual);
                    escrever_ramoArvore(arq_arv,arvore->ramos,arvore->RRNproxNo++,ramo_irmaDir);
                    escrever_ramoArvore(arq_arv,arvore->ramos,arvore->RRNproxNo++,ramo_pai);

                    return inseriu;
                }
                
                // Split 2-3
                split_2pra3(arq_arv,arvore, chave_nova, rrn_novo, rrn_atual, rrn_pai);
                copiar_ramo(&arvore->ramos[ramo_atual],arvore->ramos[ramo_pai]);

                return nao_inseriu;
            }
        }
    }

    // Buscando RRN do próximo ramo em que se tentará inserir a nova chave
    int rrn_proxRamo = busca_binariaDesc(arvore->ramos[ramo_atual], (*chave_nova).id,0,qtd_chavesMax-1);

    /*
    Visto que em alguns casos necessitamos do ramo pai do ramo atual, e, como o
    o ramo atual é o pai do próximo ramo a ser analisado, armazenamos o ramo atual
    como ramo pai antes de continuar na recursão 
    */
    copiar_ramo(&arvore->ramos[ramo_pai], arvore->ramos[ramo_atual]);
    
    // Chamada recursiva da função
    int flag = inserir_chaveNo(arq_arv,arvore, chave_nova, rrn_novo, rrn_proxRamo, rrn_atual);
    
    // Finalização do Split 2-3 
    if(flag == nao_inseriu){
        /*
        Como no split 2-3, duas chaves são "promovidas", nosso algoritmo promove
        uma das chaves dentro a lógica do split 2-3 e chama a função 'inserir_chaveNo'
        novamente para se inserir a outra chave "promovida", isso, pois, essa promoção
        e como uma inserção normal, podendo ocorrer outro split1-2, split 2-3, ou 
        redistribuição
        */
        ler_ramoIndice(arq_arv,arvore->ramos,ramo_pai,rrn_pai);
        flag = inserir_chaveNo(arq_arv,arvore, chave_nova, rrn_novo, rrn_atual, rrn_pai);
    }

    return flag;
}
int *inserir_descVetor(int *vet_antigoDesc, int novo_desc, int posicao, int tamanho){
    /* 
    Função que cria um novo vetor de descendentes (lista de inteiros correspondente a RRNs),
    copiando todos os valores de um vetor antigo e adicionando um novo em alguma 
    posição especificada
    */
    
    // Alocando memória para o novo vetor
    int *vet_novoDesc = malloc((tamanho+1) * sizeof(int));

    // Copiando o vetor de descendentes antigo no vetor de descendentes novo
    copia_vetor(vet_novoDesc, vet_antigoDesc, tamanho);

    int temp;
    for(int i = posicao; i < (tamanho+1); i++){
        temp = vet_novoDesc[i];
        vet_novoDesc[i] = novo_desc;
        novo_desc = temp;
    }

    return vet_novoDesc;
}

int encontrar_irmaViavel(FILE *arq_arv, No *ramos, int rrn_pai, int pos_noAtual){
    /* 
    Obtendo ramo da irmã "viável", a irmã viável será:
        . caso ramo possua ambas as irmãs (direita e esquerda) -> irmã que possuir 
        espaço, priorizando a esquerda (especificação do trabalho)
        . caso ramo possua apenas irmã esquerda -> irmã esquerda, se possuir espaço livre
        . caso ramo possua apenas irmã direita -> irmã direita, se possuir espaço livre
    */
   
    int rrn_irmaViavel = -42;

    // Irmã esquerda está uma posição a esquerda do ramo atual no vetor
    int pos_irmaEsq = pos_noAtual - 1; 

    /* 
    Posição da irmã esquerda deve ser >= 0, caso contrário seu índice será negativo,
    o que corresponde a estar fora do vetor (ocorre quando oramo atual está no início 
    do vetor) 
    */
    if(pos_irmaEsq >= 0 && ramos[ramo_pai].descendentes[pos_irmaEsq] != -1){
        // Armazenando ramo da irmã da esquerda em memória primária
        ler_ramoIndice(arq_arv,ramos,ramo_irmaEsq,ramos[ramo_pai].descendentes[pos_irmaEsq]);

        // Testando se a irmã viável é a da esquerda (ramo não pode estar cheio)
        if(!no_estaCheio(ramos, ramo_irmaEsq)){
            rrn_irmaViavel = ramos[ramo_pai].descendentes[pos_irmaEsq];
            return rrn_irmaViavel;
        }
    }

    // Irmã direita está uma posição a direita do ramo atual no vetor
    int pos_irmaDir = pos_noAtual + 1; 

    /*
    Posição da irmã direita deve ser < qtd_descendentesMax (5), caso contrário seu índice
    ultrapassará o tamanho do vetor, o que corresponde a estar fora do vetor (Caso 
    em que o ramo atual está no final do vetor)
    */
    if(pos_irmaDir < qtd_descendentesMax && ramos[ramo_pai].descendentes[pos_irmaDir] != -1){
        // Armazenando ramo da irmã da direita em memória primária
        ler_ramoIndice(arq_arv,ramos,ramo_irmaDir,ramos[ramo_pai].descendentes[pos_irmaDir]);
        
        // Testando se a irmã viável é a da direita
        if(!no_estaCheio(ramos, ramo_irmaDir)){
            rrn_irmaViavel = ramos[ramo_pai].descendentes[pos_irmaDir];
            return rrn_irmaViavel;
        }
    }

    // Caso nenhuma das irmãs seja viável, retorna -42 (flag genérica utilizada)
    return rrn_irmaViavel;
}
int encontrar_posPonteiro(No *ramos, int rrn_pai, int rrn_atual){
    /* 
    Função utilitária que adquire a posição do ponteiro para o ramo atual no vetor d
    e descendentes do ramo pai 
    Ex:                                       x
        ramos[rrn_pai].descendentes[i] = [2 , 3 , 4 , -1 , -1]
        rrn_atual = 3
        pos_noAtual = 1 (índice 1 no vetor de descendentes do ramo pai)
    */

    for(int i = 0; i < qtd_descendentesMax; i++){
        if(ramos[rrn_pai].descendentes[i] == rrn_atual){
            return i;
        }
    }

    // Retorna -1 caso não encontre o 'rrn_atual' no vetor de descedentes do ramo pai
    return -1;
}

int redistribuir(No *ramos, Chave chave_nova, int *descendente_novo, int rrn_pai, int rrn_atual, int rrn_irmaViavel){
    /* Funcao resposável por realizar a redistribuição em uma Árvore-B* */

    // Encontro a posicao do ponteiro para o ramo atual no pai dele
    int pos_noAtual;
    pos_noAtual = encontrar_posPonteiro(ramos,ramo_pai,rrn_atual);
    
    // Checando posição da irmã em relação ao ramo atual, pelos ponteiros do no pai
    int pos_irma;
    pos_irma = encontrar_posPonteiro(ramos,ramo_pai,rrn_irmaViavel);
    
    /*
        ramo_esq guardara qual o no da esquerda, ramo_dir o da direita, pos_pai qual a chave 
        do pai que liga as duas irmas e ramo_posIrma em qual ramo salvamos a irma na memoria
    */
    int ramo_esq, ramo_dir, pos_pai, ramo_posIrma;


    /* Descobrindo se estamos em uma redistribuicao para a esquerda ou direita */

    // Redistribuicao para a esquerda pois a posicao da irma é menor
    if(pos_irma < pos_noAtual){
        // A posição da chave pai no vetor de chaves é numericamente igual á posição da irma no vetor de descendentes
        ramo_posIrma = ramo_irmaEsq;
        ramo_esq = ramo_posIrma;
        ramo_dir = ramo_atual;
        pos_pai = pos_irma;
    }
    // Redistribuicao para a direita pois a posicao da irma é maior
    else{
        // A posição da chave pai no vetor de chaves é numericamente igual á posição atual no vetor de descendentes
        ramo_posIrma = ramo_irmaDir;
        ramo_esq = ramo_atual;
        ramo_dir = ramo_posIrma;
        pos_pai = pos_noAtual;
    }

    
    /* Obtendo vetor com todas as chaves envolvidas na redistribuição*/

    // Obtendo quantidade de chaves do rrn_atual, e irma_viável
    int qtd_ChaveIrma = ramos[ramo_posIrma].qtd_chaves;
    int qtd_ChaveNoAtual = ramos[ramo_atual].qtd_chaves;
    int qtd_ChavesTotal = qtd_ChaveNoAtual + qtd_ChaveIrma + 2; // (2, pois é a chave do ramo pai + a ser inserida)
    
    // Criando um vetor de chaves que armazenará todas as chaves envolvidas na redistribuição
    Chave *vetor_chaves = criar_chaves(qtd_ChavesTotal);
    
    // Inserimos ordenadamente no vetor_chaves cada chave do ramo que causou a redistribuição (ramo_atual)
    for(int i = 0; i < qtd_ChaveNoAtual; i++){
        inserir_ordenadoChave(vetor_chaves, ramos[ramo_atual].chaves[i], qtd_ChavesTotal);
    }
    // Inserimos ordenadamente no vetor chaves cada chave da irmã que participará da redistribuição
    for(int i = 0; i < qtd_ChaveIrma; i++){
        inserir_ordenadoChave(vetor_chaves,ramos[ramo_posIrma].chaves[i], qtd_ChavesTotal);
    }
    // Inserindo no vetor chaves a chave pai
    inserir_ordenadoChave(vetor_chaves,ramos[ramo_pai].chaves[pos_pai], qtd_ChavesTotal);
    // Inserindo no vetor chaves nova chave
    inserir_ordenadoChave(vetor_chaves, chave_nova, qtd_ChavesTotal);


    /* 
       . Preenchendo o vetor de descendentes que conterá todos os descendentes ligados 
       às chaves dos nós filhos que participam da redistribuição
       . Como cada ramo tem no máximo 5 descendentes, e para que a redistribuição 
       ocorra no máximo 9 deles estão ocupados, alocamos 10 (9 + novo descendente) 
       de espaço na memória
    */
    int *vetor_descendentes;
    vetor_descendentes = malloc((qtd_descendentesMax*2) * sizeof(int));

    // Junto os descendentes do ramo esquerdo e do ramo direito no vetor de descendentes
    unir_descendentes(vetor_descendentes,ramos[ramo_esq],ramos[ramo_dir]); 
    
    /* 
    . Encontramos a posição onde ficará o novo descendente como sendo a posicao 
    da nova chave no vetor de chaves +1, isso se dá pois como sempre criamos um 
    novo ramo à direita no split, o novo descendente viria à direita da nova chave
    
    Ex:
    chaves:         5  15 -1 -1
    descendentes   0  3  4  -1 -1         

    . É visto que o descendente à direita do 5(o 3) está na posição 1 do vetor 
    descendetes, o mesmo que a posição do 5(pos 0) + 1
    */

    // Encontramos a posicao em que sera inserido o novo descendente
    int posicao_novo = busca_binariaValor(vetor_chaves, chave_nova.id, 0, (qtd_ChavesTotal-1) ) + 1;
    int *temp; //Utilizado para nao perder o endereço de vetor_descentes e ocorrer leak de memoria
    
    //Insiro o novo descendente na posição calculada acima, shiftando os valores necessários para a direita
    temp = inserir_descVetor(vetor_descendentes, *descendente_novo, posicao_novo,qtd_descendentesMax*2);
    free(vetor_descendentes);
    vetor_descendentes = temp;

    /*  
        Tendo ambos os vetores de chaves e descendentes, agora resetar valores do 
        ramo atual e da irma viável para adicionar todos os valores de forma redistribuida 
    */
    
    // Resetando os ramos
    inicializar_ramo(&ramos[ramo_posIrma]);
    inicializar_ramo(&ramos[ramo_atual]);
    
    // Inserindo a primeira metade no ramo esquerdo e metade no ramo direito
    int metade = qtd_ChavesTotal / 2;
    inserir_vetor(&ramos[ramo_esq], vetor_chaves, vetor_descendentes, 0, metade);

    // Se quantidade de chaves for par o ramo da direita fica com uma chave a menos
    if(qtd_ChavesTotal % 2 == 0){ 
        inserir_vetor(&ramos[ramo_dir], vetor_chaves, vetor_descendentes, metade+1, metade-1);
    }
    // Caso for ímpar, ambos os nós ficam com quantidades iguais de chaves
    else{ 
        inserir_vetor(&ramos[ramo_dir], vetor_chaves, vetor_descendentes, metade+1, metade);
    }

    // A chave que substituirá a chave antiga do pai será a chave do meio do vetor
    ramos[ramo_pai].chaves[pos_pai].id = vetor_chaves[metade].id;
    ramos[ramo_pai].chaves[pos_pai].byteOffset = vetor_chaves[metade].byteOffset;
    
    // Desalocando memória utilizada
    free(vetor_chaves);
    free(temp);

    // Retorno utilizado para saber qual ramo salvar no arquivo arvore apos retorno da funcao
    return ramo_posIrma;
}
int redistribuicao_possivel(FILE *arq_arv, No *ramos, int rrn_pai, int rrn_atual){
    /* 
    Função responsável por indicar se é possível realizar uma redistribuição, dado
    a configuração atual da árvore
    */
    
    /* 
    Caso o RRN do ramo atual seja igual ao do ramo pai, significa que estamos na
    raiz da árvore, e não é possível realizar redistribuição nesse caso
    */
    if(rrn_pai == rrn_atual){
        return -42;
    }

    // Encontro a posição do ramo atual no vetor de descendentes do pai
    int pos_noAtual;
    pos_noAtual = encontrar_posPonteiro(ramos,ramo_pai,rrn_atual);
    
    // Tento verificar a existência de uma irmã viável, retorno seu RRN ou -42 se nao houver
    int rrn_irmaViavel;
    rrn_irmaViavel = encontrar_irmaViavel(arq_arv,ramos,ramo_pai,pos_noAtual);

    return rrn_irmaViavel;
}

Chave *criar_chaves(int tamanho){
    /* Funcao utlizada para alocar um vetor de chaves de algum tamanho especificado */
    
    Chave *minha_chave = (malloc(tamanho * sizeof(Chave)));
    
    for(int i = 0; i < tamanho; i++){
        // Inicializando 'id' para -1 a fim de facilitar na logica da execucao
        minha_chave[i].id = -1;
    }

    return minha_chave;
}
bool no_estaCheio(No *ramos, int pos_ramo){
    /* Funcao que verifica se o vetor de chaves de um dado ramo esta cheio */
    if(ramos[pos_ramo].qtd_chaves == qtd_chavesMax){
        return true;
    }
    return false;
}
void unir_descendentes(int *descendentes, No ramo_esq, No ramo_dir){
    /* 
    Função utilizada para unir dois vetores de descendentes, de dois ramos 
    providos, em um unico vetor
    */

    int i,j;

    // Primeiro se insere os descendentes do ramo da esquerda
    for(i = 0; i < qtd_descendentesMax; i++){
        // Caso chegue em um espaço não preenchido, se encerra a inserção
        if(ramo_esq.descendentes[i] == -1){
            break;
        }
        descendentes[i] = ramo_esq.descendentes[i];
    }

    // Após a inserção do ramo da esquerda, se insere os descendetes do ramo da direita 
    for(j = 0; j < qtd_descendentesMax; j++){
        if(ramo_dir.descendentes[j] == -1){
            break;
        }
        descendentes[i + j] = ramo_dir.descendentes[j];
    }

    // Os espaços do vetor de descendentes que não tenham sido preenchidos, seta como -1
    for(int w = i + j; w < qtd_descendentesMax * 2; w++){
        descendentes[w] = -1;
    }
}
void copiar_ramo(No *destino, No fonte){
    /* Função utilitária que copia todo um ramo fonte, em um ramo destino*/
    
    // Copiando chaves
    for(int i = 0; i < qtd_chavesMax; i++){
        destino->chaves[i].id = fonte.chaves[i].id;
        destino->chaves[i].byteOffset = fonte.chaves[i].byteOffset;
    }

    // Copiando descendentes
    for(int i = 0; i < qtd_descendentesMax; i++){
        destino->descendentes[i] = fonte.descendentes[i];
    }

    // Copiando propriedades
    destino->nivel = fonte.nivel;
    destino->qtd_chaves = fonte.qtd_chaves; 
}
void ler_ramoIndice(FILE *arq_arv, No *ramos, int pos_ramo, int RRN){
    /* 
        Funcao para ler um ramo da arvore do binario e salva-lo em memória primária.
        Decidimos onde colocar no vetor de ramos via 'pos_ramo' e encontramos o ramo 
        no arquivo via seu RRN
    */

    /* 
    Pelo código, usaremos a seguinte expressão matemática (RRN+1 * 76) para 
    encontrar o RRN de algum ramo
    */
   
    // Caso o ponteiro do arquivo não esteja no local correto, realiza um fseek
    if((RRN+1 * 76) != ftell(arq_arv)){
        fseek(arq_arv,(RRN+1)*76,SEEK_SET);
    }
    
    // Resetando os valores
    inicializar_ramo(&ramos[pos_ramo]);

    // Armazenando dados do ramo
    fread(&(ramos[pos_ramo].nivel),sizeof(int),1,arq_arv);
    fread(&(ramos[pos_ramo].qtd_chaves),sizeof(int),1,arq_arv);
    int i;
    for(i = 0; i < qtd_chavesMax; i++){
        fread(&(ramos[pos_ramo].descendentes[i]),sizeof(int),1,arq_arv);
        fread(&(ramos[pos_ramo].chaves[i].id),sizeof(int),1,arq_arv);
        fread(&(ramos[pos_ramo].chaves[i].byteOffset),sizeof(long long int),1,arq_arv);
    }
    fread(&(ramos[pos_ramo].descendentes[i]),sizeof(int),1,arq_arv);
}
void escrever_ramoArvore(FILE *arq_arv, No *ramos, int RRN ,int ramo_pos){
    /* 
    Função que escreve um dado ramo no arquivo de índice, em uma posição especificada
    por um RRN
    */

    // Verifico se ja nao estamos na posicao desejada (evitar fseek desnecessário)
    if(RRN + 1 * 76 != ftell(arq_arv)){
        fseek(arq_arv,(RRN+1)*76,SEEK_SET);
    }
    
    // Escrevo campo a campo do ramo
    fwrite(&(ramos[ramo_pos].nivel),sizeof(int),1,arq_arv);
    fwrite(&(ramos[ramo_pos].qtd_chaves),sizeof(int),1,arq_arv);
    for(int j = 0; j < qtd_chavesMax; j++){
        fwrite(&(ramos[ramo_pos].descendentes[j]),sizeof(int),1,arq_arv);
        fwrite(&(ramos[ramo_pos].chaves[j].id),sizeof(int),1,arq_arv);
        fwrite(&(ramos[ramo_pos].chaves[j].byteOffset),sizeof(long long int),1,arq_arv);
    }
    fwrite(&(ramos[ramo_pos].descendentes[4]),sizeof(int),1,arq_arv);

    // Após ramo ser escrito no arquivo, ele é resetado para próxima utilização
    inicializar_ramo(&ramos[ramo_pos]);
}
