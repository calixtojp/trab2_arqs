#include <stdio.h>
#include <stdlib.h>

int main(){

    char lixo;
    char nome_arq[50];
    int pija, lixo_int;

    printf("digite o nome do arquivo: ");
    scanf("%s", nome_arq);

    FILE *arq;
    arq = fopen(nome_arq, "rb");

    printf("\nCABECALHO:\n");
    fread(&(lixo), sizeof(char), 1, arq);
    printf("status:%c\n", lixo);
    fread(&lixo_int, sizeof(int), 1, arq);
    printf("noRaiz:%d\n", lixo_int);
    fread(&pija, sizeof(int), 1, arq);    
    printf("RRNproxNo:%d\n", pija);
    fread(&lixo_int, sizeof(int), 1, arq);    
    printf("nroNiveis:%d\n", lixo_int);
    fread(&lixo_int, sizeof(int), 1, arq);    
    printf("nroChaves:%d\n", lixo_int);
    for(int i = 0; i < 59; ++i){
        fread(&lixo, sizeof(char), 1, arq);
    }
    printf("\n\n");

    for(int i = 0; i < pija; ++i){
        int nivel, n, P, C;
        long int Pr;

        fread(&(nivel), sizeof(int), 1, arq);
        fread(&(n), sizeof(int), 1, arq);
        
        printf("No:%d\nNivel:%d|n:%d\n", i, nivel, n);

        fread(&(P), sizeof(int), 1, arq);
        printf("\tP%d:%d\n", 1, P);
        for(int j = 0; j < 4; ++j){
            fread(&(C), sizeof(int), 1, arq);
            fread(&(Pr), sizeof(long int), 1, arq);
            fread(&(P), sizeof(int), 1, arq);

            printf("\tC%d:%d|Pr%d:%ld\n\tP%d:%d\n", j+1, C, j+1, Pr, j+2, P);
        }
        printf("\n\n");
    }
    return 0;
}