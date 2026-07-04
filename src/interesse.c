#include "interesse.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



ListaInteresses *lista_interesses_criar(void) {
    ListaInteresses *lista = malloc(sizeof(ListaInteresses));
    if (lista == NULL) {
        return NULL;
    }
    lista->inicio = NULL;
    lista->tamanho = 0;
    return lista;
}

void lista_interesses_destruir(ListaInteresses *lista) {
    if (lista == NULL) {
        return;
    }
    NoInteresse *atual = lista->inicio;
    while (atual != NULL) {
        NoInteresse *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
    free(lista);
}

int lista_interesses_inserir(ListaInteresses *lista, const char *interesse) {
    if (lista == NULL || interesse == NULL || interesse[0] == '\0') {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (lista_interesses_existe(lista, interesse)) {
        return ERRO_JA_EXISTE;
    }

    NoInteresse *novo = malloc(sizeof(NoInteresse));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }

    strncpy(novo->interesse, interesse, MAX_INTERESSE - 1);
    novo->interesse[MAX_INTERESSE - 1] = '\0';

    /* inserção no início: O(1) */
    novo->seguinte = lista->inicio;
    lista->inicio = novo;
    lista->tamanho++;

    return OK;
}

int lista_interesses_remover(ListaInteresses *lista, const char *interesse) {
    if (lista == NULL || interesse == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }

    NoInteresse *atual = lista->inicio;
    NoInteresse *anterior = NULL;

    while (atual != NULL) {
        if (comparar_strings_ci(atual->interesse, interesse) == 0) {
            if (anterior == NULL) {
                lista->inicio = atual->seguinte;
            } else {
                anterior->seguinte = atual->seguinte;
            }
            free(atual);
            lista->tamanho--;
            return OK;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    return ERRO_NAO_ENCONTRADO;
}

int lista_interesses_existe(const ListaInteresses *lista, const char *interesse) {
    if (lista == NULL || interesse == NULL) {
        return 0;
    }
    for (NoInteresse *atual = lista->inicio; atual != NULL; atual = atual->seguinte) {
        if (comparar_strings_ci(atual->interesse, interesse) == 0) {
            return 1;
        }
    }
    return 0;
}

int lista_interesses_tamanho(const ListaInteresses *lista) {
    if (lista == NULL) {
        return 0;
    }
    return lista->tamanho;
}

int lista_interesses_contar_comuns(const ListaInteresses *a, const ListaInteresses *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    int comuns = 0;
    for (NoInteresse *na = a->inicio; na != NULL; na = na->seguinte) {
        for (NoInteresse *nb = b->inicio; nb != NULL; nb = nb->seguinte) {
            if (comparar_strings_ci(na->interesse, nb->interesse) == 0) {
                comuns++;
                break; /* já contabilizado, passar ao interesse seguinte de 'a' */
            }
        }
    }
    return comuns;
}

int lista_interesses_para_array(const ListaInteresses *lista, char destino[][MAX_INTERESSE], int max_elementos) {
    if (lista == NULL || destino == NULL || max_elementos <= 0) {
        return 0;
    }
    int n = 0;
    for (NoInteresse *atual = lista->inicio; atual != NULL && n < max_elementos; atual = atual->seguinte) {
        strncpy(destino[n], atual->interesse, MAX_INTERESSE - 1);
        destino[n][MAX_INTERESSE - 1] = '\0';
        n++;
    }
    return n;
}

void lista_interesses_imprimir(const ListaInteresses *lista) {
    if (lista == NULL || lista->inicio == NULL) {
        printf("(sem interesses)");
        return;
    }
    int primeiro = 1;
    for (NoInteresse *atual = lista->inicio; atual != NULL; atual = atual->seguinte) {
        if (!primeiro) {
            printf(", ");
        }
        printf("%s", atual->interesse);
        primeiro = 0;
    }
}