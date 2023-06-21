/*
          Integrantes             NºUSP     Participação
    Artur De Vlieger Lima       13671574        100%
    Gustavo Gabriel Ribeiro     13672683        100%
*/

#include <stdio.h>
#include "funcionalidades.h"

int main(){

    //Selecione a funcionalidade
    int funcao;
    scanf("%d", &funcao);

    switch(funcao){
        case 1:
            CRIAR_TABELA();
            break;
        case 2:
            SELECT();
            break;
        case 3:
            CREATE_INDEX();
            break;
        case 4:
            SELECT_WHERE();
            break;
        case 5:
            DELETE();
            break;
        case 6:
            INSERT_INTO();
            break;
        case 7:
            UPDATE();
            break;
        case 8:
            CREATE_TREE();
            break;
        case 9:
            SELECT_WHERE_TREE();
            break;
        case 10:
            INSERT_INTO_TREE();
            break;
    }

    return 0;
}