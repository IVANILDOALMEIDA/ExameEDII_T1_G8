#include "mensagens.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


ListaMensagens *mensagens_criar(void) {
    ListaMensagens *lm = malloc(sizeof(ListaMensagens));
    if (lm == NULL) {
        return NULL;
    }
    lm->inicio = NULL;
    lm->fim = NULL;
    lm->num_mensagens = 0;
    return lm;
}

void mensagens_destruir(ListaMensagens *lm) {
    if (lm == NULL) {
        return;
    }
    NoMensagem *atual = lm->inicio;
    while (atual != NULL) {
        NoMensagem *seguinte = atual->seguinte;
        free(atual);
        atual = seguinte;
    }
    free(lm);
}

int mensagens_enviar(ListaMensagens *lm, int id_remetente, int id_destinatario, const char *conteudo) {
    if (lm == NULL || conteudo == NULL || conteudo[0] == '\0' || id_remetente == id_destinatario) {
        return ERRO_PARAMETRO_INVALIDO;
    }

    NoMensagem *novo = malloc(sizeof(NoMensagem));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->id_remetente = id_remetente;
    novo->id_destinatario = id_destinatario;
    strncpy(novo->conteudo, conteudo, MAX_CONTEUDO - 1);
    novo->conteudo[MAX_CONTEUDO - 1] = '\0';
    novo->data_hora = obter_data_hora_atual();
    novo->seguinte = NULL;

    if (lm->fim == NULL) {
        lm->inicio = novo;
        lm->fim = novo;
    } else {
        lm->fim->seguinte = novo;
        lm->fim = novo;
    }
    lm->num_mensagens++;

    return OK;
}

static int pertence_a_conversa(const NoMensagem *no, int id_a, int id_b) {
    return (no->id_remetente == id_a && no->id_destinatario == id_b) ||
           (no->id_remetente == id_b && no->id_destinatario == id_a);
}

MensagemInfo *mensagens_obter_conversa(const ListaMensagens *lm, int id_a, int id_b, int *num) {
    if (num != NULL) *num = 0;
    if (lm == NULL) {
        return NULL;
    }

    int count = 0;
    for (NoMensagem *atual = lm->inicio; atual != NULL; atual = atual->seguinte) {
        if (pertence_a_conversa(atual, id_a, id_b)) count++;
    }
    if (count == 0) {
        return NULL;
    }

    MensagemInfo *resultado = malloc((size_t) count * sizeof(MensagemInfo));
    if (resultado == NULL) {
        return NULL;
    }

    int i = 0;
    /* a lista já está em ordem cronológica (inserção sempre no fim) */
    for (NoMensagem *atual = lm->inicio; atual != NULL; atual = atual->seguinte) {
        if (pertence_a_conversa(atual, id_a, id_b)) {
            resultado[i].id_remetente = atual->id_remetente;
            resultado[i].id_destinatario = atual->id_destinatario;
            strncpy(resultado[i].conteudo, atual->conteudo, MAX_CONTEUDO - 1);
            resultado[i].conteudo[MAX_CONTEUDO - 1] = '\0';
            resultado[i].data_hora = atual->data_hora;
            i++;
        }
    }
    if (num != NULL) *num = i;
    return resultado;
}

int mensagens_apagar_conversa(ListaMensagens *lm, int id_a, int id_b) {
    if (lm == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }

    NoMensagem *atual = lm->inicio;
    NoMensagem *anterior = NULL;
    int removidas = 0;

    while (atual != NULL) {
        if (pertence_a_conversa(atual, id_a, id_b)) {
            NoMensagem *seguinte = atual->seguinte;
            if (anterior == NULL) {
                lm->inicio = seguinte;
            } else {
                anterior->seguinte = seguinte;
            }
            if (atual == lm->fim) {
                lm->fim = anterior;
            }
            free(atual);
            lm->num_mensagens--;
            removidas++;
            atual = seguinte;
        } else {
            anterior = atual;
            atual = atual->seguinte;
        }
    }

    return (removidas > 0) ? OK : ERRO_NAO_ENCONTRADO;
}

int mensagens_contar_conversa(const ListaMensagens *lm, int id_a, int id_b) {
    if (lm == NULL) {
        return 0;
    }
    int count = 0;
    for (NoMensagem *atual = lm->inicio; atual != NULL; atual = atual->seguinte) {
        if (pertence_a_conversa(atual, id_a, id_b)) count++;
    }
    return count;
}

int *mensagens_listar_conversas_de(const ListaMensagens *lm, int id, int *num_conversas) {
    if (num_conversas != NULL) *num_conversas = 0;
    if (lm == NULL) {
        return NULL;
    }

    /* 1ª fase: recolher (em ordem cronológica) o "outro" id de cada
     * mensagem em que 'id' participa. */
    int total_msgs = lm->num_mensagens;
    if (total_msgs == 0) {
        return NULL;
    }
    int *outros_cronologico = malloc((size_t) total_msgs * sizeof(int));
    if (outros_cronologico == NULL) {
        return NULL;
    }
    int n_msgs = 0;
    for (NoMensagem *atual = lm->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->id_remetente == id) {
            outros_cronologico[n_msgs++] = atual->id_destinatario;
        } else if (atual->id_destinatario == id) {
            outros_cronologico[n_msgs++] = atual->id_remetente;
        }
    }

    if (n_msgs == 0) {
        free(outros_cronologico);
        return NULL;
    }

    /* 2ª fase: percorrer do mais recente para o mais antigo,
     * mantendo só a 1ª ocorrência de cada id (= mais recente). */
    int *resultado = malloc((size_t) n_msgs * sizeof(int));
    if (resultado == NULL) {
        free(outros_cronologico);
        return NULL;
    }
    int n_resultado = 0;
    for (int i = n_msgs - 1; i >= 0; i--) {
        int candidato = outros_cronologico[i];
        int ja_existe = 0;
        for (int j = 0; j < n_resultado; j++) {
            if (resultado[j] == candidato) {
                ja_existe = 1;
                break;
            }
        }
        if (!ja_existe) {
            resultado[n_resultado++] = candidato;
        }
    }
    free(outros_cronologico);

    if (num_conversas != NULL) *num_conversas = n_resultado;
    return resultado;
}