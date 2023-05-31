#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliares.h"
#include "manipulacao.h"

//Funcionalidade [3]
void create_index(){

    //Aloca os tipos que serão usados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    ArqIndex_t *arq_index = alocar_arq_index();
    erro(arq_index);

    //Faz a leitura dos inputs
    ler_nome_arq_dados(arq_dados);
    ler_campoIndexado(arq_index);
    ler_tipoDado(arq_index);
    ler_nome_arq_index(arq_index);

    //Com os inputs armazenados, faço 
    //a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "rb");
    abrir_arq_index(arq_index, "wb");

    //Ler o cabeçalho do arquivo de dados
    ler_cabecalho_dados(arq_dados);

    //Verificar consistência dos dados do arquivo
    if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    long int byteOffSetAtual = getTamCabecalhoDados(arq_dados);

    //Aloco espaço para o vetor que armazenará o arquivo de index
    unsigned int nroRegValidos = get_nroRegValidos(arq_dados);
    alocar_vet_index(arq_index, nroRegValidos);

    int foi_escrito;
    //tem 1 se conseguiu indexar o registro, 0 caso contrário

    int pos_reg = 0;
    //Guarda a posição que será escrito o registro  

    do{
        foi_escrito = indexaRegistro(arq_dados, arq_index, pos_reg, &byteOffSetAtual);
        pos_reg++;
    }while(foi_escrito==1);

    //Com o os index carregados em RAM, faço a ordenação
    int qntd_registros = pos_reg-1;
    ordenaVetIndex(arq_index, qntd_registros);

    //Antes de começar a escrever o arquivo de index, 
    //devo configurar seu status para '0'. Após isso,
    //inicio a escrita do arquivo de index.
    alterarStatusIndex(arq_index,0);
    alterarQtdRegIndex(arq_index, qntd_registros);
    escreveArqIndex(arq_index);

    //Agora, indico que o arquivo está consistente, 
    //pois as alterações já foram realizadas
    reiniciarCursorIndex(arq_index);
    alterarStatusIndex(arq_index,1);
    escreverStatusIndex(arq_index);

    //Fechar arquivos
    fechar_arq_index(arq_index);
    fechar_arq_dados(arq_dados);

    binarioNaTela(getNomeArqIndex(arq_index));

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_ArqIndex(arq_index);
}

//Funcionalidade [4]
void where(void){
    //Aloca os tipos que serão usados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    ArqIndex_t *arq_index = alocar_arq_index();
    erro(arq_index);
    int n;

    //Faz a leitura dos inputs
    ler_nome_arq_dados(arq_dados);
    ler_campoIndexado(arq_index);
    ler_tipoDado(arq_index);
    ler_nome_arq_index(arq_index);
    scanf("%d",&n);

    //Com os inputs armazenados, faço a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "rb");
    abrir_arq_index(arq_index, "rb");

    //Carrego o arquivo de indice na memoria primaria
    ler_cabecalho_arq_index(arq_index);
    ler_dados_arq_index(arq_index);

    //Ler o cabeçalho do arquivo de dados
    ler_cabecalho_dados(arq_dados);

    //Testo se os dois arquivos estao consistentes. Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusIndex(arq_index)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    //Loop que faz n buscas
    for(int i=1; i<=n; i++){
        printf("Resposta para a busca %d\n",i);

        //Ler os critérios de busca
        InfoBusca_t *criterios = ler_criterios_busca();

        //Não carrego nenhuma alteração, pois não irei alterar o registro buscado.
        InfoBusca_t *alteracoes;

        /*Processar o registro usando a ação 'printa_busca'
        e o final 'achouReg', que diz se o registro é inexistente, 
        caso nenhum satisfaça os critérios de busca*/
        processaRegistros(arq_dados,arq_index,criterios,alteracoes,printa_busca,achouReg);

        //Desalocar crtérios de busca    	
        desalocar_InfoBusca(criterios);
    }

    //Fechar arquivos
    fechar_arq_index(arq_index);
    fechar_arq_dados(arq_dados);

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_ArqIndex(arq_index);
}

