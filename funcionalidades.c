#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliares.h"
#include "manipulacao.h"

//Funcionalidade [8]
void create_index(){

    //Aloca os tipos que serão usados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    Arvore_t *arvore = alocar_arvore();
    erro(arvore);

    //Faz a leitura dos inputs
    ler_nome_arq_dados(arq_dados);
    ignorar_palavra_stdin();
    ignorar_palavra_stdin();
    ler_nome_arvore(arvore);

    //Com os inputs armazenados, faço 
    //a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "rb");
    iniciar_arvore(arvore, "w+b");

    //Ler o cabeçalho do arquivo de dados
    ler_cabecalho_dados(arq_dados);

    //Verificar consistência dos dados do arquivo
    if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    alterarStatusArvore(arvore, 0);
    setCabecalhoArvoreNulo(arvore);
    escreverCabecalhoArvore(arvore);


    //A primeira chave que vou inserir, deve ser feita manualmente, pois
    //a busca do processaRegistros não pode ser executada em uma árvore vazia

    InfoDados_t *dado_inserir_arqDados;
    InfoInserida_t *dado_inserir_arvore;

    int cont = 0;//contador de registros lidos
    long int byteOffset_atual = getTamCabecalhoDados(arq_dados); //contador de byteOffset atual
    int tam_reg; //tamanho do registro lido
    int qtd_leituras = get_nroRegTotal(arq_dados);

    

    //Procura um registro válido para inserir na árvore e criar a raiz
    //como não se deve inserir registros removidos logicamente, procura-se um válido
    if(qtd_leituras>0){//
        do{
            dado_inserir_arqDados = ler_dados_registro(lerRegDoArqDados, arq_dados, &tam_reg);
            cont++;
        }while(cont < qtd_leituras && !validaInfoDados(dado_inserir_arqDados));

        //Inserir na árvore
        dado_inserir_arvore = criar_InfoInserida(arq_dados,dado_inserir_arqDados,byteOffset_atual);
        byteOffset_atual += tam_reg;
        insercao_arvore(arvore, NULL, NULL, dado_inserir_arvore);

        desalocar_InfoDados(dado_inserir_arqDados);
        desalocar_InfoInserida(dado_inserir_arvore);
    }

    //Agora, insiro na árvore usando a busca do processa registros.

    //Configuro a struct de ações para a inserção
    FncAcoes *acoes = alocar_acoes();
    set_acoes(acoes, NoOpAcaoRegArv, NoOpAcaoRegSeq, insercao_arvore, NoOpAcaoFinal);

    for(; cont < qtd_leituras; ++cont){
        dado_inserir_arqDados = ler_dados_registro(lerRegDoArqDados, arq_dados, &tam_reg);
        if(validaInfoDados(dado_inserir_arqDados)){
            //se o registro é válido, então insiro no árvore
            dado_inserir_arvore = criar_InfoInserida(arq_dados,dado_inserir_arqDados,byteOffset_atual);
            processaRegistros(arq_dados,arvore,dado_inserir_arqDados,acoes,dado_inserir_arvore);

            desalocar_InfoDados(dado_inserir_arqDados);
            desalocar_InfoInserida(dado_inserir_arvore);
        }
        byteOffset_atual += tam_reg;

    }

    //reescrevo o cabecalho da arvore agora com status consistente
    reiniciarCursorArvore(arvore);
    alterarStatusArvore(arvore,1);
    escreverCabecalhoArvore(arvore); 

    //Fechar arquivos
    fechar_arvore(arvore);
    fechar_arq_dados(arq_dados);

    binarioNaTela(getNomeArvore(arvore));

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_Arvore(arvore);
}

//Funcionalidade [9]
void where(void){
    //Aloca os tipos que serão usados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    Arvore_t *arvore = alocar_arvore();
    erro(arvore);
    int n;

    //Faz a leitura dos inputs
    ler_nome_arq_dados(arq_dados);
    ignorar_palavra_stdin();//Ignoro a especificação do 'campoIndexado'. Sempre será usado o id.
    ignorar_palavra_stdin();//Ignoro a especificação do 'tipoDado'. Sempre será usado o int.
    ler_nome_arvore(arvore);
    scanf("%d",&n);

    //Com os inputs armazenados, faço a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "rb");
    iniciar_arvore(arvore, "rb");

    //Ler o cabeçalho dos arquivos
    ler_cabecalho_arvore(arvore);
    ler_cabecalho_dados(arq_dados);

    //Testo se os dois arquivos estao consistentes. Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusArvore(arvore)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    /*Declaro a struct com todas as ações necessárias para essa funcionalidade. 
    Se algum tipo não for necessário, passo uma função NoOp daquele tipo.*/

    FncAcoes *acoes = alocar_acoes();
    set_acoes(acoes, validaPrinta, printa_busca, NoOpAcaoBranch, achouReg);
    //Loop que faz n buscas
    for(int i=1; i<=n; i++){
        printf("Resposta para a busca %d\n",i);

        //Ler os critérios de busca
        InfoDados_t *criterios = ler_criterios_busca();

        /*Processar o registro usando a ação 'printa_busca'
        e o final 'achouReg', que diz se o registro é inexistente, 
        caso nenhum satisfaça os critérios de busca*/
        
        //MUDAR
        processaRegistros(arq_dados,arvore,criterios,acoes, NULL);

        //Desalocar crtérios de busca    	
        desalocar_InfoDados(criterios);
    }
    //Fechar arquivos
    fechar_arvore(arvore);
    fechar_arq_dados(arq_dados);

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_Arvore(arvore);
    desalocar_acoes(acoes);
}

