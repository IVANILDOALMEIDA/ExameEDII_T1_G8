/**
 * utils.h
 * -----------------------------------------------------------------------
 * Constantes, enumerados e tipos simples partilhados por todos os módulos
 * do UniConnect. Serve apenas de base comum para evitar 
 * duplicação e dependências cruzadas desnecessárias.
 * -----------------------------------------------------------------------
 */
 
#ifndef UTILS_H
#define UTILS_H
 
/* ---------------------- Limites de tamanho (strings) ------------------ */
#define MAX_NOME            80
#define MAX_USERNAME        30
#define MAX_PASSWORD        30   /* idealmente guardar apenas hash, ver persistencia.h */
#define MAX_UNIVERSIDADE    80
#define MAX_CURSO           60
#define MAX_CIDADE          50
#define MAX_EMAIL           60
#define MAX_INTERESSE       40
#define MAX_BIO             200
#define MAX_CONTEUDO        280   /* publicações / mensagens / comentários */
#define MAX_FICHEIRO        100
 
/* ---------------------------- Estados ---------------------------------- */
typedef enum {
    ESTADO_OFFLINE = 0,
    ESTADO_ONLINE  = 1
} EstadoUtilizador;
 
typedef enum {
    PEDIDO_PENDENTE = 0,
    PEDIDO_ACEITE   = 1,
    PEDIDO_RECUSADO = 2,
    PEDIDO_CANCELADO = 3
} EstadoPedido;
 
/* Código de erro genérico devolvido pelas funções dos vários TADs.
 * Cada módulo pode estender este conjunto se necessário. */
typedef enum {
    OK = 0,
    ERRO_MEMORIA = -1,
    ERRO_NAO_ENCONTRADO = -2,
    ERRO_JA_EXISTE = -3,
    ERRO_PARAMETRO_INVALIDO = -4,
    ERRO_FICHEIRO = -5,
    ERRO_NAO_AUTORIZADO = -6
} CodigoErro;
 
/* ------------------------------ Data/Hora ------------------------------ */
typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
} DataHora;
 
/* Devolve a data/hora atual do sistema. */
DataHora obter_data_hora_atual(void);
 
/* Compara duas datas: <0 se a < b, 0 se iguais, >0 se a > b.
 * Útil para ordenar mensagens/publicações cronologicamente. */
int comparar_data_hora(DataHora a, DataHora b);

int comparar_strings_ci(const char *a, const char *b);
 
#endif /* UTILS_H */