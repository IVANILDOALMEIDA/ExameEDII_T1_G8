/**
 * publicacoes.h
 * -----------------------------------------------------------------------
 * TAD ListaPublicacoes — publicações curtas (posts), do feed simplificado,
 *  implementado com listas ligadas : uma lista de posts, e cada post com a
 * sua própria lista ligada de comentários.
 * -----------------------------------------------------------------------
 */
 
#ifndef PUBLICACOES_H
#define PUBLICACOES_H
 
#include "utils.h"
#include "grafo_amizades.h"
#include "TAD.h"
 

 
/* Cria o repositório de publicações (vazio). */
ListaPublicacoes *publicacoes_criar(void);
 
/* Liberta toda a memória (posts, likes e comentários). */
void publicacoes_destruir(ListaPublicacoes *lp);
 
/* ------------------------------ Posts ------------------------------------ */
 
/* Cria uma nova publicação. Retorna o id do post criado, ou um código
 * de erro negativo (ver CodigoErro) em caso de falha. */
int publicacoes_criar_post(ListaPublicacoes *lp, int id_autor, const char *conteudo);
 
/* Remove uma publicação (e respetivos comentários/likes). */
int publicacoes_remover_post(ListaPublicacoes *lp, int id_post, int id_autor);
 
/* ------------------------------ Likes ------------------------------------- */
 
int publicacoes_curtir(ListaPublicacoes *lp, int id_post, int id_utilizador);
int publicacoes_descurtir(ListaPublicacoes *lp, int id_post, int id_utilizador);
int publicacoes_contar_likes(const ListaPublicacoes *lp, int id_post);
int publicacoes_utilizador_curtiu(const ListaPublicacoes *lp, int id_post, int id_utilizador);
 
/* ------------------------------ Comentários -------------------------------- */
 
int publicacoes_comentar(ListaPublicacoes *lp, int id_post, int id_autor, const char *conteudo);
 
/* Devolve os comentários de um post, por ordem cronológica.
 * Array alocado (a libertar pelo chamador); 'num' recebe o tamanho. */
ComentarioInfo *publicacoes_listar_comentarios(const ListaPublicacoes *lp, int id_post, int *num);
 
/* ------------------------------ Consulta / feed ---------------------------- */
 
/* Lista todas as publicações de um autor (mais recentes primeiro). */
PublicacaoInfo *publicacoes_listar_de_utilizador(const ListaPublicacoes *lp, int id_autor, int *num);
 
/* Gera o feed de um utilizador: publicações dos seus amigos diretos
 * (usa o GrafoAmizades para saber quem são os amigos), ordenadas da
 * mais recente para a mais antiga. */
PublicacaoInfo *publicacoes_feed_amigos(const ListaPublicacoes *lp, const GrafoAmizades *g, int id_utilizador, int *num);
 
#endif /* PUBLICACOES_H */