//Funcionalidade [10]
void insert_into(){
    //Alocar tipos utilizados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    Arvore_t *arvore = alocar_arvore();
    erro(arvore);

    //Ler entrada de dados
    ler_nome_arq_dados(arq_dados);
    ignorar_palavra_stdin();//Ignoro a especificação do 'campoIndexado'. Sempre será usado o id.
    ignorar_palavra_stdin();//Ignoro a especificação do 'tipoDado'. Sempre será usado o int.
    ler_nome_arvore(arvore);

    //abrir arquivos
    iniciar_arvore(arvore, "r+b");
    abrir_arq_dados(arq_dados, "r+b");

    //Ler os cabeçalhos
    ler_cabecalho_arvore(arvore);
    ler_cabecalho_dados(arq_dados);

    //Testo se os dois arquivos estao consistentes. Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusArvore(arvore)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    //Indico que os arquivos estão inconsistentes, pois posso escrever neles.
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,0);
    escreverStatusDados(arq_dados);

    reiniciarCursorArvore(arvore);
    alterarStatusArvore(arvore, 0);
    escreverStatusArvore(arvore);

    //Como vou escrever os dados no final, devo levar o cursor para o final
    levaFinalCursorDados(arq_dados);

    int qtdInserir;
    scanf(" %d", &qtdInserir);

    FncAcoes *acoes = alocar_acoes();
    set_acoes(acoes, NoOpAcaoRegArv, NoOpAcaoRegSeq, insercao_arvore, NoOpAcaoFinal);

    InfoDados_t *dado_inserir_arqDados;
    InfoInserida_t *dado_inserir_arvore;

    int tam_reg; //tamanho do registro lido
    long int byteOffset; //guarda o byteOffset do registro que vou inserir
    
    if(arvore_vazia(arvore)){
        //Se a árvore está vazia, então a busca executada pelo processaRegistros não pode ser efetuada.
        //Desse modo, a inserção deve ser feita de maneira manual.

        //Ler os dados que serão inseridos.
        dado_inserir_arqDados = ler_dados_registro(lerRegDoStdin,arq_dados, &tam_reg);

        //Inserir no arquivo de dados
        insercao_arqDados(arq_dados, dado_inserir_arqDados);

        //Inserir na árvore
        byteOffset = getProxByteOffSet(arq_dados);
        dado_inserir_arvore = criar_InfoInserida(arq_dados,dado_inserir_arqDados,byteOffset);
        insercao_arvore(arvore, NULL, NULL, dado_inserir_arvore);

        desalocar_InfoDados(dado_inserir_arqDados);
        desalocar_InfoInserida(dado_inserir_arvore);

        /*Reduzo em 1 a quantidade que deve ser inserida, 
        para que o loop de inserções não execute uma vez a mais*/
        qtdInserir--;
    }

    //Loop que faz as inserções usando a busca do processaRegistros
    for(int i=1; i<=qtdInserir; i++){
        //Ler os dados que serão inseridos.
        dado_inserir_arqDados = ler_dados_registro(lerRegDoStdin,arq_dados, &tam_reg);

        //Inserir no arquivo de dados
        byteOffset = getProxByteOffSet(arq_dados);
        insercao_arqDados(arq_dados, dado_inserir_arqDados);

        //Inserir na árvore
        dado_inserir_arvore = criar_InfoInserida(arq_dados, dado_inserir_arqDados,byteOffset);
        processaRegistros(arq_dados, arvore, dado_inserir_arqDados, acoes, dado_inserir_arvore);

        //Desalocar crtérios de busca
        desalocar_InfoDados(dado_inserir_arqDados);
        desalocar_InfoInserida(dado_inserir_arvore);
    }

    desalocar_acoes(acoes);

    //Reescrevo o cabeçalho do arquivo de dados, pois, além do status, alterei o campo de registros totais
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,1);
    escreverCabecalhoDados(arq_dados);

    //Fazer o mesmo procedimento de reescrita com o cabeçalho da árvore
    reiniciarCursorArvore(arvore);
    alterarStatusArvore(arvore,1);
    escreverCabecalhoArvore(arvore);    

    //Fechar os arquivos utilizados
    fechar_arq_dados(arq_dados);
    fechar_arvore(arvore);

    binarioNaTela(getNomeArqDados(arq_dados)); 
    binarioNaTela(getNomeArvore(arvore));

    //Desalocar os tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_Arvore(arvore); 
}