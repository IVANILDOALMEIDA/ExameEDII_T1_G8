#include "publicacoes.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/* --------------------------- Funções auxiliares internas ------------------- */

static NoPublicacao *procurar_post(const ListaPublicacoes *lp, int id_post) {
    if (lp == NULL) {
        return NULL;
    }
    for (NoPublicacao *atual = lp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->id_post == id_post) {
            return atual;
        }
    }
    return NULL;
}


ListaPublicacoes *publicacoes_criar(void) {
    ListaPublicacoes *lp = malloc(sizeof(ListaPublicacoes));
    if (lp == NULL) {
        return NULL;
    }
    lp->inicio = NULL;
    lp->num_publicacoes = 0;
    lp->proximo_id_post = 1;
    lp->proximo_id_comentario = 1;
    return lp;
}

void publicacoes_destruir(ListaPublicacoes *lp) {
    if (lp == NULL) {
        return;
    }
    NoPublicacao *atual = lp->inicio;
    while (atual != NULL) {
        NoPublicacao *seguinte_post = atual->seguinte;

        NoLike *l = atual->likes;
        while (l != NULL) {
            NoLike *prox_l = l->seguinte;
            free(l);
            l = prox_l;
        }

        NoComentario *c = atual->comentarios;
        while (c != NULL) {
            NoComentario *prox_c = c->seguinte;
            free(c);
            c = prox_c;
        }

        free(atual);
        atual = seguinte_post;
    }
    free(lp);
}

int publicacoes_criar_post(ListaPublicacoes *lp, int id_autor, const char *conteudo) {
    if (lp == NULL || conteudo == NULL || conteudo[0] == '\0') {
        return ERRO_PARAMETRO_INVALIDO;
    }

    NoPublicacao *novo = malloc(sizeof(NoPublicacao));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }

    novo->id_post = lp->proximo_id_post++;
    novo->id_autor = id_autor;
    strncpy(novo->conteudo, conteudo, MAX_CONTEUDO - 1);
    novo->conteudo[MAX_CONTEUDO - 1] = '\0';
    novo->data_hora = obter_data_hora_atual();
    novo->likes = NULL;
    novo->num_likes = 0;
    novo->comentarios = NULL;
    novo->num_comentarios = 0;

    /* inserção no início: a cabeça da lista é sempre a publicação mais
     * recente, o que simplifica listagens "mais recentes primeiro". */
    novo->seguinte = lp->inicio;
    lp->inicio = novo;
    lp->num_publicacoes++;

    return novo->id_post;
}

