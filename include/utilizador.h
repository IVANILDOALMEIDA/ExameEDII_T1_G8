/**
 * utilizador.h
 * -----------------------------------------------------------------------
 * TAD Utilizador — representa o perfil de um estudante no UniConnect.
 * -----------------------------------------------------------------------
 */
 
#ifndef UTILIZADOR_H
#define UTILIZADOR_H
 
#include "utils.h"
#include "interesse.h"
#include "TAD.h"

 
/* Cria um novo utilizador com os campos obrigatórios.
 * 'id' deve ser gerado pelo módulo responsável (ex.: rede_social.c).
 * Retorna NULL em caso de erro de alocação. */
Utilizador *utilizador_criar(int id,
                              const char *nome_completo,
                              const char *username,
                              const char *password,
                              const char *universidade,
                              const char *curso,
                              int ano_academico,
                              int idade);
 
/* Liberta toda a memória do utilizador, incluindo a lista de interesses. */
void utilizador_destruir(Utilizador *u);
 
/* ------------------------------ Autenticação ---------------------------- */
 
/* Verifica se a password fornecida corresponde à do utilizador. */
int utilizador_autenticar(const Utilizador *u, const char *password);
 
/* Altera a password (exige a password antiga por segurança). */
int utilizador_alterar_password(Utilizador *u, const char *password_antiga, const char *password_nova);
 
/* ------------------------------ Getters ---------------------------------- */
 
int utilizador_get_id(const Utilizador *u);
const char *utilizador_get_nome(const Utilizador *u);
const char *utilizador_get_username(const Utilizador *u);
const char *utilizador_get_universidade(const Utilizador *u);
const char *utilizador_get_curso(const Utilizador *u);
int utilizador_get_ano_academico(const Utilizador *u);
int utilizador_get_idade(const Utilizador *u);
const char *utilizador_get_cidade(const Utilizador *u);
const char *utilizador_get_email(const Utilizador *u);
const char *utilizador_get_bio(const Utilizador *u);
EstadoUtilizador utilizador_get_estado(const Utilizador *u);
const ListaInteresses *utilizador_get_interesses(const Utilizador *u);
 
/* ------------------------------ Setters / edição ------------------------- */
 
int utilizador_set_nome(Utilizador *u, const char *novo_nome);
int utilizador_set_curso(Utilizador *u, const char *novo_curso);
int utilizador_set_ano_academico(Utilizador *u, int novo_ano);
int utilizador_set_cidade(Utilizador *u, const char *nova_cidade);
int utilizador_set_email(Utilizador *u, const char *novo_email);
int utilizador_set_bio(Utilizador *u, const char *nova_bio);
void utilizador_set_estado(Utilizador *u, EstadoUtilizador estado);
 
/* Adiciona/remove um interesse (delega no TAD ListaInteresses). */
int utilizador_adicionar_interesse(Utilizador *u, const char *interesse);
int utilizador_remover_interesse(Utilizador *u, const char *interesse);
 
/* ------------------------------ Utilitários ------------------------------ */
 
/* Imprime o perfil completo do utilizador (modo consulta/feed). */
void utilizador_imprimir_perfil(const Utilizador *u);
 
/* Compara dois utilizadores por nome (útil para ordenação/pesquisa). */
int utilizador_comparar_por_nome(const Utilizador *a, const Utilizador *b);
 
#endif /* UTILIZADOR_H */