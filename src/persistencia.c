#include "persistencia.h"
#include "TAD.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 

int persistencia_ficheiro_existe(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return 0;
    }
    FILE *f = fopen(caminho_ficheiro, "rb");
    if (f == NULL) {
        return 0;
    }
    fclose(f);
    return 1;
}

/* ------------------------------ Utilizadores -------------------------------- */

int guardar_utilizadores(const char *caminho_ficheiro, const TabelaHash *utilizadores) {
    if (caminho_ficheiro == NULL || utilizadores == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "wb");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    int total = 0;
    Utilizador **lista = hash_listar_todos(utilizadores, &total);
    fwrite(&total, sizeof(int), 1, f);

    for (int i = 0; i < total; i++) {
        Utilizador *u = lista[i];
        RegistoUtilizador reg;
        memset(&reg, 0, sizeof(reg));

        reg.id = u->id;
        strncpy(reg.nome_completo, u->nome_completo, MAX_NOME - 1);
        strncpy(reg.username, u->username, MAX_USERNAME - 1);
        strncpy(reg.password, u->password, MAX_PASSWORD - 1);
        strncpy(reg.universidade, u->universidade, MAX_UNIVERSIDADE - 1);
        strncpy(reg.curso, u->curso, MAX_CURSO - 1);
        reg.ano_academico = u->ano_academico;
        reg.idade = u->idade;
        strncpy(reg.cidade, u->cidade, MAX_CIDADE - 1);
        strncpy(reg.email, u->email, MAX_EMAIL - 1);
        strncpy(reg.bio, u->bio, MAX_BIO - 1);
        reg.estado = u->estado;
        reg.num_interesses = lista_interesses_tamanho(u->interesses);

        fwrite(&reg, sizeof(RegistoUtilizador), 1, f);

        if (reg.num_interesses > 0) {
            char (*buffer)[MAX_INTERESSE] = malloc((size_t) reg.num_interesses * sizeof(*buffer));
            if (buffer != NULL) {
                int n = lista_interesses_para_array(u->interesses, buffer, reg.num_interesses);
                for (int k = 0; k < n; k++) {
                    fwrite(buffer[k], sizeof(char), MAX_INTERESSE, f);
                }
                free(buffer);
            }
        }
    }

    free(lista);
    fclose(f);
    return OK;
}

TabelaHash *carregar_utilizadores(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "rb");
    if (f == NULL) {
        return NULL;
    }

    int total = 0;
    if (fread(&total, sizeof(int), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    int capacidade = (total * 2 > 16) ? total * 2 : 16;
    TabelaHash *tabela = hash_criar(capacidade);
    if (tabela == NULL) {
        fclose(f);
        return NULL;
    }

    for (int i = 0; i < total; i++) {
        RegistoUtilizador reg;
        if (fread(&reg, sizeof(RegistoUtilizador), 1, f) != 1) {
            break;
        }

        Utilizador *u = utilizador_criar(reg.id, reg.nome_completo, reg.username, reg.password,
                                          reg.universidade, reg.curso, reg.ano_academico, reg.idade);
        if (u == NULL) {
            continue;
        }
        utilizador_set_cidade(u, reg.cidade);
        utilizador_set_email(u, reg.email);
        utilizador_set_bio(u, reg.bio);
        utilizador_set_estado(u, ESTADO_OFFLINE); /* ninguém fica "online" entre execuções */

        for (int k = 0; k < reg.num_interesses; k++) {
            char interesse[MAX_INTERESSE];
            if (fread(interesse, sizeof(char), MAX_INTERESSE, f) == MAX_INTERESSE) {
                utilizador_adicionar_interesse(u, interesse);
            }
        }

        hash_inserir(tabela, u);
    }

    fclose(f);
    return tabela;
}

/* ------------------------------ Amizades ----------------------------------- */

int guardar_amizades(const char *caminho_ficheiro, const GrafoAmizades *g) {
    if (caminho_ficheiro == NULL || g == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "wb");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    int num_arestas = grafo_contar_amizades(g);
    fwrite(&num_arestas, sizeof(int), 1, f);

    /* percorre todos os vértices; escreve cada aresta uma única vez
     * (apenas quando id_amigo > id do vértice atual, para evitar
     * duplicar, já que a lista de adjacência é simétrica) */
    for (int i = 0; i < g->capacidade; i++) {
        if (!g->vertices[i].usado) continue;
        for (NoAdjacencia *atual = g->vertices[i].amigos; atual != NULL; atual = atual->seguinte) {
            if (atual->id_amigo > i) {
                int par[2] = { i, atual->id_amigo };
                fwrite(par, sizeof(int), 2, f);
            }
        }
    }

    fclose(f);
    return OK;
}

int carregar_amizades(const char *caminho_ficheiro, GrafoAmizades *g) {
    if (caminho_ficheiro == NULL || g == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "rb");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    int num_arestas = 0;
    if (fread(&num_arestas, sizeof(int), 1, f) != 1) {
        fclose(f);
        return ERRO_FICHEIRO;
    }

    for (int i = 0; i < num_arestas; i++) {
        int par[2];
        if (fread(par, sizeof(int), 2, f) != 2) {
            break;
        }
        grafo_adicionar_vertice(g, par[0]);
        grafo_adicionar_vertice(g, par[1]);
        grafo_inserir_aresta(g, par[0], par[1]);
    }

    fclose(f);
    return OK;
}

/* ------------------------------ Pedidos de amizade -------------------------- */

int guardar_pedidos(const char *caminho_ficheiro, const GestorPedidos *gp) {
    if (caminho_ficheiro == NULL || gp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "wb");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    fwrite(&gp->num_pedidos, sizeof(int), 1, f);
    /* PedidoAmizade é um "record" simples sem apontadores, seguro para fwrite */
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        PedidoAmizade p;
        p.id_remetente = atual->id_remetente;
        p.id_destinatario = atual->id_destinatario;
        p.estado = atual->estado;
        p.data_envio = atual->data_envio;
        fwrite(&p, sizeof(PedidoAmizade), 1, f);
    }

    fclose(f);
    return OK;
}

