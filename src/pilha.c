#include "pilha.h"
#include <stddef.h>
#include <stdlib.h>


Pilha *pilha_criar(void) {
    Pilha *pilha = malloc(sizeof(Pilha));
    if (pilha == NULL) {
        return NULL;
    }
    pilha->topo = NULL;
    pilha->tamanho = 0;
    return pilha;
}

void pilha_destruir(Pilha *pilha, int libertar_elementos) {
    if (pilha == NULL) {
        return;
    }
    NoPilha *atual = pilha->topo;
    while (atual != NULL) {
        NoPilha *seguinte = atual->seguinte;
        if (libertar_elementos) {
            free(atual->dado);
        }
        free(atual);
        atual = seguinte;
    }
    free(pilha);
}

int pilha_empilhar(Pilha *pilha, void *elemento) {
    if (pilha == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    NoPilha *novo = malloc(sizeof(NoPilha));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->dado = elemento;
    novo->seguinte = pilha->topo;
    pilha->topo = novo;
    pilha->tamanho++;
    return OK;
}

void *pilha_desempilhar(Pilha *pilha) {
    if (pilha == NULL || pilha->topo == NULL) {
        return NULL;
    }
    NoPilha *no = pilha->topo;
    void *dado = no->dado;
    pilha->topo = no->seguinte;
    free(no);
    pilha->tamanho--;
    return dado;
}

void *pilha_topo(const Pilha *pilha) {
    if (pilha == NULL || pilha->topo == NULL) {
        return NULL;
    }
    return pilha->topo->dado;
}

int pilha_vazia(const Pilha *pilha) {
    return (pilha == NULL) || (pilha->tamanho == 0);
}

int pilha_tamanho(const Pilha *pilha) {
    return (pilha != NULL) ? pilha->tamanho : 0;
}