/**
 * TADs.h
 * -----------------------------------------------------------------------
 *                            UNICONNECT
 * -----------------------------------------------------------------------
 */

#ifndef TADS_H
#define TADS_H

#include "utils.h"

/* ============================================================
 * ListaInteresses — lista ligada simples
 * ============================================================ */
typedef struct NoInteresse {
    char interesse[MAX_INTERESSE];
    struct NoInteresse *seguinte;
} NoInteresse;

typedef struct ListaInteresses {
    NoInteresse *inicio;
    int tamanho;
} ListaInteresses;

/* ============================================================
 * Utilizador
 * ============================================================ */
typedef struct Utilizador {
    int id;
    char nome_completo[MAX_NOME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char universidade[MAX_UNIVERSIDADE];
    char curso[MAX_CURSO];
    int ano_academico;
    int idade;

    /* opcionais */
    char cidade[MAX_CIDADE];
    char email[MAX_EMAIL];
    char bio[MAX_BIO];
    EstadoUtilizador estado;

    ListaInteresses *interesses;
} Utilizador;

/* ============================================================
 * TabelaHash — encadeamento (chaining) de Utilizador*
 * ============================================================ */
typedef struct NoHash {
    Utilizador *utilizador;    
    struct NoHash *seguinte;
} NoHash;

typedef struct TabelaHash {
    NoHash **baldes;
    int capacidade;
    int num_utilizadores;
} TabelaHash;

/* ============================================================
 * GrafoAmizades — array de vértices indexado pelo id do
 * utilizador, cada um com lista de adjacência própria
 * ============================================================ */
typedef struct NoAdjacencia {
    int id_amigo;
    struct NoAdjacencia *seguinte;
} NoAdjacencia;

typedef struct {
    int id;
    int usado;              /* 1 se este vértice está ativo */
    int grau;                /* nº de amigos diretos (cache) */
    NoAdjacencia *amigos;
} Vertice;

typedef struct GrafoAmizades {
    Vertice *vertices;       /* array indexado por id */
    int capacidade;          /* tamanho atual do array */
    int num_vertices;
    int num_arestas;
} GrafoAmizades;

/* ============================================================
 * Fila — genérica (FIFO), aponta para void*
 * ============================================================ */
typedef struct NoFila {
    void *dado;
    struct NoFila *seguinte;
} NoFila;

typedef struct Fila {
    NoFila *frente;
    NoFila *tras;
    int tamanho;
} Fila;

/* ============================================================
 * Pilha — genérica (LIFO), aponta para void*
 * ============================================================ */
typedef struct NoPilha {
    void *dado;
    struct NoPilha *seguinte;
} NoPilha;

typedef struct Pilha {
    NoPilha *topo;
    int tamanho;
} Pilha;

/* ============================================================
 * GestorPedidos — lista ligada de pedidos de amizade
 * ============================================================ */
typedef struct NoPedido {
    int id_remetente;
    int id_destinatario;
    EstadoPedido estado;
    DataHora data_envio;
    struct NoPedido *seguinte;
} NoPedido;

typedef struct GestorPedidos {
    NoPedido *inicio;
    NoPedido *fim;
    int num_pedidos;
} GestorPedidos;

/* ============================================================
 * ListaMensagens — lista ligada de mensagens privadas
 * ============================================================ */
typedef struct NoMensagem {
    int id_remetente;
    int id_destinatario;
    char conteudo[MAX_CONTEUDO];
    DataHora data_hora;
    struct NoMensagem *seguinte;
} NoMensagem;

typedef struct ListaMensagens {
    NoMensagem *inicio;
    NoMensagem *fim;
    int num_mensagens;
} ListaMensagens;

/* ============================================================
 * ListaPublicacoes — lista ligada de posts; cada post tem a
 * sua própria lista ligada de likes e de comentários
 * ============================================================ */
typedef struct NoComentario {
    int id_comentario;
    int id_post;
    int id_autor;
    char conteudo[MAX_CONTEUDO];
    DataHora data_hora;
    struct NoComentario *seguinte;
} NoComentario;

typedef struct NoLike {
    int id_utilizador;
    struct NoLike *seguinte;
} NoLike;

typedef struct NoPublicacao {
    int id_post;
    int id_autor;
    char conteudo[MAX_CONTEUDO];
    DataHora data_hora;

    NoLike *likes;
    int num_likes;

    NoComentario *comentarios;
    int num_comentarios;

    struct NoPublicacao *seguinte;
} NoPublicacao;

typedef struct ListaPublicacoes {
    NoPublicacao *inicio;       /* cabeça = publicação mais recente */
    int num_publicacoes;
    int proximo_id_post;        /* gerador de ids de posts */
    int proximo_id_comentario;  /* gerador de ids de comentários */
} ListaPublicacoes;

/* ============================================================
 * RedeSocial — agrega todos os TADs acima + estado de sessão
 * ============================================================ */
typedef struct RedeSocial {
    TabelaHash *utilizadores;
    GrafoAmizades *amizades;
    GestorPedidos *pedidos;
    ListaMensagens *mensagens;
    ListaPublicacoes *publicacoes;

    int sessao_atual;  /* id do utilizador com sessão ativa, -1 se nenhum */
    int proximo_id;     /* gerador de ids únicos para novos utilizadores */
} RedeSocial;

/* ============================================================
 * Persistencia - Registro do User
 * ============================================================ */

typedef struct {
    int id;
    char nome_completo[MAX_NOME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char universidade[MAX_UNIVERSIDADE];
    char curso[MAX_CURSO];
    int ano_academico;
    int idade;
    char cidade[MAX_CIDADE];
    char email[MAX_EMAIL];
    char bio[MAX_BIO];
    EstadoUtilizador estado;
    int num_interesses;
} RegistoUtilizador;

typedef struct {
    int total_utilizadores;
    int total_amizades;
    int id_utilizador_mais_amigos;
    char universidade_mais_popular[MAX_UNIVERSIDADE];
    char curso_mais_popular[MAX_CURSO];
    char interesse_mais_comum[MAX_INTERESSE];
} Estatisticas;

typedef struct {
    int id_remetente;
    int id_destinatario;
    char conteudo[MAX_CONTEUDO];
    DataHora data_hora;
} MensagemInfo;

typedef struct {
    int id_remetente;
    int id_destinatario;
    EstadoPedido estado;
    DataHora data_envio;
} PedidoAmizade;

typedef struct {
    int id_comentario;
    int id_post;
    int id_autor;
    char conteudo[MAX_CONTEUDO];
    DataHora data_hora;
} ComentarioInfo;

typedef struct {
    int id_post;
    int id_autor;
    char conteudo[MAX_CONTEUDO];
    DataHora data_hora;
    int num_likes;
    int num_comentarios;
} PublicacaoInfo;

typedef struct {
    int id_utilizador;
    int pontuacao;
} ResultadoRecomendacao;

#endif /* TADS_H */    