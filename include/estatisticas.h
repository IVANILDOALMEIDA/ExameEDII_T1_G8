/**
 * estatisticas.h
 * -----------------------------------------------------------------------
 * Módulo de ESTATÍSTICAS globais do sistema: total de
 * utilizadores, total de amizades, utilizador com mais amigos,
 * universidade/curso/interesse mais comuns. Combina TabelaHash e
 * GrafoAmizades; não define um TAD com estado próprio.
 * -----------------------------------------------------------------------
 */
 
#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H
 
#include "utils.h"
#include "hash_table.h"
#include "grafo_amizades.h"
#include "TAD.h"
 
/* Percorre utilizadores e grafo para calcular o resumo estatístico. */
Estatisticas calcular_estatisticas(const TabelaHash *utilizadores, const GrafoAmizades *g);
 
/* Imprime o resumo estatístico de forma legível no terminal. */
void estatisticas_imprimir(const Estatisticas *stats);
 
#endif /* ESTATISTICAS_H */