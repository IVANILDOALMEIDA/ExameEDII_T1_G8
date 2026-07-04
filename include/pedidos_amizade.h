/**
 * pedidos_amizade.h
 * -----------------------------------------------------------------------
 * TAD GestorPedidos — gere o ciclo de vida dos pedidos de amizade
 * (enviar, aceitar, recusar, cancelar), apoiando-se no TAD Fila para
 * manter os pedidos pendentes por ordem de chegada.
 * Ao aceitar um pedido, este módulo cria a aresta correspondente no
 * GrafoAmizades — por isso depende desse TAD.
 * -----------------------------------------------------------------------
 */
 
#ifndef PEDIDOS_AMIZADE_H
#define PEDIDOS_AMIZADE_H
 
#include "utils.h"
#include "grafo_amizades.h"
#include "TAD.h"
 
  
/* Cria o gestor de pedidos (vazio). */
GestorPedidos *pedidos_criar(void);
 
/* Liberta toda a memória do gestor. */
void pedidos_destruir(GestorPedidos *gp);
 
/* Envia um novo pedido de amizade de id_remetente para id_destinatario.
 * Retorna ERRO_JA_EXISTE se já houver um pedido pendente entre ambos. */
int pedidos_enviar(GestorPedidos *gp, int id_remetente, int id_destinatario);
 
/* Aceita um pedido pendente: atualiza o estado e insere a aresta
 * correspondente no grafo de amizades fornecido. */
int pedidos_aceitar(GestorPedidos *gp, int id_remetente, int id_destinatario, GrafoAmizades *g);
 
/* Recusa um pedido pendente (não cria amizade). */
int pedidos_recusar(GestorPedidos *gp, int id_remetente, int id_destinatario);
 
/* Cancela um pedido que o próprio remetente tenha enviado. */
int pedidos_cancelar(GestorPedidos *gp, int id_remetente, int id_destinatario);
 
/* Verifica se já existe um pedido pendente entre dois utilizadores
 * (em qualquer sentido). */
int pedidos_existe_pendente(const GestorPedidos *gp, int id_a, int id_b);
 
/* Lista os pedidos pendentes recebidos por um utilizador.
 * Devolve um array alocado (a libertar pelo chamador); 'num' recebe o tamanho. */
PedidoAmizade *pedidos_listar_recebidos(const GestorPedidos *gp, int id_destinatario, int *num);
 
/* Lista os pedidos pendentes enviados por um utilizador. */
PedidoAmizade *pedidos_listar_enviados(const GestorPedidos *gp, int id_remetente, int *num);
 
#endif /* PEDIDOS_AMIZADE_H */