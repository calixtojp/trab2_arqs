all: manipulacao.o arq_indice.o auxiliares.o main.o arq_dados.o funcionalidades.o
	gcc manipulacao.o arq_indice.o auxiliares.o main.o arq_dados.o funcionalidades.o -o arq -Wall -std=c99 -g;

manipulacao.o:
	gcc -c manipulacao.c -o manipulacao.o

arq_indice.o:
	gcc -c arq_indice.c -o arq_indice.o

auxiliares.o:
	gcc -c auxiliares.c -o auxiliares.o

arq_dados.o:
	gcc -c arq_dados.c -o arq_dados.o

funcionalidades.o:
	gcc -c funcionalidades.c -o funcionalidades.o

main.o:
	gcc -c main.c -o main.o

run:
	./arq

clean:
	rm *.o arq

