all: manipulacao.o arvore_b_estrela.o auxiliares.o main.o arq_dados.o funcionalidades.o
	gcc manipulacao.o arvore_b_estrela.o auxiliares.o main.o arq_dados.o funcionalidades.o -o arq -Wall -std=c99 -g;

manipulacao.o:
	gcc -c manipulacao.c -o manipulacao.o

arvore_b_estrela.o:
	gcc -c arvore_b_estrela.c -o arvore_b_estrela.o

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

