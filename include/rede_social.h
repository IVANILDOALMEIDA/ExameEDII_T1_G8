/**
 * rede_social.h
 * -----------------------------------------------------------------------
 * TAD RedeSocial — estrutura de topo que agrega todos os outros TADs
 * (utilizadores, amizades, pedidos, mensagens, publicações) e gere a
 * sessão do utilizador atualmente ligado. É este módulo que o main.c /
 * interface de menus deve usar diretamente, sem precisar de
 * conhecer os detalhes internos de cada estrutura de dados.
 * -----------------------------------------------------------------------
 */
 
#ifndef REDE_SOCIAL_H
#define REDE_SOCIAL_H
 
#include "utils.h"
#include "utilizador.h"
#include "hash_table.h"
#include "grafo_amizades.h"
#include "pedidos_amizade.h"
#include "mensagens.h"
#include "publicacoes.h"
#include "TAD.h"
 
/* ------------------------- Ciclo de vida da aplicação ---------------------- */
 
/* Inicializa todos os TADs e carrega os dados persistidos em ficheiro
 * ("Ao iniciar o programa: carregar os dados"). */
RedeSocial *rede_social_iniciar(void);
 
/* Grava todos os TADs em ficheiro e liberta toda a memória
 * ( "Ao terminar: guardar todas as alterações"). */
void rede_social_terminar(RedeSocial *rede);
 
/* ------------------------------ Gestão de contas ---------------------------- */
 
/* Cria uma nova conta. Gera o id internamente. Retorna o id criado, ou
 * um código de erro negativo (ex.: ERRO_JA_EXISTE se o username já existir). */
int rede_social_criar_conta(RedeSocial *rede,
                             const char *nome_completo,
                             const char *username,
                             const char *password,
                             const char *universidade,
                             const char *curso,
                             int ano_academico,
                             int idade);
 
/* Autentica e abre sessão. Retorna o id do utilizador autenticado, ou
 * código de erro negativo. */
int rede_social_iniciar_sessao(RedeSocial *rede, const char *username, const char *password);
 
/* Termina a sessão atualmente aberta (define utilizador como offline). */
void rede_social_terminar_sessao(RedeSocial *rede);
 
/* Devolve o id do utilizador com sessão ativa, ou -1 se ninguém tiver sessão. */
int rede_social_sessao_atual(const RedeSocial *rede);
 
/* Remove definitivamente a conta de um utilizador (e os respetivos
 * dados associados: amizades, pedidos, etc.). */
int rede_social_eliminar_conta(RedeSocial *rede, int id_utilizador);
 
/* ------------------------------ Acesso aos sub-TADs -------------------------
 * Expostos como "somente leitura" (apontador constante) para que os
 * módulos de pesquisa/recomendação/estatísticas operem diretamente
 * sobre eles sem duplicar funções "wrapper" para cada operação. */
 
const TabelaHash *rede_social_get_utilizadores(const RedeSocial *rede);
const GrafoAmizades *rede_social_get_amizades(const RedeSocial *rede);
GestorPedidos *rede_social_get_pedidos(RedeSocial *rede);
ListaMensagens *rede_social_get_mensagens(RedeSocial *rede);
ListaPublicacoes *rede_social_get_publicacoes(RedeSocial *rede);
 
#endif /* REDE_SOCIAL_H */