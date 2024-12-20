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
    ignorar_palavra_stdin();//Devo ignorar a entrada, pois o campo indexado é sempre idCrime 
    ignorar_palavra_stdin();//Devo ignorar a entrada, pois o tipo indexado será sempre 
    ler_nome_arvore(arvore);

    //Com os inputs armazenados, faço a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "rb");
    iniciar_arvore(arvore, "w+b");

    //Ler o cabeçalho do arquivo de dados
    ler_cabecalho_dados(arq_dados);

    //Verificar consistência dos dados do arquivo
    if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    //Já que farei escritas no arquivo que contém a árvore,
    //devo marcar esse arquivo como inconsistente.
    alterarStatusArvore(arvore, 0);
    setCabecalhoArvoreNulo(arvore);
    escreverCabecalhoArvore(arvore);

    //A primeira chave que vou inserir, deve ser feita manualmente, pois
    //devo criar a árvore para executar a função de busca utilizada na função insere_arvore()

    InfoDados_t *dado_inserir_arqDados;//Dados que serão lidos/inseridos.

    //Configuro a árvore e o arquivo de dados para a inserção.
    configuraInsercao(arq_dados, arvore);

    int cont = 0;
    //Contador de registros lidos.

    long int byteOffset_atual = getTamCabecalhoDados(arq_dados); 
    //contador de byteOffset atual

    int tam_reg; 
    //tamanho do registro lido

    int qtd_leituras = get_nroRegTotal(arq_dados);
    //leituras que serão realizadas no arquivo de dados. 

    //Procuro um primeiro registro válido para inserir na árvore e criar a raiz
    //como não se deve inserir registros removidos logicamente, procura-se um válido.
    if(qtd_leituras>0){
        do{
            //Devo armazenar o os dados de um registro (vindos pelo Stdin Ou pelo Arquivo de dados).
            dado_inserir_arqDados = ler_dados_registro(lerRegDoArqDados, arq_dados, &tam_reg);
            cont++;

            //-Incrementar o cursor do byteOffSet atual.
            byteOffset_atual += tam_reg;

        }while(cont < qtd_leituras && !validaInfoDados(dado_inserir_arqDados));

        //Inserir na árvore. Para isso, devo:
        /*-Incrementar o cursor do byteOffSet atual, pois atualmente ele está no começo 
        do próximo registro, mas quero que ele esteja no começo daquele que vou escrever*/
        byteOffset_atual -= tam_reg;
        
        cria_raiz(arq_dados, arvore, byteOffset_atual, dado_inserir_arqDados);

        //- Voltar o cursor para o início do próximo registro
        byteOffset_atual += tam_reg;
        
        //-Desalocar as informações lidas.
        desalocar_InfoDados(dado_inserir_arqDados);
    }

    //Agora, insiro os demais registros na árvore usando a busca a função de insercao_arvore().

    //Inicio o laço partindo do cont anterior e indo até qtd_leituras.
    for(; cont < qtd_leituras; ++cont){
        dado_inserir_arqDados = ler_dados_registro(lerRegDoArqDados, arq_dados, &tam_reg);
        if(validaInfoDados(dado_inserir_arqDados)){
            //se o registro é válido, então insiro no árvore
            insercao_arvore(arq_dados, arvore, byteOffset_atual, dado_inserir_arqDados);

            desalocar_InfoDados(dado_inserir_arqDados);
        }
        byteOffset_atual += tam_reg;
    }

    //reescrevo o cabecalho da arvore agora com status consistente e com os novos dados atualizados.
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
    int n;//número de buscas que serão realizadas

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

    //Testo se os dois arquivos estao consistentes. 
    //Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusArvore(arvore)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    /*Configuro a struct com todas as ações necessárias para essa funcionalidade.*/
    configuraBusca(arq_dados, arvore);

    //Loop que faz n buscas
    for(int i=1; i<=n; i++){
        printf("Resposta para a busca %d\n",i);

        //Ler os critérios de busca
        InfoDados_t *criterios = ler_criterios_busca();

        buscaRegistro(arq_dados, arvore, criterios);

        //Desalocar crtérios de busca    	
        desalocar_InfoDados(criterios);
    }

    //Fechar arquivos
    fechar_arvore(arvore);
    fechar_arq_dados(arq_dados);

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_Arvore(arvore);
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

    //Testo se os dois arquivos estao consistentes.
    //Se não estão, encerro o programa com uma mensagem de erro.
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

    configuraInsercao(arq_dados, arvore);

    InfoDados_t *dado_inserir_arqDados;

    int tam_reg; //tamanho do registro lido
    long int byteOffset; //guarda o byteOffset do registro que vou inserir
    
    if(arvore_vazia(arvore)){
        //Se a árvore está vazia, então a função insercao_arvore() não pode ser efetuada.
        //Desse modo, a inserção deve ser feita de maneira manual.

        //Ler os dados que serão inseridos.
        dado_inserir_arqDados = ler_dados_registro(lerRegDoStdin,arq_dados, &tam_reg);

        //Inserir no arquivo de dados
        insercao_arqDados(arq_dados, dado_inserir_arqDados);

        //Realizar a inserção específica para árvore vazia.
        byteOffset = getProxByteOffSet(arq_dados);
        cria_raiz(arq_dados, arvore, byteOffset, dado_inserir_arqDados);

        desalocar_InfoDados(dado_inserir_arqDados);

        /*Reduzo em 1 a quantidade que deve ser inserida, 
        para que o loop de inserções não execute uma vez a mais*/
        qtdInserir--;
    }

    //Loop que faz as inserções usando a inserção para árvores não vazias (insercao_arvore()).
    for(int i=1; i<=qtdInserir; i++){
        //Ler os dados que serão inseridos.
        dado_inserir_arqDados = ler_dados_registro(lerRegDoStdin,arq_dados, &tam_reg);

        //Inserir no arquivo de dados
        byteOffset = getProxByteOffSet(arq_dados);
        insercao_arqDados(arq_dados, dado_inserir_arqDados);

        //Inserir na árvore
        insercao_arvore(arq_dados, arvore, byteOffset, dado_inserir_arqDados);

        //Desalocar crtérios de busca
        desalocar_InfoDados(dado_inserir_arqDados);
    }

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