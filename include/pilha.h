/**
 * pilha.h
 * -----------------------------------------------------------------------
 * TAD Pilha — pilha genérica (LIFO), implementada com lista ligada.
 * -----------------------------------------------------------------------
 */
 
#ifndef PILHA_H
#define PILHA_H
 
#include "utils.h"
#include "TAD.h"
 
/* Cria uma pilha vazia. */
Pilha *pilha_criar(void);
 
/* Liberta a pilha. Se 'libertar_elementos' for verdadeiro, chama free()
 * sobre cada elemento guardado. */
void pilha_destruir(Pilha *pilha, int libertar_elementos);
 
/* Insere um elemento no topo da pilha (push). */
int pilha_empilhar(Pilha *pilha, void *elemento);
 
/* Remove e devolve o elemento no topo da pilha (pop).
 * Devolve NULL se a pilha estiver vazia. */
void *pilha_desempilhar(Pilha *pilha);
 
/* Consulta o elemento no topo sem o remover. */
void *pilha_topo(const Pilha *pilha);
 
/* Verifica se a pilha está vazia. */
int pilha_vazia(const Pilha *pilha);
 
/* Número de elementos na pilha. */
int pilha_tamanho(const Pilha *pilha);
 
#endif /* PILHA_H */