#include "hash_table.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>



TabelaHash *hash_criar(int capacidade) {
    if (capacidade <= 0) {
        capacidade = 101; /* primo razoável por omissão */
    }

    TabelaHash *tabela = malloc(sizeof(TabelaHash));
    if (tabela == NULL) {
        return NULL;
    }

    tabela->baldes = calloc((size_t) capacidade, sizeof(NoHash *));
    if (tabela->baldes == NULL) {
        free(tabela);
        return NULL;
    }

    tabela->capacidade = capacidade;
    tabela->num_utilizadores = 0;
    return tabela;
}

void hash_destruir(TabelaHash *tabela, int libertar_utilizadores) {
    if (tabela == NULL) {
        return;
    }
    for (int i = 0; i < tabela->capacidade; i++) {
        NoHash *atual = tabela->baldes[i];
        while (atual != NULL) {
            NoHash *seguinte = atual->seguinte;
            if (libertar_utilizadores) {
                utilizador_destruir(atual->utilizador);
            }
            free(atual);
            atual = seguinte;
        }
    }
    free(tabela->baldes);
    free(tabela);
}

unsigned int hash_funcao(const char *chave, int capacidade) {
    if (chave == NULL || capacidade <= 0) {
        return 0;
    }
    /* djb2 */
    unsigned long hash = 5381;
    int c;
    const char *p = chave;
    while ((c = (unsigned char) *p++) != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long) c; /* hash*33 + c */
    }
    return (unsigned int) (hash % (unsigned long) capacidade);
}

int hash_inserir(TabelaHash *tabela, Utilizador *u) {
    if (tabela == NULL || u == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (hash_existe_username(tabela, u->username)) {
        return ERRO_JA_EXISTE;
    }

    unsigned int idx = hash_funcao(u->username, tabela->capacidade);

    NoHash *novo = malloc(sizeof(NoHash));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->utilizador = u;
    novo->seguinte = tabela->baldes[idx];
    tabela->baldes[idx] = novo;
    tabela->num_utilizadores++;

    return OK;
}

int hash_remover(TabelaHash *tabela, const char *username) {
    if (tabela == NULL || username == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    unsigned int idx = hash_funcao(username, tabela->capacidade);

    NoHash *atual = tabela->baldes[idx];
    NoHash *anterior = NULL;

    while (atual != NULL) {
        if (comparar_strings_ci(atual->utilizador->username, username) == 0) {
            if (anterior == NULL) {
                tabela->baldes[idx] = atual->seguinte;
            } else {
                anterior->seguinte = atual->seguinte;
            }
            free(atual);
            tabela->num_utilizadores--;
            return OK;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    return ERRO_NAO_ENCONTRADO;
}

Utilizador *hash_procurar_username(const TabelaHash *tabela, const char *username) {
    if (tabela == NULL || username == NULL) {
        return NULL;
    }
    unsigned int idx = hash_funcao(username, tabela->capacidade);
    for (NoHash *atual = tabela->baldes[idx]; atual != NULL; atual = atual->seguinte) {
        if (comparar_strings_ci(atual->utilizador->username, username) == 0) {
            return atual->utilizador;
        }
    }
    return NULL;
}

Utilizador *hash_procurar_id(const TabelaHash *tabela, int id) {
    if (tabela == NULL) {
        return NULL;
    }
    for (int i = 0; i < tabela->capacidade; i++) {
        for (NoHash *atual = tabela->baldes[i]; atual != NULL; atual = atual->seguinte) {
            if (atual->utilizador->id == id) {
                return atual->utilizador;
            }
        }
    }
    return NULL;
}

int hash_existe_username(const TabelaHash *tabela, const char *username) {
    return hash_procurar_username(tabela, username) != NULL;
}

int hash_contar_utilizadores(const TabelaHash *tabela) {
    return (tabela != NULL) ? tabela->num_utilizadores : 0;
}

void hash_para_cada(const TabelaHash *tabela, void (*callback)(Utilizador *u, void *contexto), void *contexto) {
    if (tabela == NULL || callback == NULL) {
        return;
    }
    for (int i = 0; i < tabela->capacidade; i++) {
        for (NoHash *atual = tabela->baldes[i]; atual != NULL; atual = atual->seguinte) {
            callback(atual->utilizador, contexto);
        }
    }
}

Utilizador **hash_listar_todos(const TabelaHash *tabela, int *num_utilizadores) {
    if (tabela == NULL) {
        if (num_utilizadores != NULL) {
            *num_utilizadores = 0;
        }
        return NULL;
    }

    Utilizador **lista = malloc((size_t) tabela->num_utilizadores * sizeof(Utilizador *));
    if (lista == NULL) {
        if (num_utilizadores != NULL) {
            *num_utilizadores = 0;
        }
        return NULL;
    }

    int n = 0;
    for (int i = 0; i < tabela->capacidade; i++) {
        for (NoHash *atual = tabela->baldes[i]; atual != NULL; atual = atual->seguinte) {
            lista[n++] = atual->utilizador;
        }
    }

    if (num_utilizadores != NULL) {
        *num_utilizadores = n;
    }
    return lista;
}