/**
 * recomendacao.h
 * -----------------------------------------------------------------------
 * Módulo de RECOMENDAÇÃO de amigos e cálculo de afinidade,
 * -----------------------------------------------------------------------
 */
 
#ifndef RECOMENDACAO_H
#define RECOMENDACAO_H
 
#include "hash_table.h"
#include "grafo_amizades.h"
#include "TAD.h"
 
/* Pontuação usada pelo desafio bónus:
 *   mesmo curso          -> +20
 *   mesma universidade   -> +15
 *   interesse em comum   -> +10 cada
 *   amigo em comum       -> +5  cada
 * Calcula a pontuação de afinidade entre dois utilizadores. */
int calcular_pontuacao_afinidade(const Utilizador *a, const Utilizador *b, const GrafoAmizades *g);
 
/* Gera recomendações de amigos para 'id_utilizador', combinando os
 * critérios (interesses comuns, mesmo curso, mesma
 * universidade, amigos de amigos, nº de interesses partilhados).
 * Exclui utilizadores que já são amigos ou o próprio utilizador.
 * Devolve um array ORDENADO por pontuação decrescente (alocado, a
 * libertar pelo chamador); 'num_resultados' recebe o tamanho. */
ResultadoRecomendacao *recomendar_amigos(int id_utilizador,
                                          const TabelaHash *utilizadores,
                                          const GrafoAmizades *g,
                                          int *num_resultados);
 
/* Funcionalidade bónus: devolve os 5 estudantes com maior pontuação de
 * afinidade para 'id_utilizador' (usa calcular_pontuacao_afinidade
 * sobre todos os utilizadores e ordena). */
ResultadoRecomendacao *top5_compatibilidade(int id_utilizador,
                                             const TabelaHash *utilizadores,
                                             const GrafoAmizades *g);
 
#endif /* RECOMENDACAO_H */