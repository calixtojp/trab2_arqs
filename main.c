//Lucas de Souza Brandão, 13695021 100%
//João Pedro Soares de Azevedo Calixto 13732011 100%

#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"

int main(){
    int funcionalidade;
    scanf("%d", &funcionalidade);

    switch(funcionalidade){
        case 3:
            create_index();
            break;
        case 4:
            where();
            break;
        case 5:
            delete_from();
            break;
        case 6:
            insert_into();
            break;
        case 7:
            update();
            break;
    }

    return 0;
}
