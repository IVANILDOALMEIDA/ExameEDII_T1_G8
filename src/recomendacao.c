#include "recomendacao.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/* Pontuação do desafio bónus (secção 10):
 *   mesmo curso          -> +20
 *   mesma universidade   -> +15
 *   interesse em comum   -> +10 cada
 *   amigo em comum       -> +5  cada */
 
int calcular_pontuacao_afinidade(const Utilizador *a, const Utilizador *b, const GrafoAmizades *g) {
    if (a == NULL || b == NULL) {
        return 0;
    }

    int pontos = 0;

    if (comparar_strings_ci(a->curso, b->curso) == 0) {
        pontos += 20;
    }
    if (comparar_strings_ci(a->universidade, b->universidade) == 0) {
        pontos += 15;
    }

    pontos += 10 * lista_interesses_contar_comuns(a->interesses, b->interesses);

    if (g != NULL) {
        int num_comuns = 0;
        int *comuns = grafo_amigos_comuns(g, a->id, b->id, &num_comuns);
        free(comuns);
        pontos += 5 * num_comuns;
    }

    return pontos;
}

/* Comparador para qsort: ordena por pontuação decrescente. */
static int comparar_resultado_desc(const void *pa, const void *pb) {
    const ResultadoRecomendacao *ra = (const ResultadoRecomendacao *) pa;
    const ResultadoRecomendacao *rb = (const ResultadoRecomendacao *) pb;
    return rb->pontuacao - ra->pontuacao;
}

ResultadoRecomendacao *recomendar_amigos(int id_utilizador,
                                          const TabelaHash *utilizadores,
                                          const GrafoAmizades *g,
                                          int *num_resultados) {
    if (num_resultados != NULL) *num_resultados = 0;
    if (utilizadores == NULL) {
        return NULL;
    }

    Utilizador *atual_user = hash_procurar_id(utilizadores, id_utilizador);
    if (atual_user == NULL) {
        return NULL;
    }

    int total = 0;
    Utilizador **todos = hash_listar_todos(utilizadores, &total);
    if (todos == NULL || total == 0) {
        free(todos);
        return NULL;
    }

    ResultadoRecomendacao *temp = malloc((size_t) total * sizeof(ResultadoRecomendacao));
    if (temp == NULL) {
        free(todos);
        return NULL;
    }

    int n = 0;
    for (int i = 0; i < total; i++) {
        Utilizador *candidato = todos[i];
        if (candidato->id == id_utilizador) {
            continue; /* não recomendar o próprio utilizador */
        }
        if (g != NULL && grafo_existe_ligacao(g, id_utilizador, candidato->id)) {
            continue; /* já são amigos */
        }
        temp[n].id_utilizador = candidato->id;
        temp[n].pontuacao = calcular_pontuacao_afinidade(atual_user, candidato, g);
        n++;
    }
    free(todos);

    if (n == 0) {
        free(temp);
        return NULL;
    }

    qsort(temp, (size_t) n, sizeof(ResultadoRecomendacao), comparar_resultado_desc);

    ResultadoRecomendacao *resultado = realloc(temp, (size_t) n * sizeof(ResultadoRecomendacao));
    if (resultado == NULL) {
        resultado = temp; /* realloc para tamanho menor não deve falhar, mas por garantia */
    }

    if (num_resultados != NULL) *num_resultados = n;
    return resultado;
}

/* Contrato: devolve SEMPRE um array com exatamente 5 posições. Se
 * existirem menos de 5 candidatos possíveis, as posições restantes
 * ficam marcadas com id_utilizador = -1 (sentinela "sem mais
 * recomendações") e pontuacao = 0. */
 
ResultadoRecomendacao *top5_compatibilidade(int id_utilizador,
                                             const TabelaHash *utilizadores,
                                             const GrafoAmizades *g) {
    ResultadoRecomendacao *top5 = malloc(5 * sizeof(ResultadoRecomendacao));
    if (top5 == NULL) {
        return NULL;
    }
    for (int i = 0; i < 5; i++) {
        top5[i].id_utilizador = -1;
        top5[i].pontuacao = 0;
    }

    if (utilizadores == NULL) {
        return top5;
    }

    Utilizador *atual_user = hash_procurar_id(utilizadores, id_utilizador);
    if (atual_user == NULL) {
        return top5;
    }

    int total = 0;
    Utilizador **todos = hash_listar_todos(utilizadores, &total);
    if (todos == NULL || total == 0) {
        free(todos);
        return top5;
    }

    ResultadoRecomendacao *temp = malloc((size_t) total * sizeof(ResultadoRecomendacao));
    if (temp == NULL) {
        free(todos);
        return top5;
    }

    int n = 0;
    for (int i = 0; i < total; i++) {
        Utilizador *candidato = todos[i];
        if (candidato->id == id_utilizador) {
            continue;
        }
        temp[n].id_utilizador = candidato->id;
        temp[n].pontuacao = calcular_pontuacao_afinidade(atual_user, candidato, g);
        n++;
    }
    free(todos);

    qsort(temp, (size_t) n, sizeof(ResultadoRecomendacao), comparar_resultado_desc);

    int limite = (n < 5) ? n : 5;
    for (int i = 0; i < limite; i++) {
        top5[i] = temp[i];
    }
    free(temp);

    return top5;
}