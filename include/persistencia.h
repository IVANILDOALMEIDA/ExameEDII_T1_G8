/**
 * persistencia.h
 * -----------------------------------------------------------------------
 * Módulo de PERSISTÊNCIA : carrega e grava em ficheiro binário
 * (.dat) o estado de cada TAD do sistema. Cada função "guardar_*"
 * percorre o TAD em memória e escreve-o no ficheiro; cada "carregar_*"
 * lê o ficheiro e reconstrói o TAD correspondente.
 *
 * Nomes de ficheiro:
 *   usuarios.dat, amizades.dat, mensagens.dat,
 *   posts.dat, comentarios.dat, interesses.dat
 * -----------------------------------------------------------------------
 */
 
#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H
 
#include "hash_table.h"
#include "grafo_amizades.h"
#include "pedidos_amizade.h"
#include "mensagens.h"
#include "publicacoes.h"
 
/* ------------------------------ Utilizadores ------------------------------ */
int guardar_utilizadores(const char *caminho_ficheiro, const TabelaHash *utilizadores);
TabelaHash *carregar_utilizadores(const char *caminho_ficheiro);
 
/* ------------------------------ Amizades ----------------------------------- */
int guardar_amizades(const char *caminho_ficheiro, const GrafoAmizades *g);
int carregar_amizades(const char *caminho_ficheiro, GrafoAmizades *g);
 
/* ------------------------------ Pedidos de amizade -------------------------- */
int guardar_pedidos(const char *caminho_ficheiro, const GestorPedidos *gp);
GestorPedidos *carregar_pedidos(const char *caminho_ficheiro);
 
/* ------------------------------ Mensagens ----------------------------------- */
int guardar_mensagens(const char *caminho_ficheiro, const ListaMensagens *lm);
ListaMensagens *carregar_mensagens(const char *caminho_ficheiro);
 
/* ------------------------------ Publicações / comentários -------------------- */
int guardar_publicacoes(const char *caminho_ficheiro, const ListaPublicacoes *lp);
ListaPublicacoes *carregar_publicacoes(const char *caminho_ficheiro);
 
 
/* Verifica se um ficheiro de dados existe e é legível*/
int persistencia_ficheiro_existe(const char *caminho_ficheiro);
 
#endif /* PERSISTENCIA_H */