GestorPedidos *carregar_pedidos(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "rb");
    if (f == NULL) {
        return NULL;
    }

    GestorPedidos *gp = pedidos_criar();
    if (gp == NULL) {
        fclose(f);
        return NULL;
    }

    int total = 0;
    if (fread(&total, sizeof(int), 1, f) != 1) {
        fclose(f);
        return gp;
    }

    for (int i = 0; i < total; i++) {
        PedidoAmizade p;
        if (fread(&p, sizeof(PedidoAmizade), 1, f) != 1) {
            break;
        }
        /* reconstrução direta do nó interno, para preservar o estado
         * exato gravado (pedidos_enviar() força sempre PEDIDO_PENDENTE,
         * o que apagaria histórico de aceites/recusados/cancelados) */
        NoPedido *novo = malloc(sizeof(NoPedido));
        if (novo == NULL) continue;
        novo->id_remetente = p.id_remetente;
        novo->id_destinatario = p.id_destinatario;
        novo->estado = p.estado;
        novo->data_envio = p.data_envio;
        novo->seguinte = NULL;

        if (gp->fim == NULL) {
            gp->inicio = novo;
            gp->fim = novo;
        } else {
            gp->fim->seguinte = novo;
            gp->fim = novo;
        }
        gp->num_pedidos++;
    }

    fclose(f);
    return gp;
}

/* ------------------------------ Mensagens ----------------------------------- */

int guardar_mensagens(const char *caminho_ficheiro, const ListaMensagens *lm) {
    if (caminho_ficheiro == NULL || lm == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "wb");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    fwrite(&lm->num_mensagens, sizeof(int), 1, f);
    for (NoMensagem *atual = lm->inicio; atual != NULL; atual = atual->seguinte) {
        MensagemInfo info;
        info.id_remetente = atual->id_remetente;
        info.id_destinatario = atual->id_destinatario;
        strncpy(info.conteudo, atual->conteudo, MAX_CONTEUDO - 1);
        info.conteudo[MAX_CONTEUDO - 1] = '\0';
        info.data_hora = atual->data_hora;
        fwrite(&info, sizeof(MensagemInfo), 1, f);
    }

    fclose(f);
    return OK;
}

ListaMensagens *carregar_mensagens(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "rb");
    if (f == NULL) {
        return NULL;
    }

    ListaMensagens *lm = mensagens_criar();
    if (lm == NULL) {
        fclose(f);
        return NULL;
    }

    int total = 0;
    if (fread(&total, sizeof(int), 1, f) != 1) {
        fclose(f);
        return lm;
    }

    for (int i = 0; i < total; i++) {
        MensagemInfo info;
        if (fread(&info, sizeof(MensagemInfo), 1, f) != 1) {
            break;
        }
        NoMensagem *novo = malloc(sizeof(NoMensagem));
        if (novo == NULL) continue;
        novo->id_remetente = info.id_remetente;
        novo->id_destinatario = info.id_destinatario;
        strncpy(novo->conteudo, info.conteudo, MAX_CONTEUDO - 1);
        novo->conteudo[MAX_CONTEUDO - 1] = '\0';
        novo->data_hora = info.data_hora;
        novo->seguinte = NULL;

        if (lm->fim == NULL) {
            lm->inicio = novo;
            lm->fim = novo;
        } else {
            lm->fim->seguinte = novo;
            lm->fim = novo;
        }
        lm->num_mensagens++;
    }

    fclose(f);
    return lm;
}

/* ------------------------------ Publicações / comentários -------------------- */

