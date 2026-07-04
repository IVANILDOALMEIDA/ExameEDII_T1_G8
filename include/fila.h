/**
 * fila.h
 * -----------------------------------------------------------------------
 * TAD Fila — fila genérica (FIFO), implementada com lista ligada.
 * Estrutura de apoio usada para gerir pedidos de amizade
 * pendentes e notificações, mantendo a ordem de chegada.
 * -----------------------------------------------------------------------
 */
 
#ifndef FILA_H
#define FILA_H
 
#include "utils.h"
#include "TAD.h"
 
/* Cria uma fila vazia. */
Fila *fila_criar(void);
 
/* Liberta a fila. Se 'libertar_elementos' for verdadeiro, chama free()
 * sobre cada elemento guardado. */
void fila_destruir(Fila *fila, int libertar_elementos);
 
/* Insere um elemento no fim da fila (enqueue). */
int fila_enfileirar(Fila *fila, void *elemento);
 
/* Remove e devolve o elemento no início da fila (dequeue).
 * Devolve NULL se a fila estiver vazia. */
void *fila_desenfileirar(Fila *fila);
 
/* Consulta o elemento no início da fila sem o remover. */
void *fila_frente(const Fila *fila);
 
/* Verifica se a fila está vazia. */
int fila_vazia(const Fila *fila);
 
/* Número de elementos na fila. */
int fila_tamanho(const Fila *fila);
 
#endif /* FILA_H */