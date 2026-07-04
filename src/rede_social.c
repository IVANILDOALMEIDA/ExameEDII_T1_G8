#include "rede_social.h"
#include "persistencia.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#define FICHEIRO_UTILIZADORES  "usuarios.dat"
#define FICHEIRO_AMIZADES      "amizades.dat"
#define FICHEIRO_PEDIDOS       "pedidos.dat"
#define FICHEIRO_MENSAGENS     "mensagens.dat"
#define FICHEIRO_PUBLICACOES   "posts.dat"

/* ------------------------------ Ciclo de vida ------------------------------- */

RedeSocial *rede_social_iniciar(void) {
    RedeSocial *rede = malloc(sizeof(RedeSocial));
    if (rede == NULL) {
        return NULL;
    }

    /* utilizadores */
    if (persistencia_ficheiro_existe(FICHEIRO_UTILIZADORES)) {
        rede->utilizadores = carregar_utilizadores(FICHEIRO_UTILIZADORES);
    }
    if (!persistencia_ficheiro_existe(FICHEIRO_UTILIZADORES) || rede->utilizadores == NULL) {
        rede->utilizadores = hash_criar(101);
    }

    /* amizades — cria o grafo e garante um vértice para cada utilizador */
    rede->amizades = grafo_criar(101);
    int total_utilizadores = 0;
    Utilizador **todos = hash_listar_todos(rede->utilizadores, &total_utilizadores);
    int maior_id = 0;
    for (int i = 0; i < total_utilizadores; i++) {
        grafo_adicionar_vertice(rede->amizades, todos[i]->id);
        if (todos[i]->id > maior_id) {
            maior_id = todos[i]->id;
        }
    }
    free(todos);

    if (persistencia_ficheiro_existe(FICHEIRO_AMIZADES)) {
        carregar_amizades(FICHEIRO_AMIZADES, rede->amizades);
    }

    /* pedidos de amizade */
    rede->pedidos = persistencia_ficheiro_existe(FICHEIRO_PEDIDOS)
                        ? carregar_pedidos(FICHEIRO_PEDIDOS)
                        : pedidos_criar();
    if (rede->pedidos == NULL) {
        rede->pedidos = pedidos_criar();
    }

    /* mensagens */
    rede->mensagens = persistencia_ficheiro_existe(FICHEIRO_MENSAGENS)
                           ? carregar_mensagens(FICHEIRO_MENSAGENS)
                           : mensagens_criar();
    if (rede->mensagens == NULL) {
        rede->mensagens = mensagens_criar();
    }

    /* publicações */
    rede->publicacoes = persistencia_ficheiro_existe(FICHEIRO_PUBLICACOES)
                             ? carregar_publicacoes(FICHEIRO_PUBLICACOES)
                             : publicacoes_criar();
    if (rede->publicacoes == NULL) {
        rede->publicacoes = publicacoes_criar();
    }

    rede->sessao_atual = -1;
    rede->proximo_id = maior_id + 1;

    return rede;
}

void rede_social_terminar(RedeSocial *rede) {
    if (rede == NULL) {
        return;
    }

    guardar_utilizadores(FICHEIRO_UTILIZADORES, rede->utilizadores);
    guardar_amizades(FICHEIRO_AMIZADES, rede->amizades);
    guardar_pedidos(FICHEIRO_PEDIDOS, rede->pedidos);
    guardar_mensagens(FICHEIRO_MENSAGENS, rede->mensagens);
    guardar_publicacoes(FICHEIRO_PUBLICACOES, rede->publicacoes);

    hash_destruir(rede->utilizadores, 1); /* liberta também os Utilizador* */
    grafo_destruir(rede->amizades);
    pedidos_destruir(rede->pedidos);
    mensagens_destruir(rede->mensagens);
    publicacoes_destruir(rede->publicacoes);

    free(rede);
}



int rede_social_criar_conta(RedeSocial *rede,
                             const char *nome_completo,
                             const char *username,
                             const char *password,
                             const char *universidade,
                             const char *curso,
                             int ano_academico,
                             int idade) {
    if (rede == NULL || nome_completo == NULL || username == NULL || password == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (hash_existe_username(rede->utilizadores, username)) {
        return ERRO_JA_EXISTE;
    }

    int novo_id = rede->proximo_id;
    Utilizador *u = utilizador_criar(novo_id, nome_completo, username, password,
                                      universidade, curso, ano_academico, idade);
    if (u == NULL) {
        return ERRO_MEMORIA;
    }

    if (hash_inserir(rede->utilizadores, u) != OK) {
        utilizador_destruir(u);
        return ERRO_MEMORIA;
    }
    grafo_adicionar_vertice(rede->amizades, novo_id);

    rede->proximo_id++;
    return novo_id;
}

int rede_social_iniciar_sessao(RedeSocial *rede, const char *username, const char *password) {
    if (rede == NULL || username == NULL || password == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }

    Utilizador *u = hash_procurar_username(rede->utilizadores, username);
    if (u == NULL || !utilizador_autenticar(u, password)) {
        return ERRO_NAO_AUTORIZADO;
    }

    utilizador_set_estado(u, ESTADO_ONLINE);
    rede->sessao_atual = u->id;
    return rede->sessao_atual;
}

void rede_social_terminar_sessao(RedeSocial *rede) {
    if (rede == NULL || rede->sessao_atual < 0) {
        return;
    }
    Utilizador *u = hash_procurar_id(rede->utilizadores, rede->sessao_atual);
    if (u != NULL) {
        utilizador_set_estado(u, ESTADO_OFFLINE);
    }
    rede->sessao_atual = -1;
}

int rede_social_sessao_atual(const RedeSocial *rede) {
    return (rede != NULL) ? rede->sessao_atual : -1;
}

int rede_social_eliminar_conta(RedeSocial *rede, int id_utilizador) {
    if (rede == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    Utilizador *u = hash_procurar_id(rede->utilizadores, id_utilizador);
    if (u == NULL) {
        return ERRO_NAO_ENCONTRADO;
    }

    char username_copia[MAX_USERNAME];
    strncpy(username_copia, u->username, MAX_USERNAME - 1);
    username_copia[MAX_USERNAME - 1] = '\0';

    hash_remover(rede->utilizadores, username_copia);
    utilizador_destruir(u);
    grafo_remover_vertice(rede->amizades, id_utilizador);

    if (rede->sessao_atual == id_utilizador) {
        rede->sessao_atual = -1;
    }

    return OK;
}


const TabelaHash *rede_social_get_utilizadores(const RedeSocial *rede) {
    return (rede != NULL) ? rede->utilizadores : NULL;
}

const GrafoAmizades *rede_social_get_amizades(const RedeSocial *rede) {
    return (rede != NULL) ? rede->amizades : NULL;
}

GestorPedidos *rede_social_get_pedidos(RedeSocial *rede) {
    return (rede != NULL) ? rede->pedidos : NULL;
}

ListaMensagens *rede_social_get_mensagens(RedeSocial *rede) {
    return (rede != NULL) ? rede->mensagens : NULL;
}

ListaPublicacoes *rede_social_get_publicacoes(RedeSocial *rede) {
    return (rede != NULL) ? rede->publicacoes : NULL;
}