int guardar_publicacoes(const char *caminho_ficheiro, const ListaPublicacoes *lp) {
    if (caminho_ficheiro == NULL || lp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "wb");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    /* cabeçalho: contadores necessários para reconstruir o estado exato */
    fwrite(&lp->num_publicacoes, sizeof(int), 1, f);
    fwrite(&lp->proximo_id_post, sizeof(int), 1, f);
    fwrite(&lp->proximo_id_comentario, sizeof(int), 1, f);

    /* posts gravados na ordem da lista (cabeça = mais recente) */
    for (NoPublicacao *post = lp->inicio; post != NULL; post = post->seguinte) {
        fwrite(&post->id_post, sizeof(int), 1, f);
        fwrite(&post->id_autor, sizeof(int), 1, f);
        char conteudo[MAX_CONTEUDO];
        strncpy(conteudo, post->conteudo, MAX_CONTEUDO);
        fwrite(conteudo, sizeof(char), MAX_CONTEUDO, f);
        fwrite(&post->data_hora, sizeof(DataHora), 1, f);

        fwrite(&post->num_likes, sizeof(int), 1, f);
        for (NoLike *l = post->likes; l != NULL; l = l->seguinte) {
            fwrite(&l->id_utilizador, sizeof(int), 1, f);
        }

        fwrite(&post->num_comentarios, sizeof(int), 1, f);
        for (NoComentario *c = post->comentarios; c != NULL; c = c->seguinte) {
            ComentarioInfo info;
            info.id_comentario = c->id_comentario;
            info.id_post = c->id_post;
            info.id_autor = c->id_autor;
            strncpy(info.conteudo, c->conteudo, MAX_CONTEUDO - 1);
            info.conteudo[MAX_CONTEUDO - 1] = '\0';
            info.data_hora = c->data_hora;
            fwrite(&info, sizeof(ComentarioInfo), 1, f);
        }
    }

    fclose(f);
    return OK;
}

ListaPublicacoes *carregar_publicacoes(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "rb");
    if (f == NULL) {
        return NULL;
    }

    ListaPublicacoes *lp = publicacoes_criar();
    if (lp == NULL) {
        fclose(f);
        return NULL;
    }

    int total_posts = 0, proximo_post = 1, proximo_comentario = 1;
    if (fread(&total_posts, sizeof(int), 1, f) != 1 ||
        fread(&proximo_post, sizeof(int), 1, f) != 1 ||
        fread(&proximo_comentario, sizeof(int), 1, f) != 1) {
        fclose(f);
        return lp;
    }

    NoPublicacao *ultimo = NULL; /* para inserir no fim e preservar a ordem do ficheiro */

    for (int i = 0; i < total_posts; i++) {
        NoPublicacao *post = malloc(sizeof(NoPublicacao));
        if (post == NULL) break;

        if (fread(&post->id_post, sizeof(int), 1, f) != 1 ||
            fread(&post->id_autor, sizeof(int), 1, f) != 1 ||
            fread(post->conteudo, sizeof(char), MAX_CONTEUDO, f) != MAX_CONTEUDO ||
            fread(&post->data_hora, sizeof(DataHora), 1, f) != 1) {
            free(post);
            break;
        }
        post->likes = NULL;
        post->num_likes = 0;
        post->comentarios = NULL;
        post->num_comentarios = 0;
        post->seguinte = NULL;

        int num_likes = 0;
        fread(&num_likes, sizeof(int), 1, f);
        for (int k = 0; k < num_likes; k++) {
            int id_utilizador;
            if (fread(&id_utilizador, sizeof(int), 1, f) != 1) break;
            NoLike *l = malloc(sizeof(NoLike));
            if (l == NULL) continue;
            l->id_utilizador = id_utilizador;
            l->seguinte = post->likes;
            post->likes = l;
            post->num_likes++;
        }

        int num_comentarios = 0;
        fread(&num_comentarios, sizeof(int), 1, f);
        NoComentario *ultimo_comentario = NULL;
        for (int k = 0; k < num_comentarios; k++) {
            ComentarioInfo info;
            if (fread(&info, sizeof(ComentarioInfo), 1, f) != 1) break;
            NoComentario *c = malloc(sizeof(NoComentario));
            if (c == NULL) continue;
            c->id_comentario = info.id_comentario;
            c->id_post = info.id_post;
            c->id_autor = info.id_autor;
            strncpy(c->conteudo, info.conteudo, MAX_CONTEUDO - 1);
            c->conteudo[MAX_CONTEUDO - 1] = '\0';
            c->data_hora = info.data_hora;
            c->seguinte = NULL;

            if (ultimo_comentario == NULL) {
                post->comentarios = c;
            } else {
                ultimo_comentario->seguinte = c;
            }
            ultimo_comentario = c;
            post->num_comentarios++;
        }

        /* inserção no fim, para preservar a mesma ordem (cabeça=mais
         * recente) que existia no ficheiro */
        if (ultimo == NULL) {
            lp->inicio = post;
        } else {
            ultimo->seguinte = post;
        }
        ultimo = post;
        lp->num_publicacoes++;
    }

    lp->proximo_id_post = proximo_post;
    lp->proximo_id_comentario = proximo_comentario;

    fclose(f);
    return lp;
}