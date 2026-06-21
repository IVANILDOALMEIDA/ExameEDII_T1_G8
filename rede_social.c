//
// Created by Diangienda on 21/06/2026.
//

#include "rede_social.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX 50

struct Pessoa {
    int id;
    char nome[MAX];
    char universidade[MAX];
    char curso[MAX];
    char senha[MAX];
    char interesses[5][20];
};

struct node {
    int vertice;
    node* prox;
};

struct Grafo {
    int numVertices;   // quantos vertices existem AGORA
    int capacidade;     // capacidade máxima de vértices que o grafo abarca (cresce se o num de vértices for maior)
    node** listaAdjacencias;
    Pessoa* pessoas;
};

// Forward declaration: adicionarPessoa() precisa chamar buscarIndicePorId()
// para validar unicidade do id, mas essa funcao so e definida mais abaixo.
int buscarIndicePorId(Grafo* grafo, int id);

node* criarNo(int valor) {
    node* novo = malloc(sizeof(node));
    if (!novo) {
        printf("Erro: sem memoria para criar no.\n");
        exit(1);
    }
    novo->vertice = valor;
    novo->prox = NULL;
    return novo;
}

Grafo* criarGrafo(int capacidadeInicial) {
    if (capacidadeInicial <= 0) {
        printf("Erro: capacidade inicial invalida.\n");
        exit(1);
    }

    Grafo* grafo = malloc(sizeof(Grafo));
    if (!grafo) {
        printf("Erro: sem memoria para criar o grafo.\n");
        exit(1);
    }

    grafo->numVertices = 0;
    grafo->capacidade = capacidadeInicial;
    grafo->listaAdjacencias = calloc(capacidadeInicial, sizeof(node*));
    grafo->pessoas = malloc(capacidadeInicial * sizeof(Pessoa));

    if (!grafo->listaAdjacencias || !grafo->pessoas) {
        printf("Erro: sem memoria para criar o grafo.\n");
        exit(1);
    }

    return grafo;
}

// Duplica a capacidade dos arrays quando o grafo enche (crescimento amortizado)
void crescerGrafo(Grafo* grafo) {
    int capacidadeAntiga = grafo->capacidade;
    int novaCapacidade = grafo->capacidade * 2;

    node** novaLista = realloc(grafo->listaAdjacencias, novaCapacidade * sizeof(node*));
    Pessoa* novasPessoas = realloc(grafo->pessoas, novaCapacidade * sizeof(Pessoa));

    if (!novaLista || !novasPessoas) {
        printf("Erro: sem memoria para crescer o grafo.\n");
        exit(1);
    }

    grafo->listaAdjacencias = novaLista;
    grafo->pessoas = novasPessoas;
    grafo->capacidade = novaCapacidade;

    // realloc preserva o conteudo das posicoes antigas, mas as posicoes
    // novas ficam com lixo de memoria, igual ao malloc. Por isso e preciso
    // zera-las explicitamente aqui, ou o mesmo problema do criarGrafo
    // reapareceria a cada crescimento do grafo.
    for (int i = capacidadeAntiga; i < novaCapacidade; i++) {
        grafo->listaAdjacencias[i] = NULL;
    }
}

// Adiciona uma nova pessoa (um novo vertice) e devolve o seu indice interno (indice do vértice).
// Devolve -1 se o id_usuario ja existir (id é a chave de negocio, deve ser unico).
int adicionarPessoa(Grafo* grafo, int id, char* nome, char* universidade, char* curso, char* senha, int qtd_interesses,char (*interesses)[20]){
    if (buscarIndicePorId(grafo, id) != -1) {
        printf("Aviso: ja existe uma pessoa com id %d. Pessoa nao adicionada.\n", id);
        return -1;
    }



    if (grafo->numVertices == grafo->capacidade)
        crescerGrafo(grafo);

    int indice = grafo->numVertices;


    grafo->pessoas[indice].id = id;
    strncpy(grafo->pessoas[indice].nome, nome, MAX - 1);
    grafo->pessoas[indice].nome[MAX - 1] = '\0';

    strncpy(grafo->pessoas[indice].universidade, universidade, MAX - 1);
    grafo->pessoas[indice].universidade[MAX - 1] = '\0';

    strncpy(grafo->pessoas[indice].curso, curso, MAX - 1);
    grafo->pessoas[indice].curso[MAX - 1] = '\0';

    strncpy(grafo->pessoas[indice].senha, senha, MAX - 1);
    grafo->pessoas[indice].senha[MAX - 1] = '\0';

    for (int i =0; i < qtd_interesses; i++) {
        strncpy(grafo->pessoas[indice].interesses[i],  interesses[i], MAX - 1);
        grafo->pessoas[indice].interesses[i][MAX - 1] = '\0';
    }

    printf("ola");
    grafo->listaAdjacencias[indice] = NULL;
    grafo->numVertices++;

    return indice;
}

// Procura o indice interno de uma pessoa a partir do seu id_usuario (busca linear)
int buscarIndicePorId(Grafo* grafo, int id) {
    for (int i = 0; i < grafo->numVertices; i++) {
        if (grafo->pessoas[i].id == id)
            return i;
    }
    return -1; // nao encontrado
}

int arestaExiste(Grafo* grafo, int origem, int destino) {
    node* atual = grafo->listaAdjacencias[origem];
    while (atual) {
        if (atual->vertice == destino)
            return 1;
        atual = atual->prox;
    }
    return 0;
}

// Versao interna: trabalha com indices do array
void addAmizade(Grafo* grafo, int s, int d) {
    if (s < 0 || s >= grafo->numVertices || d < 0 || d >= grafo->numVertices) {
        printf("Aviso: vertice invalido. Amizade ignorada.\n");
        return;
    }
    if (arestaExiste(grafo, s, d)) {
        printf("Aviso: %s e %s ja sao amigos.\n", grafo->pessoas[s].nome, grafo->pessoas[d].nome);
        return;
    }

    node* novoNo = criarNo(d);
    novoNo->prox = grafo->listaAdjacencias[s];
    grafo->listaAdjacencias[s] = novoNo;

    novoNo = criarNo(s);
    novoNo->prox = grafo->listaAdjacencias[d];
    grafo->listaAdjacencias[d] = novoNo;
}

// Versao publica: quem usa o grafo (o main) trabalha so com ids, nunca com indices
void addAmizadePorId(Grafo* grafo, int id1, int id2) {
    int s = buscarIndicePorId(grafo, id1);
    int d = buscarIndicePorId(grafo, id2);

    if (s == -1 || d == -1) {
        printf("Aviso: id %d ou %d nao existe. Amizade ignorada.\n", id1, id2);
        return;
    }
    addAmizade(grafo, s, d);
}

void imprimirRede(Grafo* grafo) {
    for (int v = 0; v < grafo->numVertices; v++) {
        printf("\n%s (id %d, %s) e amigo de: ",
               grafo->pessoas[v].nome, grafo->pessoas[v].id, grafo->pessoas[v].universidade);
        node* temp = grafo->listaAdjacencias[v];
        if (!temp) {
            printf("(ninguem ainda)");
        }
        while (temp) {
            printf("%s", grafo->pessoas[temp->vertice].nome);
            temp = temp->prox;
            if (temp) printf(", ");
        }
    }
    printf("\n");
}

void liberarGrafo(Grafo* grafo) {
    for (int v = 0; v < grafo->numVertices; v++) {
        node* atual = grafo->listaAdjacencias[v];
        while (atual) {
            node* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(grafo->listaAdjacencias);
    free(grafo->pessoas);
    free(grafo);
}
