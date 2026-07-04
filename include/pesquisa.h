/**
 * pesquisa.h
 * -----------------------------------------------------------------------
 * Módulo de PESQUISA de estudantes por nome, universidade, curso,
 * interesse ou cidade . Opera sobre a TabelaHash de
 * utilizadores (varrimento O(n) com hash_para_cada / hash_listar_todos,
 * pois os critérios de pesquisa não correspondem à chave da tabela).
 * -----------------------------------------------------------------------
 */
 
#ifndef PESQUISA_H
#define PESQUISA_H
 
#include "hash_table.h"
 
/* Cada função devolve um array de apontadores para Utilizador (alocado
 * dinamicamente, a libertar pelo chamador com free() — os Utilizador
 * em si continuam pertencer à TabelaHash); 'num_resultados' recebe o
 * tamanho do array. A pesquisa por texto deve ser case-insensitive e
 * aceitar correspondência parcial (substring). */
 
Utilizador **pesquisar_por_nome(const TabelaHash *utilizadores, const char *termo, int *num_resultados);
Utilizador **pesquisar_por_universidade(const TabelaHash *utilizadores, const char *universidade, int *num_resultados);
Utilizador **pesquisar_por_curso(const TabelaHash *utilizadores, const char *curso, int *num_resultados);
Utilizador **pesquisar_por_interesse(const TabelaHash *utilizadores, const char *interesse, int *num_resultados);
Utilizador **pesquisar_por_cidade(const TabelaHash *utilizadores, const char *cidade, int *num_resultados);
 
/* Pesquisa combinada: aplica vários critérios em simultâneo (campos a
 * NULL são ignorados)*/
Utilizador **pesquisar_avancada(const TabelaHash *utilizadores,
                                 const char *nome,
                                 const char *universidade,
                                 const char *curso,
                                 const char *interesse,
                                 const char *cidade,
                                 int *num_resultados);
 
#endif /* PESQUISA_H */