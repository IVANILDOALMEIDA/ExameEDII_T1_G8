/**
 * hash_table.h
 * -----------------------------------------------------------------------
 * TAD TabelaHash — tabela de dispersão (hashing) usada para guardar
 * todos os utilizadores do sistema e permitir login / pesquisa por
 * username ou id em tempo médio O(1).
 * Colisões resolvidas por encadeamento (chaining)
 * -----------------------------------------------------------------------
 */
 
#ifndef HASH_TABLE_H
#define HASH_TABLE_H
 
#include "utilizador.h"
#include "TAD.h"
 
/* Cria uma tabela hash com 'capacidade' baldes iniciais. */
TabelaHash *hash_criar(int capacidade);
 
/* Liberta a tabela. 'libertar_utilizadores' indica se deve também
 * chamar utilizador_destruir() sobre cada entrada (dono dos dados). */
void hash_destruir(TabelaHash *tabela, int libertar_utilizadores);
 
/* Função de dispersão (ex.: djb2) aplicada a uma chave textual. */
unsigned int hash_funcao(const char *chave, int capacidade);
 
/* Insere um utilizador, indexado pelo seu username.
 * Retorna OK ou ERRO_JA_EXISTE se o username já estiver ocupado. */
int hash_inserir(TabelaHash *tabela, Utilizador *u);
 
/* Remove o utilizador identificado por username (não liberta a memória
 * do Utilizador, apenas a entrada da tabela — decisão do chamador). */
int hash_remover(TabelaHash *tabela, const char *username);
 
/* Pesquisa O(1) médio por username (ex.: login). */
Utilizador *hash_procurar_username(const TabelaHash *tabela, const char *username);
 
/* Pesquisa por id — pode exigir percorrer todos os baldes (O(n)) a não
 * ser que se mantenha um índice auxiliar; documentar a opção escolhida. */
Utilizador *hash_procurar_id(const TabelaHash *tabela, int id);
 
/* Verifica apenas a existência de um username (sem devolver o utilizador). */
int hash_existe_username(const TabelaHash *tabela, const char *username);
 
/* Número total de utilizadores armazenados. */
int hash_contar_utilizadores(const TabelaHash *tabela);
 
/* Aplica 'callback' a todos os utilizadores da tabela — usado para
 * pesquisas avançadas, estatísticas e gravação em ficheiro. */
void hash_para_cada(const TabelaHash *tabela, void (*callback)(Utilizador *u, void *contexto), void *contexto);
 
/* Devolve um array (alocado dinamicamente, a libertar pelo chamador
 * com free()) com apontadores para todos os utilizadores. */
Utilizador **hash_listar_todos(const TabelaHash *tabela, int *num_utilizadores);
 
#endif /* HASH_TABLE_H */