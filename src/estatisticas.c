#include "estatisticas.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


Estatisticas calcular_estatisticas(const TabelaHash *utilizadores, const GrafoAmizades *g) {
    Estatisticas stats;
    memset(&stats, 0, sizeof(stats));
    stats.id_utilizador_mais_amigos = -1;

    if (utilizadores == NULL) {
        return stats;
    }

    int total = 0;
    Utilizador **lista = hash_listar_todos(utilizadores, &total);
    stats.total_utilizadores = total;
    stats.total_amizades = (g != NULL) ? grafo_contar_amizades(g) : 0;
    stats.id_utilizador_mais_amigos = (g != NULL) ? grafo_utilizador_mais_amigos(g) : -1;

    if (lista == NULL || total == 0) {
        free(lista);
        return stats;
    }

    /* ---------------- universidade mais popular ---------------- */
    int melhor_count = 0;
    for (int i = 0; i < total; i++) {
        int count = 0;
        for (int j = 0; j < total; j++) {
            if (comparar_strings_ci(lista[i]->universidade, lista[j]->universidade) == 0) {
                count++;
            }
        }
        if (count > melhor_count) {
            melhor_count = count;
            strncpy(stats.universidade_mais_popular, lista[i]->universidade, MAX_UNIVERSIDADE - 1);
            stats.universidade_mais_popular[MAX_UNIVERSIDADE - 1] = '\0';
        }
    }

    /* ---------------- curso mais popular ---------------- */
    melhor_count = 0;
    for (int i = 0; i < total; i++) {
        int count = 0;
        for (int j = 0; j < total; j++) {
            if (comparar_strings_ci(lista[i]->curso, lista[j]->curso) == 0) {
                count++;
            }
        }
        if (count > melhor_count) {
            melhor_count = count;
            strncpy(stats.curso_mais_popular, lista[i]->curso, MAX_CURSO - 1);
            stats.curso_mais_popular[MAX_CURSO - 1] = '\0';
        }
    }

    /* ---------------- interesse mais comum ---------------- */
    melhor_count = 0;
    for (int i = 0; i < total; i++) {
        int n_i = lista_interesses_tamanho(lista[i]->interesses);
        if (n_i == 0) continue;

        char (*buffer)[MAX_INTERESSE] = malloc((size_t) n_i * sizeof(*buffer));
        if (buffer == NULL) continue;
        int n = lista_interesses_para_array(lista[i]->interesses, buffer, n_i);

        for (int k = 0; k < n; k++) {
            int count = 0;
            for (int j = 0; j < total; j++) {
                if (lista_interesses_existe(lista[j]->interesses, buffer[k])) {
                    count++;
                }
            }
            if (count > melhor_count) {
                melhor_count = count;
                strncpy(stats.interesse_mais_comum, buffer[k], MAX_INTERESSE - 1);
                stats.interesse_mais_comum[MAX_INTERESSE - 1] = '\0';
            }
        }
        free(buffer);
    }

    free(lista);
    return stats;
}

void estatisticas_imprimir(const Estatisticas *stats) {
    if (stats == NULL) {
        return;
    }
    printf("========== ESTATÍSTICAS DO UNICONNECT ==========\n");
    printf("Total de utilizadores: %d\n", stats->total_utilizadores);
    printf("Total de amizades: %d\n", stats->total_amizades);
    if (stats->id_utilizador_mais_amigos >= 0) {
        printf("Utilizador com mais amigos: id %d\n", stats->id_utilizador_mais_amigos);
    } else {
        printf("Utilizador com mais amigos: (sem dados)\n");
    }
    printf("Universidade com mais estudantes: %s\n",
           stats->universidade_mais_popular[0] != '\0' ? stats->universidade_mais_popular : "(sem dados)");
    printf("Curso mais popular: %s\n",
           stats->curso_mais_popular[0] != '\0' ? stats->curso_mais_popular : "(sem dados)");
    printf("Interesse mais comum: %s\n",
           stats->interesse_mais_comum[0] != '\0' ? stats->interesse_mais_comum : "(sem dados)");
    printf("==================================================\n");
}