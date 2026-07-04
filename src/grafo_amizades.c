#include "grafo_amizades.h"
#include "pilha.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/* Garante que o array de vértices tem espaço para o índice 'id',
 * fazendo crescer (realloc) e inicializando as novas posições. */
static int garantir_capacidade(GrafoAmizades *g, int id) {
    if (id < g->capacidade) {
        return OK;
    }
    int nova_capacidade = id + 1;
    Vertice *novo_array = realloc(g->vertices, (size_t) nova_capacidade * sizeof(Vertice));
    if (novo_array == NULL) {
        return ERRO_MEMORIA;
    }
    for (int i = g->capacidade; i < nova_capacidade; i++) {
        novo_array[i].id = i;
        novo_array[i].usado = 0;
        novo_array[i].grau = 0;
        novo_array[i].amigos = NULL;
    }
    g->vertices = novo_array;
    g->capacidade = nova_capacidade;
    return OK;
}

static int vertice_valido(const GrafoAmizades *g, int id) {
    return g != NULL && id >= 0 && id < g->capacidade && g->vertices[id].usado;
}


GrafoAmizades *grafo_criar(int capacidade_inicial) {
    if (capacidade_inicial <= 0) {
        capacidade_inicial = 16;
    }

    GrafoAmizades *g = malloc(sizeof(GrafoAmizades));
    if (g == NULL) {
        return NULL;
    }

    g->vertices = calloc((size_t) capacidade_inicial, sizeof(Vertice));
    if (g->vertices == NULL) {
        free(g);
        return NULL;
    }
    for (int i = 0; i < capacidade_inicial; i++) {
        g->vertices[i].id = i;
        g->vertices[i].usado = 0;
        g->vertices[i].grau = 0;
        g->vertices[i].amigos = NULL;
    }

    g->capacidade = capacidade_inicial;
    g->num_vertices = 0;
    g->num_arestas = 0;
    return g;
}

void grafo_destruir(GrafoAmizades *g) {
    if (g == NULL) {
        return;
    }
    for (int i = 0; i < g->capacidade; i++) {
        NoAdjacencia *atual = g->vertices[i].amigos;
        while (atual != NULL) {
            NoAdjacencia *seguinte = atual->seguinte;
            free(atual);
            atual = seguinte;
        }
    }
    free(g->vertices);
    free(g);
}

int grafo_adicionar_vertice(GrafoAmizades *g, int id) {
    if (g == NULL || id < 0) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (garantir_capacidade(g, id) != OK) {
        return ERRO_MEMORIA;
    }
    if (g->vertices[id].usado) {
        return OK; /* já existe, operação idempotente */
    }
    g->vertices[id].usado = 1;
    g->vertices[id].id = id;
    g->vertices[id].grau = 0;
    g->vertices[id].amigos = NULL;
    g->num_vertices++;
    return OK;
}

int grafo_remover_vertice(GrafoAmizades *g, int id) {
    if (!vertice_valido(g, id)) {
        return ERRO_NAO_ENCONTRADO;
    }
    /* remove todas as arestas ligadas a este vértice */
    while (g->vertices[id].amigos != NULL) {
        grafo_remover_aresta(g, id, g->vertices[id].amigos->id_amigo);
    }
    g->vertices[id].usado = 0;
    g->num_vertices--;
    return OK;
}

int grafo_inserir_aresta(GrafoAmizades *g, int id_a, int id_b) {
    if (g == NULL || id_a < 0 || id_b < 0 || id_a == id_b) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (grafo_adicionar_vertice(g, id_a) != OK || grafo_adicionar_vertice(g, id_b) != OK) {
        return ERRO_MEMORIA;
    }
    if (grafo_existe_ligacao(g, id_a, id_b)) {
        return ERRO_JA_EXISTE;
    }

    NoAdjacencia *no_b = malloc(sizeof(NoAdjacencia));
    NoAdjacencia *no_a = malloc(sizeof(NoAdjacencia));
    if (no_b == NULL || no_a == NULL) {
        free(no_b);
        free(no_a);
        return ERRO_MEMORIA;
    }

    no_b->id_amigo = id_b;
    no_b->seguinte = g->vertices[id_a].amigos;
    g->vertices[id_a].amigos = no_b;
    g->vertices[id_a].grau++;

    no_a->id_amigo = id_a;
    no_a->seguinte = g->vertices[id_b].amigos;
    g->vertices[id_b].amigos = no_a;
    g->vertices[id_b].grau++;

    g->num_arestas++;
    return OK;
}