int publicacoes_remover_post(ListaPublicacoes *lp, int id_post, int id_autor) {
    if (lp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }

    NoPublicacao *atual = lp->inicio;
    NoPublicacao *anterior = NULL;

    while (atual != NULL) {
        if (atual->id_post == id_post) {
            if (atual->id_autor != id_autor) {
                return ERRO_NAO_AUTORIZADO;
            }
            if (anterior == NULL) {
                lp->inicio = atual->seguinte;
            } else {
                anterior->seguinte = atual->seguinte;
            }

            NoLike *l = atual->likes;
            while (l != NULL) { NoLike *p = l->seguinte; free(l); l = p; }
            NoComentario *c = atual->comentarios;
            while (c != NULL) { NoComentario *p = c->seguinte; free(c); c = p; }

            free(atual);
            lp->num_publicacoes--;
            return OK;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    return ERRO_NAO_ENCONTRADO;
}

int publicacoes_curtir(ListaPublicacoes *lp, int id_post, int id_utilizador) {
    NoPublicacao *post = procurar_post(lp, id_post);
    if (post == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }
    if (publicacoes_utilizador_curtiu(lp, id_post, id_utilizador)) {
        return ERRO_JA_EXISTE;
    }

    NoLike *novo = malloc(sizeof(NoLike));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->id_utilizador = id_utilizador;
    novo->seguinte = post->likes;
    post->likes = novo;
    post->num_likes++;

    return OK;
}

int publicacoes_descurtir(ListaPublicacoes *lp, int id_post, int id_utilizador) {
    NoPublicacao *post = procurar_post(lp, id_post);
    if (post == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }

    NoLike *atual = post->likes;
    NoLike *anterior = NULL;
    while (atual != NULL) {
        if (atual->id_utilizador == id_utilizador) {
            if (anterior == NULL) post->likes = atual->seguinte;
            else anterior->seguinte = atual->seguinte;
            free(atual);
            post->num_likes--;
            return OK;
        }
        anterior = atual;
        atual = atual->seguinte;
    }

    return ERRO_NAO_ENCONTRADO;
}

int publicacoes_contar_likes(const ListaPublicacoes *lp, int id_post) {
    NoPublicacao *post = procurar_post(lp, id_post);
    return (post != NULL) ? post->num_likes : 0;
}

int publicacoes_utilizador_curtiu(const ListaPublicacoes *lp, int id_post, int id_utilizador) {
    NoPublicacao *post = procurar_post(lp, id_post);
    if (post == NULL) {
        return 0;
    }
    for (NoLike *atual = post->likes; atual != NULL; atual = atual->seguinte) {
        if (atual->id_utilizador == id_utilizador) {
            return 1;
        }
    }
    return 0;
}

int publicacoes_comentar(ListaPublicacoes *lp, int id_post, int id_autor, const char *conteudo) {
    if (lp == NULL || conteudo == NULL || conteudo[0] == '\0') {
        return ERRO_PARAMETRO_INVALIDO;
    }
    NoPublicacao *post = procurar_post(lp, id_post);
    if (post == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }

    NoComentario *novo = malloc(sizeof(NoComentario));
    if (novo == NULL) {
        return ERRO_MEMORIA;
    }
    novo->id_comentario = lp->proximo_id_comentario++;
    novo->id_post = id_post;
    novo->id_autor = id_autor;
    strncpy(novo->conteudo, conteudo, MAX_CONTEUDO - 1);
    novo->conteudo[MAX_CONTEUDO - 1] = '\0';
    novo->data_hora = obter_data_hora_atual();
    novo->seguinte = NULL;

    /* inserção no fim: preserva a ordem cronológica dos comentários */
    if (post->comentarios == NULL) {
        post->comentarios = novo;
    } else {
        NoComentario *atual = post->comentarios;
        while (atual->seguinte != NULL) {
            atual = atual->seguinte;
        }
        atual->seguinte = novo;
    }
    post->num_comentarios++;

    return OK;
}

ComentarioInfo *publicacoes_listar_comentarios(const ListaPublicacoes *lp, int id_post, int *num) {
    if (num != NULL) *num = 0;
    NoPublicacao *post = procurar_post(lp, id_post);
    if (post == NULL || post->num_comentarios == 0) {
        return NULL;
    }

    ComentarioInfo *resultado = malloc((size_t) post->num_comentarios * sizeof(ComentarioInfo));
    if (resultado == NULL) {
        return NULL;
    }

    int i = 0;
    for (NoComentario *atual = post->comentarios; atual != NULL; atual = atual->seguinte) {
        resultado[i].id_comentario = atual->id_comentario;
        resultado[i].id_post = atual->id_post;
        resultado[i].id_autor = atual->id_autor;
        strncpy(resultado[i].conteudo, atual->conteudo, MAX_CONTEUDO - 1);
        resultado[i].conteudo[MAX_CONTEUDO - 1] = '\0';
        resultado[i].data_hora = atual->data_hora;
        i++;
    }
    if (num != NULL) *num = i;
    return resultado;
}

static void preencher_info(const NoPublicacao *post, PublicacaoInfo *info) {
    info->id_post = post->id_post;
    info->id_autor = post->id_autor;
    strncpy(info->conteudo, post->conteudo, MAX_CONTEUDO - 1);
    info->conteudo[MAX_CONTEUDO - 1] = '\0';
    info->data_hora = post->data_hora;
    info->num_likes = post->num_likes;
    info->num_comentarios = post->num_comentarios;
}

PublicacaoInfo *publicacoes_listar_de_utilizador(const ListaPublicacoes *lp, int id_autor, int *num) {
    if (num != NULL) *num = 0;
    if (lp == NULL) {
        return NULL;
    }

    int count = 0;
    for (NoPublicacao *atual = lp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->id_autor == id_autor) count++;
    }
    if (count == 0) {
        return NULL;
    }

    PublicacaoInfo *resultado = malloc((size_t) count * sizeof(PublicacaoInfo));
    if (resultado == NULL) {
        return NULL;
    }

    int i = 0;
    /* a lista já está ordenada da mais recente para a mais antiga */
    for (NoPublicacao *atual = lp->inicio; atual != NULL; atual = atual->seguinte) {
        if (atual->id_autor == id_autor) {
            preencher_info(atual, &resultado[i]);
            i++;
        }
    }
    if (num != NULL) *num = i;
    return resultado;
}

PublicacaoInfo *publicacoes_feed_amigos(const ListaPublicacoes *lp, const GrafoAmizades *g, int id_utilizador, int *num) {
    if (num != NULL) *num = 0;
    if (lp == NULL || g == NULL) {
        return NULL;
    }

    int num_amigos = 0;
    int *amigos = grafo_listar_amigos(g, id_utilizador, &num_amigos);
    if (num_amigos == 0) {
        free(amigos);
        return NULL;
    }

    /* conta quantos posts pertencem a algum amigo */
    int count = 0;
    for (NoPublicacao *atual = lp->inicio; atual != NULL; atual = atual->seguinte) {
        for (int i = 0; i < num_amigos; i++) {
            if (atual->id_autor == amigos[i]) { count++; break; }
        }
    }
    if (count == 0) {
        free(amigos);
        return NULL;
    }

    PublicacaoInfo *resultado = malloc((size_t) count * sizeof(PublicacaoInfo));
    if (resultado == NULL) {
        free(amigos);
        return NULL;
    }

    int j = 0;
    for (NoPublicacao *atual = lp->inicio; atual != NULL; atual = atual->seguinte) {
        for (int i = 0; i < num_amigos; i++) {
            if (atual->id_autor == amigos[i]) {
                preencher_info(atual, &resultado[j]);
                j++;
                break;
            }
        }
    }

    free(amigos);
    if (num != NULL) *num = j;
    return resultado;
}