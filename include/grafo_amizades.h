/**
 * grafo_amizades.h
 * -----------------------------------------------------------------------
 * TAD GrafoAmizades — grafo não-dirigido (lista de adjacências) onde
 * cada vértice é o id de um utilizador e cada aresta representa uma
 * amizade.
 * -----------------------------------------------------------------------
 */
 
#ifndef GRAFO_AMIZADES_H
#define GRAFO_AMIZADES_H
 
#include "utils.h"
#include "TAD.h"
 
/* Cria um grafo capaz de suportar até 'capacidade_inicial' vértices
 * (pode crescer dinamicamente, dependendo da implementação escolhida). */
GrafoAmizades *grafo_criar(int capacidade_inicial);
 
/* Liberta toda a memória do grafo (vértices e arestas). */
void grafo_destruir(GrafoAmizades *g);
 
/* Garante que o id passa a existir como vértice do grafo (chamado
 * quando um novo utilizador é criado). */
int grafo_adicionar_vertice(GrafoAmizades *g, int id);
 
/* Remove um vértice e todas as arestas associadas (ex.: ao eliminar conta). */
int grafo_remover_vertice(GrafoAmizades *g, int id);
 
/* ------------------------------ Arestas (amizades) ----------------------- */
 
/* Cria a amizade (aresta) entre dois utilizadores. Não dirigido:
 * afeta as listas de adjacência de ambos. */
int grafo_inserir_aresta(GrafoAmizades *g, int id_a, int id_b);
 
/* Remove a amizade entre dois utilizadores. */
int grafo_remover_aresta(GrafoAmizades *g, int id_a, int id_b);
 
/* Verifica se dois utilizadores são amigos diretos. */
int grafo_existe_ligacao(const GrafoAmizades *g, int id_a, int id_b);
 
/* Número de amigos diretos de um utilizador (grau do vértice). */
int grafo_grau(const GrafoAmizades *g, int id);
 
/* Devolve um array (alocado, a libertar pelo chamador) com os ids dos
 * amigos diretos de 'id'. 'num_amigos' recebe o tamanho do array. */
int *grafo_listar_amigos(const GrafoAmizades *g, int id, int *num_amigos);
 
/* Devolve os ids dos amigos em comum entre dois utilizadores. */
int *grafo_amigos_comuns(const GrafoAmizades *g, int id_a, int id_b, int *num_comuns);
 
/* ------------------------------ Percursos --------------------------------- */
 
/* Percorre o grafo em largura (BFS) a partir de 'origem'.
 * Usado para sugerir amigos próximos e para calcular distâncias.
 * Devolve a ordem de visita; 'num_visitados' recebe o tamanho. */
int *grafo_bfs(const GrafoAmizades *g, int origem, int *num_visitados);
 
/* Percorre o grafo em profundidade (DFS) a partir de 'origem'.
 * Usado para explorar a rede social. */
int *grafo_dfs(const GrafoAmizades *g, int origem, int *num_visitados);
 
/* Distância (nº de "saltos") mais curta entre dois utilizadores,
 * calculada via BFS. Retorna -1 se não houver caminho. */
int grafo_distancia(const GrafoAmizades *g, int id_a, int id_b);
 
/* Devolve o caminho mais curto entre dois utilizadores (sequência de
 * ids, do mais curto possível). 'tamanho' recebe o nº de elementos. */
int *grafo_caminho_mais_curto(const GrafoAmizades *g, int id_origem, int id_destino, int *tamanho);
 
/* Devolve os ids de "amigos de amigos" (utilizadores a distância 2,
 * ainda não amigos diretos) — base para o critério 4 da recomendação. */
int *grafo_amigos_de_amigos(const GrafoAmizades *g, int id, int *num_resultados);
 
/* ------------------------------ Estatísticas auxiliares ------------------- */
 
/* Id do utilizador com maior número de amigos (maior grau) no grafo. */
int grafo_utilizador_mais_amigos(const GrafoAmizades *g);
 
/* Número total de amizades (arestas) no grafo. */
int grafo_contar_amizades(const GrafoAmizades *g);
 
#endif /* GRAFO_AMIZADES_H */