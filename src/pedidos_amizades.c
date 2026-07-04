#include "pedidos_amizade.h"
#include <stddef.h>
#include <stdlib.h>


GestorPedidos *pedidos_criar(void) {
    GestorPedidos *gp = malloc(sizeof(GestorPedidos));
    if (gp == NULL) {
        return NULL;
    }
    gp->inicio = NULL;
    gp->fim = NULL;
    gp->num_pedidos = 0;
    return gp;
}

void pedidos_destruir(GestorPedidos *gp) {
    if (gp == NULL) {
        return;
    }
    NoPedido *atual = gp->inicio;
    while (atual != NULL) {
        NoPedido *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
    free(gp);
}

int pedidos_existe_pendente(const GestorPedidos *gp, int id_a, int id_b) {
    if (gp == NULL) {
        return 0;
    }
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->estado == PEDIDO_PENDENTE &&
            ((atual->id_remetente == id_a && atual->id_destinatario == id_b) ||
             (atual->id_remetente == id_b && atual->id_destinatario == id_a))) {
            return 1;
        }
    }
    return 0;
}

/* Procura o nó de um pedido PENDENTE entre remetente->destinatario
 * (sentido exato, não nos dois sentidos). Função auxiliar interna. */
static NoPedido *procurar_pendente(GestorPedidos *gp, int id_remetente, int id_destinatario) {
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->estado == PEDIDO_PENDENTE &&
            atual->id_remetente == id_remetente &&
            atual->id_destinatario == id_destinatario) {
            return atual;
        }
    }
    return NULL;
}

int pedidos_enviar(GestorPedidos *gp, int id_remetente, int id_destinatario) {
    if (gp == NULL || id_remetente < 0 || id_destinatario < 0 || id_remetente == id_destinatario) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (pedidos_existe_pendente(gp, id_remetente, id_destinatario)) {
        return ERRO_JA_EXISTE;
    }

    NoPedido *novo = malloc(sizeof(NoPedido));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->id_remetente = id_remetente;
    novo->id_destinatario = id_destinatario;
    novo->estado = PEDIDO_PENDENTE;
    novo->data_envio = obter_data_hora_atual();
    novo->seguinte = NULL;

    if (gp->fim == NULL) {
        gp->inicio = novo;
        gp->fim = novo;
    } else {
        gp->fim->seguinte = novo;
        gp->fim = novo;
    }
    gp->num_pedidos++;

    return OK;
}

int pedidos_aceitar(GestorPedidos *gp, int id_remetente, int id_destinatario, GrafoAmizades *g) {
    if (gp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    NoPedido *pedido = procurar_pendente(gp, id_remetente, id_destinatario);
    if (pedido == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }
    pedido->estado = PEDIDO_ACEITE;

    if (g != NULL) {
        grafo_inserir_aresta(g, id_remetente, id_destinatario);
    }
    return OK;
}

int pedidos_recusar(GestorPedidos *gp, int id_remetente, int id_destinatario) {
    if (gp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    NoPedido *pedido = procurar_pendente(gp, id_remetente, id_destinatario);
    if (pedido == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }
    pedido->estado = PEDIDO_RECUSADO;
    return OK;
}

int pedidos_cancelar(GestorPedidos *gp, int id_remetente, int id_destinatario) {
    if (gp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    NoPedido *pedido = procurar_pendente(gp, id_remetente, id_destinatario);
    if (pedido == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }
    pedido->estado = PEDIDO_CANCELADO;
    return OK;
}

PedidoAmizade *pedidos_listar_recebidos(const GestorPedidos *gp, int id_destinatario, int *num) {
    if (num != NULL) *num = 0;
    if (gp == NULL) {
        return NULL;
    }

    int count = 0;
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->estado == PEDIDO_PENDENTE && atual->id_destinatario == id_destinatario) {
            count++;
        }
    }
    if (count == 0) {
        return NULL;
    }

    PedidoAmizade *resultado = malloc((size_t) count * sizeof(PedidoAmizade));
    if (resultado == NULL) {
        return NULL;
    }

    int i = 0;
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->estado == PEDIDO_PENDENTE && atual->id_destinatario == id_destinatario) {
            resultado[i].id_remetente = atual->id_remetente;
            resultado[i].id_destinatario = atual->id_destinatario;
            resultado[i].estado = atual->estado;
            resultado[i].data_envio = atual->data_envio;
            i++;
        }
    }
    if (num != NULL) *num = i;
    return resultado;
}

PedidoAmizade *pedidos_listar_enviados(const GestorPedidos *gp, int id_remetente, int *num) {
    if (num != NULL) *num = 0;
    if (gp == NULL) {
        return NULL;
    }

    int count = 0;
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->estado == PEDIDO_PENDENTE && atual->id_remetente == id_remetente) {
            count++;
        }
    }
    if (count == 0) {
        return NULL;
    }

    PedidoAmizade *resultado = malloc((size_t) count * sizeof(PedidoAmizade));
    if (resultado == NULL) {
        return NULL;
    }

    int i = 0;
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->estado == PEDIDO_PENDENTE && atual->id_remetente == id_remetente) {
            resultado[i].id_remetente = atual->id_remetente;
            resultado[i].id_destinatario = atual->id_destinatario;
            resultado[i].estado = atual->estado;
            resultado[i].data_envio = atual->data_envio;
            i++;
        }
    }
    if (num != NULL) *num = i;
    return resultado;
}