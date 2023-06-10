//Lucas de Souza Brandão, 13695021 100%
//João Pedro Soares de Azevedo Calixto 13732011 100%

#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"

int main(){
    int funcionalidade;
    scanf("%d", &funcionalidade);

    switch(funcionalidade){
        case 8:
            create_index();
            break;
        case 9:
            where();
            break;
        case 10:
            insert_into();
            break;
    }

    return 0;
}