//Funcionalidade [5]
void delete_from(){
    //Aloca os tipos que serão usados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    ArqIndex_t *arq_index = alocar_arq_index();
    erro(arq_index);
    int n;

    //Faz a leitura dos inputs
    ler_nome_arq_dados(arq_dados);
    ler_campoIndexado(arq_index);
    ler_tipoDado(arq_index);
    ler_nome_arq_index(arq_index);
    scanf("%d",&n);

    //Com os inputs armazenados, faço a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "r+b");
    abrir_arq_index(arq_index, "rb");

    //Carrego o arquivo de indice na memoria primaria
    ler_cabecalho_arq_index(arq_index);
    ler_dados_arq_index(arq_index);

    //Ler o cabeçalho do arquivo de dados
    ler_cabecalho_dados(arq_dados);

    //Testo se os dois arquivos estao consistentes. Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusIndex(arq_index)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    //indico que o arquivo de dados está insconsistente, pois vou escrever apenas nele por enquanto
    //como li o cabecalho do arquivo de dados, o cursor está logo após o cabecalho
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,0);
    escreverStatusDados(arq_dados);

    //Loop que faz 'n' deleções
    for(int i=1; i<=n; i++){
        //Ler os critérios de busca para os registros que se deseja remover
        InfoBusca_t *criterios = ler_criterios_busca();

        //Variável que recebe os critérios de alteração da funcionalidade 7. Na funcionalidade 5 ela é apenas ignorada
        InfoBusca_t *alteracoes;

        //Processar o registro usando a ação 'deletar_registros' e o final 'copiaVetTemp'
        processaRegistros(arq_dados,arq_index,criterios,alteracoes,deletarRegistro, copiaVetTemp);

        //Desalocar tipos utilizados    	
        desalocar_InfoBusca(criterios);
    }
    
    //Agora devo reescrever todo o arquivo de índice. Para isso:
    //Fecho o arquivo e abro com modo 'wb', para reescrevê-lo por completo
    fechar_arq_index(arq_index);
    abrir_arq_index(arq_index, "wb");
    alterarStatusIndex(arq_index,0);
    escreveArqIndex(arq_index);

    //Agora, indico que os arquivos estão consistentes, pois já usei ambos
    reiniciarCursorIndex(arq_index);
    alterarStatusIndex(arq_index,1);
    escreverStatusIndex(arq_index);

    //Reescrevo o cabeçalho do arquivo de dados, pois, além do status, alterei o campo de registros removidos
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,1);
    escreverCabecalhoDados(arq_dados);

    //Fechar arquivos
    fechar_arq_index(arq_index);
    fechar_arq_dados(arq_dados);

    //Chama a funcao binarioNaTela() para gerar uma saída com base nas mudanças nos arquivos
    binarioNaTela(getNomeArqDados(arq_dados)); 
    binarioNaTela(getNomeArqIndex(arq_index));

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_ArqIndex(arq_index);
}

//Funcionalidade [6]
void insert_into(){
    //Alocar tipos utilizados
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    ArqIndex_t *arq_index = alocar_arq_index();
    erro(arq_index);

    //Ler entrada de dados
    ler_nome_arq_dados(arq_dados);
    ler_campoIndexado(arq_index);
    ler_tipoDado(arq_index);
    ler_nome_arq_index(arq_index);

    //abrir arquivos
    abrir_arq_index(arq_index, "rb");
    abrir_arq_dados(arq_dados, "r+b");

    //Ler os cabeçalhos
    ler_cabecalho_arq_index(arq_index);
    ler_dados_arq_index(arq_index);
    ler_cabecalho_dados(arq_dados);

    //Testo se os dois arquivos estao consistentes. Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusIndex(arq_index)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }
    
    //indico que o arquivo de dados está insconsistente, pois vou escrever apenas nele por enquanto
    //como li o cabecalho do arquivo de dados, o cursor está logo após o cabecalho
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,0);
    escreverStatusDados(arq_dados);

    //Como vou escrever os dados no final, devo levar o cursor para o final
    levaFinalCursorDados(arq_dados);

    int qtdInserir;
    scanf(" %d", &qtdInserir);

    //Requisito a função que irá ler da entrada padrão e,
    //posteriormente, inserir os dados nos arquivos.
    inserirRegStdin(arq_dados, arq_index, qtdInserir);

    //Após inserir os dados, obtenho a quantidade atual de registros
    int qtdRegIndex = get_nroRegIndex(arq_index);
    ordenaVetIndex(arq_index, qtdRegIndex);

    //Agora devo reescrever todo o arquivo de índice. Para isso:
    //Fecho o arquivo e abro com modo 'wb', para reescrevê-lo por completo
    fechar_arq_index(arq_index);
    abrir_arq_index(arq_index, "wb");
    alterarStatusIndex(arq_index,0);
    escreveArqIndex(arq_index);

    //Agora, indico que os arquivos estão consistentes, pois já usei ambos
    reiniciarCursorIndex(arq_index);
    alterarStatusIndex(arq_index,1);
    escreverStatusIndex(arq_index);

    //Reescrevo o cabeçalho do arquivo de dados, pois, além do status, alterei o campo de registros totais
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,1);
    escreverCabecalhoDados(arq_dados);

    //Fechar os arquivos utilizados
    fechar_arq_dados(arq_dados);
    fechar_arq_index(arq_index);

    binarioNaTela(getNomeArqDados(arq_dados)); 
    binarioNaTela(getNomeArqIndex(arq_index));

    //Desalocar os tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_ArqIndex(arq_index);
}

