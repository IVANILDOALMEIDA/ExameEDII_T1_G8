/**
 * persistencia.c
 * -----------------------------------------------------------------------
 * Implementação em TEXTO SIMPLES (.txt) do módulo de persistência.
 *
 * Cada ficheiro tem uma linha de cabeçalho com o(s) contador(es), seguida
 * de uma linha por registo. Os campos de cada registo são separados por
 * '|'. Isto substitui a versão binária original (fwrite/fread de
 * structs) por fprintf/fgets, o que tem duas vantagens práticas para um
 * projeto académico: dá para abrir os ficheiros num editor de texto
 * normal, e não depende do layout de memória da struct (que muda de
 * compilador para compilador, de sistema para sistema).
 *
 * LIMITAÇÃO ACEITE (mantida propositadamente simples):
 *   Os campos de texto livre (bio, conteúdo de mensagens/posts/
 *   comentários) não podem conter o caractere '|' nem quebras de linha -
 *   se aparecerem, são substituídos por ';' e espaço, respetivamente, no
 *   momento de gravar (o texto em memória, durante a sessão em curso,
 *   não é alterado). Não há escaping - isso complicaria o parser para
 *   um ganho que este projeto não precisa.
 *
 * Formatos de ficheiro:
 *
 *   usuarios.txt
 *     total
 *     id|nome|username|password|universidade|curso|ano|idade|cidade|email|bio|estado|interesse1;interesse2;...
 *
 *   amizades.txt
 *     total_arestas
 *     id1|id2                          (uma aresta por linha)
 *
 *   pedidos.txt
 *     total
 *     remetente|destinatario|estado|dia|mes|ano|hora|minuto
 *
 *   mensagens.txt
 *     total
 *     remetente|destinatario|conteudo|dia|mes|ano|hora|minuto
 *
 *   posts.txt
 *     total_posts|proximo_id_post|proximo_id_comentario
 *     P|id_post|id_autor|conteudo|dia|mes|ano|hora|minuto|num_likes|num_comentarios
 *     L|id_utilizador                  (repetido num_likes vezes)
 *     C|id_comentario|id_post|id_autor|conteudo|dia|mes|ano|hora|minuto
 *                                       (repetido num_comentarios vezes)
 * -----------------------------------------------------------------------
 */

#include "persistencia.h"
#include "TAD.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * Auxiliares de texto
 * ============================================================ */

/* Substitui '|' (delimitador de campo) e quebras de linha por
 * caracteres seguros, copiando para 'destino' (tamanho 'tamanho').
 * Não faz overflow. */
static void sanitizar_campo(const char *origem, char *destino, size_t tamanho) {
    size_t i = 0;
    if (origem == NULL || tamanho == 0) {
        if (tamanho > 0) destino[0] = '\0';
        return;
    }
    for (; origem[i] != '\0' && i < tamanho - 1; i++) {
        char c = origem[i];
        if (c == '|') c = ';';
        else if (c == '\n' || c == '\r') c = ' ';
        destino[i] = c;
    }
    destino[i] = '\0';
}

/* Extrai o próximo campo delimitado por '|' a partir de *cursor,
 * avançando *cursor para depois do delimitador. Ao contrário de
 * strtok(), NÃO ignora campos vazios ("a||b" dá "a", "", "b") - isso
 * é essencial para não desalinhar colunas quando um campo opcional
 * (ex.: cidade, email) está vazio. Modifica o buffer original,
 * inserindo '\0' onde estava o '|'. */
static char *proximo_campo(char **cursor) {
    static char vazio[] = "";
    if (*cursor == NULL) {
        return vazio;
    }
    char *inicio = *cursor;
    char *sep = strchr(inicio, '|');
    if (sep != NULL) {
        *sep = '\0';
        *cursor = sep + 1;
    } else {
        *cursor = NULL;
    }
    return inicio;
}

/* Remove o '\n' (e '\r', para o caso de ficheiros com CRLF) do fim de
 * uma linha lida com fgets(). */
