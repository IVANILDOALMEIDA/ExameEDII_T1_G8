/**
 * mensagens.h
 * -----------------------------------------------------------------------
 * TAD ListaMensagens — histórico de mensagens privadas trocadas entre
 * utilizadores , implementado com lista ligada,
 * mantendo a ordem cronológica de envio.
 * -----------------------------------------------------------------------
 */
 
#ifndef MENSAGENS_H
#define MENSAGENS_H
 
#include "utils.h"
#include "TAD.h"
 
 

 
/* Cria o histórico de mensagens (vazio). */
ListaMensagens *mensagens_criar(void);
 
/* Liberta toda a memória do histórico. */
void mensagens_destruir(ListaMensagens *lm);
 
/* Regista o envio de uma mensagem privada. */
int mensagens_enviar(ListaMensagens *lm, int id_remetente, int id_destinatario, const char *conteudo);
 
/* Devolve a conversa completa (por ordem cronológica) entre dois
 * utilizadores. Array alocado (a libertar pelo chamador); 'num' recebe o tamanho. */
MensagemInfo *mensagens_obter_conversa(const ListaMensagens *lm, int id_a, int id_b, int *num);
 
/* Apaga todo o histórico de conversa entre dois utilizadores. */
int mensagens_apagar_conversa(ListaMensagens *lm, int id_a, int id_b);
 
/* Conta quantas mensagens existem numa conversa. */
int mensagens_contar_conversa(const ListaMensagens *lm, int id_a, int id_b);
 
/* Devolve a lista de ids de utilizadores com quem 'id' já trocou
 * mensagens, ordenada pela mais recente — útil para a lista de "chats". */
int *mensagens_listar_conversas_de(const ListaMensagens *lm, int id, int *num_conversas);
 
#endif /* MENSAGENS_H */