//Funcionalidade [7]
void update(){
    //Aloca os tipos que serão usados.
    ArqDados_t *arq_dados = alocar_arq_dados();
    erro(arq_dados);
    ArqIndex_t *arq_index = alocar_arq_index();
    erro(arq_index);
    int n;

    //Faz a leitura dos inputs.
    ler_nome_arq_dados(arq_dados);
    ler_campoIndexado(arq_index);
    ler_tipoDado(arq_index);
    ler_nome_arq_index(arq_index);
    scanf("%d\n",&n);

    //Com os inputs armazenados, faço a abertura dos arquivos.
    abrir_arq_dados(arq_dados, "r+b");
    abrir_arq_index(arq_index, "rb");

    //Carrego o arquivo de indice na memoria primaria.
    ler_cabecalho_arq_index(arq_index);
    ler_dados_arq_index(arq_index);

    //Ler o cabeçalho do arquivo de dados.
    ler_cabecalho_dados(arq_dados);

    //Testo se os dois arquivos estao consistentes. Se não estão, encerro o programa com uma mensagem de erro.
    if(testarStatusIndex(arq_index)==0){
        mensagem_erro();
    }else if(testarStatusDados(arq_dados)==0){
        mensagem_erro();
    }

    //indico que o arquivo de dados está insconsistente, pois vou escrever apenas nele por enquanto
    //como li o cabecalho do arquivo de dados, o cursor está logo após o cabecalho
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,0);
    escreverStatusDados(arq_dados);

    int cont_n;
    for(cont_n = 0; cont_n < n; cont_n++){
        //ler o critérios de busca, e alterações
        InfoBusca_t *criterios = ler_criterios_busca();
        InfoBusca_t *alteracoes = ler_criterios_busca();

        processaRegistros(arq_dados,arq_index,criterios,alteracoes,modificaReg,ordenaVetIndexFinal);

        desalocar_InfoBusca(criterios);
        desalocar_InfoBusca(alteracoes);
    }

    //Agora devo reescrever todo o arquivo de índice. Para isso:
    //Fecho o arquivo e abro com modo 'wb', para reescrevê-lo por completo
    fechar_arq_index(arq_index);
    abrir_arq_index(arq_index, "wb");
    alterarStatusIndex(arq_index,0);
    escreveArqIndex(arq_index);

    //Agora, indico que os arquivos estão consistentes, pois já usei ambos
    reiniciarCursorIndex(arq_index);
    alterarStatusIndex(arq_index,1);
    escreverStatusIndex(arq_index);

    //Reescrevo o cabeçalho do arquivo de dados, pois, além do status, alterei o campo de registros removidos
    reiniciarCursorDados(arq_dados);
    alterarStatusDados(arq_dados,1);
    escreverCabecalhoDados(arq_dados);

    //Fechar arquivos
    fechar_arq_index(arq_index);
    fechar_arq_dados(arq_dados);

    //Chama a funcao binarioNaTela() para gerar uma saída com base nas mudanças nos arquivos
    binarioNaTela(getNomeArqDados(arq_dados)); 
    binarioNaTela(getNomeArqIndex(arq_index));

    //Desalocar tipos utilizados
    desalocar_ArqDados(arq_dados);
    desalocar_ArqIndex(arq_index);
}