static void aparar_linha(char *linha) {
    size_t len = strlen(linha);
    while (len > 0 && (linha[len - 1] == '\n' || linha[len - 1] == '\r')) {
        linha[--len] = '\0';
    }
}

int persistencia_ficheiro_existe(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return 0;
    }
    FILE *f = fopen(caminho_ficheiro, "r");
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
    FILE *f = fopen(caminho_ficheiro, "w");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    int total = 0;
    Utilizador **lista = hash_listar_todos(utilizadores, &total);
    fprintf(f, "%d\n", total);

    for (int i = 0; i < total; i++) {
        Utilizador *u = lista[i];

        char nome[MAX_NOME], username[MAX_USERNAME], password[MAX_PASSWORD];
        char universidade[MAX_UNIVERSIDADE], curso[MAX_CURSO];
        char cidade[MAX_CIDADE], email[MAX_EMAIL], bio[MAX_BIO];
        sanitizar_campo(u->nome_completo, nome, sizeof(nome));
        sanitizar_campo(u->username, username, sizeof(username));
        sanitizar_campo(u->password, password, sizeof(password));
        sanitizar_campo(u->universidade, universidade, sizeof(universidade));
        sanitizar_campo(u->curso, curso, sizeof(curso));
        sanitizar_campo(u->cidade, cidade, sizeof(cidade));
        sanitizar_campo(u->email, email, sizeof(email));
        sanitizar_campo(u->bio, bio, sizeof(bio));

        fprintf(f, "%d|%s|%s|%s|%s|%s|%d|%d|%s|%s|%s|%d|",
                u->id, nome, username, password, universidade, curso,
                u->ano_academico, u->idade, cidade, email, bio, (int) u->estado);

        int num_interesses = lista_interesses_tamanho(u->interesses);
        if (num_interesses > 0) {
            char (*buffer)[MAX_INTERESSE] = malloc((size_t) num_interesses * sizeof(*buffer));
            if (buffer != NULL) {
                int n = lista_interesses_para_array(u->interesses, buffer, num_interesses);
                for (int k = 0; k < n; k++) {
                    char campo[MAX_INTERESSE];
                    sanitizar_campo(buffer[k], campo, sizeof(campo));
                    /* ';' é o separador entre interesses nesta mesma coluna */
                    for (char *p = campo; *p != '\0'; p++) {
                        if (*p == ';') *p = ',';
                    }
                    fprintf(f, "%s%s", campo, (k < n - 1) ? ";" : "");
                }
                free(buffer);
            }
        }
        fprintf(f, "\n");
    }

    free(lista);
    fclose(f);
    return OK;
}

