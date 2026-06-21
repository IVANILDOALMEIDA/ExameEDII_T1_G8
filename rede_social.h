//
// Created by Diangienda on 21/06/2026.
//

#ifndef UNTITLED1_REDE_SOCIAL_H
#define UNTITLED1_REDE_SOCIAL_H

typedef struct Pessoa Pessoa;
typedef struct node node;
typedef struct Grafo Grafo;

int buscarIndicePorId(Grafo* grafo, int id);
node* criarNo(int valor);
Grafo* criarGrafo(int capacidadeInicial);
void crescerGrafo(Grafo* grafo);
int adicionarPessoa(Grafo* grafo, int id, char* nome, char* universidade, char* curso, char* senha, int qtd_interesses ,char (*interesses)[20]);
int buscarIndicePorId(Grafo* grafo, int id);
int arestaExiste(Grafo* grafo, int origem, int destino);
void addAmizade(Grafo* grafo, int s, int d);
void addAmizadePorId(Grafo* grafo, int id1, int id2);
void imprimirRede(Grafo* grafo);
void liberarGrafo(Grafo* grafo);




#endif //UNTITLED1_REDE_SOCIAL_H