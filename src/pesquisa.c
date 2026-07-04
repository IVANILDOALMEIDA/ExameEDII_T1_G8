#include "pesquisa.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Verifica se 'termo' ocorre como substring de 'texto', ignorando
 * maiúsculas/minúsculas. Um termo vazio corresponde sempre. */
static int contem_substring_ci(const char *texto, const char *termo) {
    if (texto == NULL || termo == NULL) {
        return 0;
    }
    if (termo[0] == '\0') {
        return 1;
    }
    size_t lt = strlen(texto);
    size_t lp = strlen(termo);
    if (lp > lt) {
        return 0;
    }
    for (size_t i = 0; i + lp <= lt; i++) {
        size_t j = 0;
        while (j < lp && tolower((unsigned char) texto[i + j]) == tolower((unsigned char) termo[j])) {
            j++;
        }
        if (j == lp) {
            return 1;
        }
    }
    return 0;
}

/* Verifica se algum interesse do utilizador contém 'termo'. */
static int utilizador_tem_interesse_parecido(const Utilizador *u, const char *termo) {
    if (u == NULL || termo == NULL) {
        return 0;
    }
    int n = lista_interesses_tamanho(u->interesses);
    if (n == 0) {
        return 0;
    }
    char (*buffer)[MAX_INTERESSE] = malloc((size_t) n * sizeof(*buffer));
    if (buffer == NULL) {
        return 0;
    }
    int total = lista_interesses_para_array(u->interesses, buffer, n);
    int encontrado = 0;
    for (int i = 0; i < total; i++) {
        if (contem_substring_ci(buffer[i], termo)) {
            encontrado = 1;
            break;
        }
    }
    free(buffer);
    return encontrado;
}

/* Filtra utilizadores segundo um predicado e devolve o array de
 * resultados (alocado). Função auxiliar genérica interna. */
static Utilizador **filtrar(const TabelaHash *utilizadores,
                             int (*predicado)(const Utilizador *, const char *),
                             const char *termo,
                             int *num_resultados) {
    if (num_resultados != NULL) *num_resultados = 0;
    if (utilizadores == NULL || termo == NULL) {
        return NULL;
    }

    int total = 0;
    Utilizador **todos = hash_listar_todos(utilizadores, &total);
    if (todos == NULL || total == 0) {
        free(todos);
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < total; i++) {
        if (predicado(todos[i], termo)) count++;
    }
    if (count == 0) {
        free(todos);
        return NULL;
    }

    Utilizador **resultado = malloc((size_t) count * sizeof(Utilizador *));
    if (resultado == NULL) {
        free(todos);
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < total; i++) {
        if (predicado(todos[i], termo)) {
            resultado[j++] = todos[i];
        }
    }
    free(todos);

    if (num_resultados != NULL) *num_resultados = j;
    return resultado;
}

static int pred_nome(const Utilizador *u, const char *termo) { return contem_substring_ci(u->nome_completo, termo); }
static int pred_universidade(const Utilizador *u, const char *termo) { return contem_substring_ci(u->universidade, termo); }
static int pred_curso(const Utilizador *u, const char *termo) { return contem_substring_ci(u->curso, termo); }
static int pred_cidade(const Utilizador *u, const char *termo) { return contem_substring_ci(u->cidade, termo); }
static int pred_interesse(const Utilizador *u, const char *termo) { return utilizador_tem_interesse_parecido(u, termo); }

Utilizador **pesquisar_por_nome(const TabelaHash *utilizadores, const char *termo, int *num_resultados) {
    return filtrar(utilizadores, pred_nome, termo, num_resultados);
}

Utilizador **pesquisar_por_universidade(const TabelaHash *utilizadores, const char *universidade, int *num_resultados) {
    return filtrar(utilizadores, pred_universidade, universidade, num_resultados);
}

Utilizador **pesquisar_por_curso(const TabelaHash *utilizadores, const char *curso, int *num_resultados) {
    return filtrar(utilizadores, pred_curso, curso, num_resultados);
}

Utilizador **pesquisar_por_interesse(const TabelaHash *utilizadores, const char *interesse, int *num_resultados) {
    return filtrar(utilizadores, pred_interesse, interesse, num_resultados);
}

Utilizador **pesquisar_por_cidade(const TabelaHash *utilizadores, const char *cidade, int *num_resultados) {
    return filtrar(utilizadores, pred_cidade, cidade, num_resultados);
}

Utilizador **pesquisar_avancada(const TabelaHash *utilizadores,
                                 const char *nome,
                                 const char *universidade,
                                 const char *curso,
                                 const char *interesse,
                                 const char *cidade,
                                 int *num_resultados) {
    if (num_resultados != NULL) *num_resultados = 0;
    if (utilizadores == NULL) {
        return NULL;
    }

    int total = 0;
    Utilizador **todos = hash_listar_todos(utilizadores, &total);
    if (todos == NULL || total == 0) {
        free(todos);
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < total; i++) {
        Utilizador *u = todos[i];
        if (nome != NULL && !contem_substring_ci(u->nome_completo, nome)) continue;
        if (universidade != NULL && !contem_substring_ci(u->universidade, universidade)) continue;
        if (curso != NULL && !contem_substring_ci(u->curso, curso)) continue;
        if (cidade != NULL && !contem_substring_ci(u->cidade, cidade)) continue;
        if (interesse != NULL && !utilizador_tem_interesse_parecido(u, interesse)) continue;
        count++;
    }
    if (count == 0) {
        free(todos);
        return NULL;
    }

    Utilizador **resultado = malloc((size_t) count * sizeof(Utilizador *));
    if (resultado == NULL) {
        free(todos);
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < total; i++) {
        Utilizador *u = todos[i];
        if (nome != NULL && !contem_substring_ci(u->nome_completo, nome)) continue;
        if (universidade != NULL && !contem_substring_ci(u->universidade, universidade)) continue;
        if (curso != NULL && !contem_substring_ci(u->curso, curso)) continue;
        if (cidade != NULL && !contem_substring_ci(u->cidade, cidade)) continue;
        if (interesse != NULL && !utilizador_tem_interesse_parecido(u, interesse)) continue;
        resultado[j++] = u;
    }
    free(todos);

    if (num_resultados != NULL) *num_resultados = j;
    return resultado;
}