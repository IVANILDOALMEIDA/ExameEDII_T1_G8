#include "fila.h"
#include <stddef.h>
#include <stdlib.h>


Fila *fila_criar(void) {
    Fila *fila = malloc(sizeof(Fila));
    if (fila == NULL) {
        return NULL;
    }
    fila->frente = NULL;
    fila->tras = NULL;
    fila->tamanho = 0;
    return fila;
}

void fila_destruir(Fila *fila, int libertar_elementos) {
    if (fila == NULL) {
        return;
    }
    NoFila *atual = fila->frente;
    while (atual != NULL) {
        NoFila *seguinte = atual->seguinte;
        if (libertar_elementos) {
            free(atual->dado);
        }
        free(atual);
        atual = seguinte;
    }
    free(fila);
}

int fila_enfileirar(Fila *fila, void *elemento) {
    if (fila == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    NoFila *novo = malloc(sizeof(NoFila));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->dado = elemento;
    novo->seguinte = NULL;

    if (fila->tras == NULL) {
        fila->frente = novo;
        fila->tras = novo;
    } else {
        fila->tras->seguinte = novo;
        fila->tras = novo;
    }
    fila->tamanho++;
    return OK;
}

void *fila_desenfileirar(Fila *fila) {
    if (fila == NULL || fila->frente == NULL) {
        return NULL;
    }
    NoFila *no = fila->frente;
    void *dado = no->dado;

    fila->frente = no->seguinte;
    if (fila->frente == NULL) {
        fila->tras = NULL;
    }
    free(no);
    fila->tamanho--;
    return dado;
}

void *fila_frente(const Fila *fila) {
    if (fila == NULL || fila->frente == NULL) {
        return NULL;
    }
    return fila->frente->dado;
}

int fila_vazia(const Fila *fila) {
    return (fila == NULL) || (fila->tamanho == 0);
}

int fila_tamanho(const Fila *fila) {
    return (fila != NULL) ? fila->tamanho : 0;
}