TabelaHash *carregar_utilizadores(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "r");
    if (f == NULL) {
        return NULL;
    }

    char linha[4096];
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return NULL;
    }
    int total = atoi(linha);

    int capacidade = (total * 2 > 16) ? total * 2 : 16;
    TabelaHash *tabela = hash_criar(capacidade);
    if (tabela == NULL) {
        fclose(f);
        return NULL;
    }

    for (int i = 0; i < total; i++) {
        if (fgets(linha, sizeof(linha), f) == NULL) {
            break;
        }
        aparar_linha(linha);

        char *cursor = linha;
        int id             = atoi(proximo_campo(&cursor));
        char *nome          = proximo_campo(&cursor);
        char *username      = proximo_campo(&cursor);
        char *password      = proximo_campo(&cursor);
        char *universidade  = proximo_campo(&cursor);
        char *curso         = proximo_campo(&cursor);
        int ano_academico  = atoi(proximo_campo(&cursor));
        int idade          = atoi(proximo_campo(&cursor));
        char *cidade        = proximo_campo(&cursor);
        char *email         = proximo_campo(&cursor);
        char *bio           = proximo_campo(&cursor);
        (void) proximo_campo(&cursor); /* estado gravado - ignorado (ver utilizador_set_estado abaixo) */
        char *interesses_txt = (cursor != NULL) ? cursor : "";

        Utilizador *u = utilizador_criar(id, nome, username, password,
                                          universidade, curso, ano_academico, idade);
        if (u == NULL) {
            continue;
        }
        utilizador_set_cidade(u, cidade);
        utilizador_set_email(u, email);
        utilizador_set_bio(u, bio);
        utilizador_set_estado(u, ESTADO_OFFLINE); /* ninguém fica "online" entre execuções */

        char *tok = interesses_txt;
        while (tok != NULL && *tok != '\0') {
            char *fim_tok = strchr(tok, ';');
            if (fim_tok != NULL) {
                *fim_tok = '\0';
            }
            if (tok[0] != '\0') {
                utilizador_adicionar_interesse(u, tok);
            }
            tok = (fim_tok != NULL) ? fim_tok + 1 : NULL;
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
    FILE *f = fopen(caminho_ficheiro, "w");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    int num_arestas = grafo_contar_amizades(g);
    fprintf(f, "%d\n", num_arestas);

    /* percorre todos os vértices; escreve cada aresta uma única vez
     * (apenas quando id_amigo > id do vértice atual, já que a lista
     * de adjacência é simétrica) */
    for (int i = 0; i < g->capacidade; i++) {
        if (!g->vertices[i].usado) continue;
        for (NoAdjacencia *atual = g->vertices[i].amigos; atual != NULL; atual = atual->seguinte) {
            if (atual->id_amigo > i) {
                fprintf(f, "%d|%d\n", i, atual->id_amigo);
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
    FILE *f = fopen(caminho_ficheiro, "r");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    char linha[64];
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return ERRO_FICHEIRO;
    }
    int num_arestas = atoi(linha);

    for (int i = 0; i < num_arestas; i++) {
        if (fgets(linha, sizeof(linha), f) == NULL) {
            break;
        }
        int a, b;
        if (sscanf(linha, "%d|%d", &a, &b) != 2) {
            break;
        }
        grafo_adicionar_vertice(g, a);
        grafo_adicionar_vertice(g, b);
        grafo_inserir_aresta(g, a, b);
    }

    fclose(f);
    return OK;
}

/* ------------------------------ Pedidos de amizade -------------------------- */

int guardar_pedidos(const char *caminho_ficheiro, const GestorPedidos *gp) {
    if (caminho_ficheiro == NULL || gp == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    FILE *f = fopen(caminho_ficheiro, "w");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    fprintf(f, "%d\n", gp->num_pedidos);
    for (NoPedido *atual = gp->inicio; atual != NULL; atual = atual->seguinte) {
        fprintf(f, "%d|%d|%d|%d|%d|%d|%d|%d\n",
                atual->id_remetente, atual->id_destinatario, (int) atual->estado,
                atual->data_envio.dia, atual->data_envio.mes, atual->data_envio.ano,
                atual->data_envio.hora, atual->data_envio.minuto);
    }

    fclose(f);
    return OK;
}

GestorPedidos *carregar_pedidos(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "r");
    if (f == NULL) {
        return NULL;
    }

    GestorPedidos *gp = pedidos_criar();
    if (gp == NULL) {
        fclose(f);
        return NULL;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return gp;
    }
    int total = atoi(linha);

    for (int i = 0; i < total; i++) {
        if (fgets(linha, sizeof(linha), f) == NULL) {
            break;
        }
        int remetente, destinatario, estado, dia, mes, ano, hora, minuto;
        if (sscanf(linha, "%d|%d|%d|%d|%d|%d|%d|%d",
                   &remetente, &destinatario, &estado,
                   &dia, &mes, &ano, &hora, &minuto) != 8) {
            break;
        }

        /* reconstrução direta do nó interno, para preservar o estado
         * exato gravado (pedidos_enviar() força sempre PEDIDO_PENDENTE,
         * o que apagaria histórico de aceites/recusados/cancelados) */
        NoPedido *novo = malloc(sizeof(NoPedido));
        if (novo == NULL) continue;
        novo->id_remetente = remetente;
        novo->id_destinatario = destinatario;
        novo->estado = (EstadoPedido) estado;
        novo->data_envio.dia = dia;
        novo->data_envio.mes = mes;
        novo->data_envio.ano = ano;
        novo->data_envio.hora = hora;
        novo->data_envio.minuto = minuto;
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
    FILE *f = fopen(caminho_ficheiro, "w");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    fprintf(f, "%d\n", lm->num_mensagens);
    for (NoMensagem *atual = lm->inicio; atual != NULL; atual = atual->seguinte) {
        char conteudo[MAX_CONTEUDO];
        sanitizar_campo(atual->conteudo, conteudo, sizeof(conteudo));
        fprintf(f, "%d|%d|%s|%d|%d|%d|%d|%d\n",
                atual->id_remetente, atual->id_destinatario, conteudo,
                atual->data_hora.dia, atual->data_hora.mes, atual->data_hora.ano,
                atual->data_hora.hora, atual->data_hora.minuto);
    }

    fclose(f);
    return OK;
}

ListaMensagens *carregar_mensagens(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "r");
    if (f == NULL) {
        return NULL;
    }

    ListaMensagens *lm = mensagens_criar();
    if (lm == NULL) {
        fclose(f);
        return NULL;
    }

    char linha[1024];
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return lm;
    }
    int total = atoi(linha);

    for (int i = 0; i < total; i++) {
        if (fgets(linha, sizeof(linha), f) == NULL) {
            break;
        }
        aparar_linha(linha);

        char *cursor = linha;
        int remetente     = atoi(proximo_campo(&cursor));
        int destinatario  = atoi(proximo_campo(&cursor));
        char *conteudo     = proximo_campo(&cursor);
        int dia    = atoi(proximo_campo(&cursor));
        int mes    = atoi(proximo_campo(&cursor));
        int ano    = atoi(proximo_campo(&cursor));
        int hora   = atoi(proximo_campo(&cursor));
        int minuto = atoi(proximo_campo(&cursor));

        NoMensagem *novo = malloc(sizeof(NoMensagem));
        if (novo == NULL) continue;
        novo->id_remetente = remetente;
        novo->id_destinatario = destinatario;
        strncpy(novo->conteudo, conteudo, MAX_CONTEUDO - 1);
        novo->conteudo[MAX_CONTEUDO - 1] = '\0';
        novo->data_hora.dia = dia;
        novo->data_hora.mes = mes;
        novo->data_hora.ano = ano;
        novo->data_hora.hora = hora;
        novo->data_hora.minuto = minuto;
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
    FILE *f = fopen(caminho_ficheiro, "w");
    if (f == NULL) {
        return ERRO_FICHEIRO;
    }

    fprintf(f, "%d|%d|%d\n", lp->num_publicacoes, lp->proximo_id_post, lp->proximo_id_comentario);

    /* posts gravados na ordem da lista (cabeça = mais recente) */
    for (NoPublicacao *post = lp->inicio; post != NULL; post = post->seguinte) {
        char conteudo[MAX_CONTEUDO];
        sanitizar_campo(post->conteudo, conteudo, sizeof(conteudo));
        fprintf(f, "P|%d|%d|%s|%d|%d|%d|%d|%d|%d|%d\n",
                post->id_post, post->id_autor, conteudo,
                post->data_hora.dia, post->data_hora.mes, post->data_hora.ano,
                post->data_hora.hora, post->data_hora.minuto,
                post->num_likes, post->num_comentarios);

        for (NoLike *l = post->likes; l != NULL; l = l->seguinte) {
            fprintf(f, "L|%d\n", l->id_utilizador);
        }

        for (NoComentario *c = post->comentarios; c != NULL; c = c->seguinte) {
            char coment[MAX_CONTEUDO];
            sanitizar_campo(c->conteudo, coment, sizeof(coment));
            fprintf(f, "C|%d|%d|%d|%s|%d|%d|%d|%d|%d\n",
                    c->id_comentario, c->id_post, c->id_autor, coment,
                    c->data_hora.dia, c->data_hora.mes, c->data_hora.ano,
                    c->data_hora.hora, c->data_hora.minuto);
        }
    }

    fclose(f);
    return OK;
}

ListaPublicacoes *carregar_publicacoes(const char *caminho_ficheiro) {
    if (caminho_ficheiro == NULL) {
        return NULL;
    }
    FILE *f = fopen(caminho_ficheiro, "r");
    if (f == NULL) {
        return NULL;
    }

    ListaPublicacoes *lp = publicacoes_criar();
    if (lp == NULL) {
        fclose(f);
        return NULL;
    }

    char linha[1024];
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return lp;
    }
    int total_posts = 0, proximo_post = 1, proximo_comentario = 1;
    sscanf(linha, "%d|%d|%d", &total_posts, &proximo_post, &proximo_comentario);

    NoPublicacao *ultimo = NULL; /* para inserir no fim e preservar a ordem do ficheiro */

    for (int i = 0; i < total_posts; i++) {
        if (fgets(linha, sizeof(linha), f) == NULL) {
            break;
        }
        aparar_linha(linha);

        if (linha[0] != 'P' || linha[1] != '|') {
            break; /* ficheiro corrompido ou incompatível: para aqui */
        }

        char *cursor = linha + 2; /* salta o marcador "P|" */
        NoPublicacao *post = malloc(sizeof(NoPublicacao));
        if (post == NULL) {
            break;
        }

        post->id_post  = atoi(proximo_campo(&cursor));
        post->id_autor = atoi(proximo_campo(&cursor));
        char *conteudo  = proximo_campo(&cursor);
        strncpy(post->conteudo, conteudo, MAX_CONTEUDO - 1);
        post->conteudo[MAX_CONTEUDO - 1] = '\0';
        post->data_hora.dia    = atoi(proximo_campo(&cursor));
        post->data_hora.mes    = atoi(proximo_campo(&cursor));
        post->data_hora.ano    = atoi(proximo_campo(&cursor));
        post->data_hora.hora   = atoi(proximo_campo(&cursor));
        post->data_hora.minuto = atoi(proximo_campo(&cursor));
        int num_likes          = atoi(proximo_campo(&cursor));
        int num_comentarios    = atoi(proximo_campo(&cursor));

        post->likes = NULL;
        post->num_likes = 0;
        post->comentarios = NULL;
        post->num_comentarios = 0;
        post->seguinte = NULL;

        for (int k = 0; k < num_likes; k++) {
            if (fgets(linha, sizeof(linha), f) == NULL) break;
            aparar_linha(linha);
            if (linha[0] != 'L' || linha[1] != '|') continue;

            NoLike *l = malloc(sizeof(NoLike));
            if (l == NULL) continue;
            l->id_utilizador = atoi(linha + 2);
            l->seguinte = post->likes;
            post->likes = l;
            post->num_likes++;
        }

        NoComentario *ultimo_comentario = NULL;
        for (int k = 0; k < num_comentarios; k++) {
            if (fgets(linha, sizeof(linha), f) == NULL) break;
            aparar_linha(linha);
            if (linha[0] != 'C' || linha[1] != '|') continue;

            char *ccursor = linha + 2;
            NoComentario *c = malloc(sizeof(NoComentario));
            if (c == NULL) continue;
            c->id_comentario = atoi(proximo_campo(&ccursor));
            c->id_post       = atoi(proximo_campo(&ccursor));
            c->id_autor      = atoi(proximo_campo(&ccursor));
            char *cconteudo   = proximo_campo(&ccursor);
            strncpy(c->conteudo, cconteudo, MAX_CONTEUDO - 1);
            c->conteudo[MAX_CONTEUDO - 1] = '\0';
            c->data_hora.dia    = atoi(proximo_campo(&ccursor));
            c->data_hora.mes    = atoi(proximo_campo(&ccursor));
            c->data_hora.ano    = atoi(proximo_campo(&ccursor));
            c->data_hora.hora   = atoi(proximo_campo(&ccursor));
            c->data_hora.minuto = atoi(proximo_campo(&ccursor));
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
