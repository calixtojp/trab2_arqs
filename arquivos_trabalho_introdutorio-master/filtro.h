#ifndef _FILTRO_H
#define _FILTRO_H
    
    #include "indice.h"
    #include "funcoesFornecidas.h"

    
    typedef struct filtro_t{
        int  *idCrime;
        char *dataCrime;
        int  *numeroArtigo;
        char *marcaCelular;
        char *lugarCrime;
        char *descricaoCrime; 
    }Filtro;


    Filtro* criar_filtro();
    bool preencher_filtro(Filtro *meu_filtro, char *campoIndexado,char **valor_buscado);
    bool adequa_filtro(Registro *meu_registro, Filtro *meu_filtro);
    void liberar_filtro(Filtro* meu_filtro);
    
#endif