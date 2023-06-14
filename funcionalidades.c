#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliares.h"
#include "manipulacao.h"

//Funcionalidade [8]
void create_index(){

//     //Aloca os tipos que serão usados
//     ArqDados_t *arq_dados = alocar_arq_dados();
//     erro(arq_dados);
//     Arvore_t *arvore = alocar_arvore();
//     erro(arvore);

//     //Faz a leitura dos inputs
//     ler_nome_arq_dados(arq_dados);
//     ler_campoIndexado(arq_index);
//     ler_tipoDado(arq_index);
//     ler_nome_arq_index(arq_index);

//     //Com os inputs armazenados, faço 
//     //a abertura dos arquivos.
//     abrir_arq_dados(arq_dados, "rb");
//     abrir_arq_index(arq_index, "wb");

//     //Ler o cabeçalho do arquivo de dados
//     ler_cabecalho_dados(arq_dados);

//     //Verificar consistência dos dados do arquivo
//     if(testarStatusDados(arq_dados)==0){
//         mensagem_erro();
//     }

//     long int byteOffSetAtual = getTamCabecalhoDados(arq_dados);

//     //Aloco espaço para o vetor que armazenará o arquivo de index
//     unsigned int nroRegValidos = get_nroRegValidos(arq_dados);
//     alocar_vet_index(arq_index, nroRegValidos);

//     int foi_escrito;
//     //tem 1 se conseguiu indexar o registro, 0 caso contrário

//     int pos_reg = 0;
//     //Guarda a posição que será escrito o registro  

//     do{
//         foi_escrito = indexaRegistro(arq_dados, arq_index, pos_reg, &byteOffSetAtual);
//         pos_reg++;
//     }while(foi_escrito==1);

//     //Com o os index carregados em RAM, faço a ordenação
//     int qntd_registros = pos_reg-1;
//     ordenaVetIndex(arq_index, qntd_registros);

//     //Antes de começar a escrever o arquivo de index, 
//     //devo configurar seu status para '0'. Após isso,
//     //inicio a escrita do arquivo de index.
//     alterarStatusIndex(arq_index,0);
//     alterarQtdRegIndex(arq_index, qntd_registros);
//     escreveArqIndex(arq_index);

//     //Agora, indico que o arquivo está consistente, 
//     //pois as alterações já foram realizadas
//     reiniciarCursorIndex(arq_index);
//     alterarStatusIndex(arq_index,1);
//     escreverStatusIndex(arq_index);

//     //Fechar arquivos
//     fechar_arq_index(arq_index);
//     fechar_arq_dados(arq_dados);

//     binarioNaTela(getNomeArqIndex(arq_index));

//     //Desalocar tipos utilizados
//     desalocar_ArqDados(arq_dados);
//     desalocar_ArqIndex(arq_index);
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

    /*Declaro a struct com todas as ações necessárias para esse funcionalidade. 
    Se algum tipo não for necessário, passo uma função NoOp daquele tipo.*/

    FncAcoes *acoes = alocar_acoes();
    set_acoes(acoes, validaPrinta, printa_busca, NoOpAcaoNo, achouReg);
    
    //Loop que faz n buscas
    for(int i=1; i<=n; i++){
        printf("Resposta para a busca %d\n",i);

        //Ler os critérios de busca
        InfoDados_t *criterios = ler_criterios_busca();

        /*Processar o registro usando a ação 'printa_busca'
        e o final 'achouReg', que diz se o registro é inexistente, 
        caso nenhum satisfaça os critérios de busca*/
        
        //MUDAR
        processaRegistros(arq_dados,arvore,criterios,acoes);

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

    // //Requisito a função que irá ler da entrada padrão e,
    // //posteriormente, inserir os dados nos arquivos.
    // inserirRegStdin(arq_dados, arq_index, qtdInserir);

    //Loop que faz as inserções
    for(int i=1; i<=qtdInserir; i++){

        //Ler os dados que serão inseridos
        InfoDados_t *dados_inserir = ler_criterios_busca();

        /*Processar o registro usando a ação 'insere_reg'
        e como não é necessário uma ação final, a ação final é um 'no operation
        */



        //MUDAR
        //processaRegistros(arq_dados,arvore,dados_inserir,insere_reg,noop);

        //Desalocar crtérios de busca    	
        desalocar_InfoDados(dados_inserir);
    }

    // //Após inserir os dados, obtenho a quantidade atual de registros
    // int qtdRegIndex = get_nroRegIndex(arq_index);
    // ordenaVetIndex(arq_index, qtdRegIndex);

    // //Agora devo reescrever todo o arquivo de índice. Para isso:
    // //Fecho o arquivo e abro com modo 'wb', para reescrevê-lo por completo
    // fechar_arq_index(arq_index);
    // abrir_arq_index(arq_index, "wb");
    // alterarStatusIndex(arq_index,0);
    // escreveArqIndex(arq_index);

    // //Agora, indico que os arquivos estão consistentes, pois já usei ambos
    // reiniciarCursorIndex(arq_index);
    // alterarStatusIndex(arq_index,1);
    // escreverStatusIndex(arq_index);

    //Reescrevo o cabeçalho do arquivo de dados, pois, além do status, alterei o campo de registros totais
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,1);
    escreverCabecalhoDados(arq_dados);

    //Fechar os arquivos utilizados
    fechar_arq_dados(arq_dados);
    // fechar_arq_index(arq_index);

    binarioNaTela(getNomeArqDados(arq_dados)); 
    // binarioNaTela(getNomeArqIndex(arq_index));

    //Desalocar os tipos utilizados
    desalocar_ArqDados(arq_dados);
    // desalocar_ArqIndex(arq_index); 
}