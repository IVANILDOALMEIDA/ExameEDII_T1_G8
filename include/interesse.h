/**
 * interesse.h
 * -----------------------------------------------------------------------
 * TAD ListaInteresses — lista ligada simples de strings, usada para
 * guardar os interesses de cada utilizador (desporto, IA, música, ...).
 * Estrutura de dados: LISTA LIGADA.
 * -----------------------------------------------------------------------
 */
 
#ifndef INTERESSE_H
#define INTERESSE_H
 
#include "utils.h"
#include "TAD.h" 


 
/* Cria uma lista de interesses vazia. */
ListaInteresses *lista_interesses_criar(void);
 
/* Liberta toda a memória ocupada pela lista. */
void lista_interesses_destruir(ListaInteresses *lista);
 
/* Insere um novo interesse (evita duplicados).
 * Retorna OK, ERRO_JA_EXISTE ou ERRO_MEMORIA. */
int lista_interesses_inserir(ListaInteresses *lista, const char *interesse);
 
/* Remove um interesse pelo nome.
 * Retorna OK ou ERRO_NAO_ENCONTRADO. */
int lista_interesses_remover(ListaInteresses *lista, const char *interesse);
 
/* Verifica se um interesse existe na lista. */
int lista_interesses_existe(const ListaInteresses *lista, const char *interesse);
 
/* Número de interesses na lista. */
int lista_interesses_tamanho(const ListaInteresses *lista);
 
/* Conta quantos interesses são comuns entre duas listas.
 * Usado no algoritmo de recomendação/afinidade. Complexidade O(n*m). */
int lista_interesses_contar_comuns(const ListaInteresses *a, const ListaInteresses *b);
 
/* Copia os interesses para um array de strings já alocado pelo
 * chamador (de tamanho >= lista_interesses_tamanho()). Útil para
 * persistência em ficheiro (guardar no ficheiro) e para impressão. Retorna nº de elementos copiados. */
int lista_interesses_para_array(const ListaInteresses *lista, char destino[][MAX_INTERESSE], int max_elementos);
 
/* Imprime os interesses, um por linha (ou separados por vírgula). */
void lista_interesses_imprimir(const ListaInteresses *lista);
 
#endif /* INTERESSE_H */
 