int grafo_remover_aresta(GrafoAmizades *g, int id_a, int id_b) {
    if (!vertice_valido(g, id_a) || !vertice_valido(g, id_b)) {
        return ERRO_NAO_ENCONTRADO;
    }

    int removido_a = 0, removido_b = 0;

    NoAdjacencia *atual = g->vertices[id_a].amigos;
    NoAdjacencia *anterior = NULL;
    while (atual != NULL) {
        if (atual->id_amigo == id_b) {
            if (anterior == NULL) g->vertices[id_a].amigos = atual->seguinte;
            else anterior->seguinte = atual->seguinte;
            free(atual);
            g->vertices[id_a].grau--;
            removido_a = 1;
            break;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    atual = g->vertices[id_b].amigos;
    anterior = NULL;
    while (atual != NULL) {
        if (atual->id_amigo == id_a) {
            if (anterior == NULL) g->vertices[id_b].amigos = atual->seguinte;
            else anterior->seguinte = atual->seguinte;
            free(atual);
            g->vertices[id_b].grau--;
            removido_b = 1;
            break;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    if (!removido_a && !removido_b) {
        return ERRO_NAO_ENCONTRADO;
    }
    g->num_arestas--;
    return OK;
}

int grafo_existe_ligacao(const GrafoAmizades *g, int id_a, int id_b) {
    if (!vertice_valido(g, id_a) || !vertice_valido(g, id_b)) {
        return 0;
    }
    for (NoAdjacencia *atual = g->vertices[id_a].amigos; atual != NULL; atual = atual->seguinte) {
        if (atual->id_amigo == id_b) {
            return 1;
        }
    }
    return 0;
}

int grafo_grau(const GrafoAmizades *g, int id) {
    if (!vertice_valido(g, id)) {
        return 0;
    }
    return g->vertices[id].grau;
}

int *grafo_listar_amigos(const GrafoAmizades *g, int id, int *num_amigos) {
    if (!vertice_valido(g, id)) {
        if (num_amigos != NULL) *num_amigos = 0;
        return NULL;
    }
    int grau = g->vertices[id].grau;
    if (grau == 0) {
        if (num_amigos != NULL) *num_amigos = 0;
        return NULL;
    }
    int *resultado = malloc((size_t) grau * sizeof(int));
    if (resultado == NULL) {
        if (num_amigos != NULL) *num_amigos = 0;
        return NULL;
    }
    int i = 0;
    for (NoAdjacencia *atual = g->vertices[id].amigos; atual != NULL; atual = atual->seguinte) {
        resultado[i++] = atual->id_amigo;
    }
    if (num_amigos != NULL) *num_amigos = i;
    return resultado;
}

int *grafo_amigos_comuns(const GrafoAmizades *g, int id_a, int id_b, int *num_comuns) {
    if (num_comuns != NULL) *num_comuns = 0;
    if (!vertice_valido(g, id_a) || !vertice_valido(g, id_b)) {
        return NULL;
    }

    /* 1ª passagem: contar */
    int count = 0;
    for (NoAdjacencia *a = g->vertices[id_a].amigos; a != NULL; a = a->seguinte) {
        if (grafo_existe_ligacao(g, id_b, a->id_amigo)) {
            count++;
        }
    }
    if (count == 0) {
        return NULL;
    }

    /* 2ª passagem: preencher */
    int *resultado = malloc((size_t) count * sizeof(int));
    if (resultado == NULL) {
        return NULL;
    }
    int i = 0;
    for (NoAdjacencia *a = g->vertices[id_a].amigos; a != NULL; a = a->seguinte) {
        if (grafo_existe_ligacao(g, id_b, a->id_amigo)) {
            resultado[i++] = a->id_amigo;
        }
    }
    if (num_comuns != NULL) *num_comuns = i;
    return resultado;
}

int *grafo_bfs(const GrafoAmizades *g, int origem, int *num_visitados) {
    if (num_visitados != NULL) *num_visitados = 0;
    if (!vertice_valido(g, origem)) {
        return NULL;
    }

    int *visitado = calloc((size_t) g->capacidade, sizeof(int));
    int *fila = malloc((size_t) g->capacidade * sizeof(int));
    int *resultado = malloc((size_t) g->capacidade * sizeof(int));
    if (visitado == NULL || fila == NULL || resultado == NULL) {
        free(visitado); free(fila); free(resultado);
        return NULL;
    }

    int ini = 0, fim = 0, n = 0;
    fila[fim++] = origem;
    visitado[origem] = 1;

    while (ini < fim) {
        int atual = fila[ini++];
        resultado[n++] = atual;
        for (NoAdjacencia *vizinho = g->vertices[atual].amigos; vizinho != NULL; vizinho = vizinho->seguinte) {
            if (!visitado[vizinho->id_amigo]) {
                visitado[vizinho->id_amigo] = 1;
                fila[fim++] = vizinho->id_amigo;
            }
        }
    }

    free(visitado);
    free(fila);
    if (num_visitados != NULL) *num_visitados = n;

    int *final = realloc(resultado, (size_t) n * sizeof(int));
    return (final != NULL) ? final : resultado;
}

int *grafo_dfs(const GrafoAmizades *g, int origem, int *num_visitados) {
    if (num_visitados != NULL) *num_visitados = 0;
    if (!vertice_valido(g, origem)) {
        return NULL;
    }

    int *visitado = calloc((size_t) g->capacidade, sizeof(int));
    int *resultado = malloc((size_t) g->capacidade * sizeof(int));
    if (visitado == NULL || resultado == NULL) {
        free(visitado); free(resultado);
        return NULL;
    }

    /* DFS iterativo, reaproveitando o TAD Pilha */
    Pilha *pilha = pilha_criar();
    int n = 0;

    int *valor = malloc(sizeof(int));
    *valor = origem;
    pilha_empilhar(pilha, valor);

    while (!pilha_vazia(pilha)) {
        int *p = pilha_desempilhar(pilha);
        int atual = *p;
        free(p);

        if (visitado[atual]) {
            continue;
        }
        visitado[atual] = 1;
        resultado[n++] = atual;

        for (NoAdjacencia *vizinho = g->vertices[atual].amigos; vizinho != NULL; vizinho = vizinho->seguinte) {
            if (!visitado[vizinho->id_amigo]) {
                int *v = malloc(sizeof(int));
                *v = vizinho->id_amigo;
                pilha_empilhar(pilha, v);
            }
        }
    }

    pilha_destruir(pilha, 1);
    free(visitado);
    if (num_visitados != NULL) *num_visitados = n;

    int *final = realloc(resultado, (size_t) n * sizeof(int));
    return (final != NULL) ? final : resultado;
}

int grafo_distancia(const GrafoAmizades *g, int id_a, int id_b) {
    if (!vertice_valido(g, id_a) || !vertice_valido(g, id_b)) {
        return -1;
    }
    if (id_a == id_b) {
        return 0;
    }

    int *dist = malloc((size_t) g->capacidade * sizeof(int));
    int *fila = malloc((size_t) g->capacidade * sizeof(int));
    if (dist == NULL || fila == NULL) {
        free(dist); free(fila);
        return -1;
    }
    for (int i = 0; i < g->capacidade; i++) dist[i] = -1;

    int ini = 0, fim = 0;
    dist[id_a] = 0;
    fila[fim++] = id_a;

    int resultado = -1;
    while (ini < fim) {
        int atual = fila[ini++];
        for (NoAdjacencia *vizinho = g->vertices[atual].amigos; vizinho != NULL; vizinho = vizinho->seguinte) {
            if (dist[vizinho->id_amigo] == -1) {
                dist[vizinho->id_amigo] = dist[atual] + 1;
                if (vizinho->id_amigo == id_b) {
                    resultado = dist[id_b];
                    goto fim_bfs;
                }
                fila[fim++] = vizinho->id_amigo;
            }
        }
    }
fim_bfs:
    free(dist);
    free(fila);
    return resultado;
}

int *grafo_caminho_mais_curto(const GrafoAmizades *g, int id_origem, int id_destino, int *tamanho) {
    if (tamanho != NULL) *tamanho = 0;
    if (!vertice_valido(g, id_origem) || !vertice_valido(g, id_destino)) {
        return NULL;
    }

    int *anterior = malloc((size_t) g->capacidade * sizeof(int));
    int *visitado = calloc((size_t) g->capacidade, sizeof(int));
    int *fila = malloc((size_t) g->capacidade * sizeof(int));
    if (anterior == NULL || visitado == NULL || fila == NULL) {
        free(anterior); free(visitado); free(fila);
        return NULL;
    }
    for (int i = 0; i < g->capacidade; i++) anterior[i] = -1;

    int ini = 0, fim = 0;
    visitado[id_origem] = 1;
    fila[fim++] = id_origem;
    int encontrado = (id_origem == id_destino);

    while (ini < fim && !encontrado) {
        int atual = fila[ini++];
        for (NoAdjacencia *vizinho = g->vertices[atual].amigos; vizinho != NULL; vizinho = vizinho->seguinte) {
            if (!visitado[vizinho->id_amigo]) {
                visitado[vizinho->id_amigo] = 1;
                anterior[vizinho->id_amigo] = atual;
                if (vizinho->id_amigo == id_destino) {
                    encontrado = 1;
                    break;
                }
                fila[fim++] = vizinho->id_amigo;
            }
        }
    }

    free(visitado);
    free(fila);

    if (!encontrado) {
        free(anterior);
        return NULL;
    }

    int comprimento = 1;
    int atual = id_destino;
    while (atual != id_origem) {
        atual = anterior[atual];
        comprimento++;
    }

    int *caminho = malloc((size_t) comprimento * sizeof(int));
    if (caminho == NULL) {
        free(anterior);
        return NULL;
    }
    atual = id_destino;
    for (int i = comprimento - 1; i >= 0; i--) {
        caminho[i] = atual;
        if (atual != id_origem) {
            atual = anterior[atual];
        }
    }
    free(anterior);

    if (tamanho != NULL) *tamanho = comprimento;
    return caminho;
}

int *grafo_amigos_de_amigos(const GrafoAmizades *g, int id, int *num_resultados) {
    if (num_resultados != NULL) *num_resultados = 0;
    if (!vertice_valido(g, id)) {
        return NULL;
    }

    int *dist = malloc((size_t) g->capacidade * sizeof(int));
    int *fila = malloc((size_t) g->capacidade * sizeof(int));
    if (dist == NULL || fila == NULL) {
        free(dist); free(fila);
        return NULL;
    }
    for (int i = 0; i < g->capacidade; i++) dist[i] = -1;

    int ini = 0, fim = 0;
    dist[id] = 0;
    fila[fim++] = id;

    while (ini < fim) {
        int atual = fila[ini++];
        if (dist[atual] >= 2) {
            continue; /* não precisamos de expandir além da distância 2 */
        }
        for (NoAdjacencia *vizinho = g->vertices[atual].amigos; vizinho != NULL; vizinho = vizinho->seguinte) {
            if (dist[vizinho->id_amigo] == -1) {
                dist[vizinho->id_amigo] = dist[atual] + 1;
                fila[fim++] = vizinho->id_amigo;
            }
        }
    }
    free(fila);

    int count = 0;
    for (int i = 0; i < g->capacidade; i++) {
        if (g->vertices[i].usado && dist[i] == 2) count++;
    }

    int *resultado = NULL;
    if (count > 0) {
        resultado = malloc((size_t) count * sizeof(int));
        if (resultado != NULL) {
            int j = 0;
            for (int i = 0; i < g->capacidade; i++) {
                if (g->vertices[i].usado && dist[i] == 2) resultado[j++] = i;
            }
        } else {
            count = 0;
        }
    }

    free(dist);
    if (num_resultados != NULL) *num_resultados = count;
    return resultado;
}

int grafo_utilizador_mais_amigos(const GrafoAmizades *g) {
    if (g == NULL) {
        return -1;
    }
    int melhor_id = -1, melhor_grau = -1;
    for (int i = 0; i < g->capacidade; i++) {
        if (g->vertices[i].usado && g->vertices[i].grau > melhor_grau) {
            melhor_grau = g->vertices[i].grau;
            melhor_id = i;
        }
    }
    return melhor_id;
}

int grafo_contar_amizades(const GrafoAmizades *g) {
    return (g != NULL) ? g->num_arestas : 0;
}