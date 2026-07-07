/**
 * main.c
 * -----------------------------------------------------------------------
 *                            UNICONNECT
 * -----------------------------------------------------------------------
 * Interface de terminal (CLI) que liga a arquitetura RedeSocial (TADs)
 * ao utilizador final. Este ficheiro NAO define nenhuma estrutura de
 * dados nova - usa apenas a API publica exposta pelos modulos existentes
 * (rede_social.h, pesquisa.h, recomendacao.h, estatisticas.h).
 *
 * NOTAS IMPORTANTES (ler antes de compilar):
 *
 * 1) CONST-CORRECTNESS: rede_social_get_amizades() devolve um
 *    `const GrafoAmizades *`, mas pedidos_aceitar() e
 *    grafo_remover_aresta() exigem um `GrafoAmizades *` mutavel (porque
 *    inserem/removem arestas). Isto e uma inconsistencia no header
 *    original: o modulo promete acesso "so de leitura" mas duas
 *    operacoes do menu (aceitar pedido de amizade, remover amigo)
 *    precisam de escrever no grafo. O contorno usado abaixo e um cast
 *    explicito `(GrafoAmizades *)`. Isto FUNCIONA mas nao e limpo -
 *    o correto e adicionar a rede_social.h/.c uma funcao:
 *        GrafoAmizades *rede_social_get_amizades_editavel(RedeSocial *rede);
 *    e usar essa em vez do cast.
 *
 * 2) "Seguir utilizador" (do menu antigo) NAO existe nesta arquitetura:
 *    o grafo e nao-dirigido, so ha amizade simetrica. Essa opcao foi
 *    removida do menu.
 *
 * 3) top5_compatibilidade() devolve SEMPRE um array de 5 posicoes; os
 *    slots nao usados vem marcados com id_utilizador == -1. Isto so se
 *    percebe a ler o .c (nao esta documentado no .h), por isso o
 *    tratamento do sentinela esta explicito no codigo abaixo.
 * -----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rede_social.h"
#include "pesquisa.h"
#include "recomendacao.h"
#include "estatisticas.h"

#ifdef _WIN32
    #define LIMPAR_ECRA() system("cls")
#else
    #define LIMPAR_ECRA() system("clear")
#endif

/* ============================================================
 * Prototipos (para nao depender da ordem de definicao no ficheiro)
 * ============================================================ */

static void limpar_buffer(void);
static int  ler_linha(char *destino, int tamanho);
static void ler_texto(const char *prompt, char *destino, int tamanho, int obrigatorio);
static int  ler_inteiro(const char *prompt, int minimo, int maximo);
static void pausar(void);
static void imprimir_cartao_utilizador(const Utilizador *u);

static void ecra_cadastro(RedeSocial *rede);
static void ecra_login(RedeSocial *rede);
static void menu_principal(RedeSocial *rede, int id_sessao);

static void ecra_ver_perfil(RedeSocial *rede, int id_sessao);
static void ecra_editar_perfil(RedeSocial *rede, int id_sessao);
static void ecra_procurar(RedeSocial *rede);
static void ecra_enviar_pedido(RedeSocial *rede, int id_sessao);
static void ecra_pedidos_recebidos(RedeSocial *rede, int id_sessao);
static void ecra_pedidos_enviados(RedeSocial *rede, int id_sessao);
static void ecra_sugestoes(RedeSocial *rede, int id_sessao);
static void ecra_top5(RedeSocial *rede, int id_sessao);
static void ecra_meus_amigos(RedeSocial *rede, int id_sessao);
static void ecra_remover_amigo(RedeSocial *rede, int id_sessao);
static void ecra_amigos_comuns(RedeSocial *rede, int id_sessao);
static void ecra_enviar_mensagem(RedeSocial *rede, int id_sessao);
static void ecra_ver_conversa(RedeSocial *rede, int id_sessao);
static void ecra_feed(RedeSocial *rede, int id_sessao);
static void ecra_criar_post(RedeSocial *rede, int id_sessao);
static void ecra_comentar_curtir(RedeSocial *rede, int id_sessao);
static void ecra_ver_todos(RedeSocial *rede);
static void ecra_estatisticas(RedeSocial *rede);
static int  ecra_eliminar_conta(RedeSocial *rede, int id_sessao);

/* ============================================================
 * Utilitarios de I/O seguro
 * ============================================================
 * O main.c original usava scanf("%d", ...) e scanf(" %49[^\n]", ...)
 * misturados, o que deixa lixo no buffer de entrada e nao valida nada
 * (idade nao numerica, strings maiores que o buffer, etc.). Aqui tudo
 * passa por fgets + validacao explicita.
 */

/* Descarta o resto da linha atual no stdin (usado quando fgets nao
 * apanhou o '\n', ou seja, a linha era maior que o buffer). */
static void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta */
    }
}

/* Le uma linha completa (sem o '\n' final) para 'destino', com no
 * maximo 'tamanho - 1' caracteres uteis. Nunca faz overflow.
 * Devolve 0 se o stdin chegou ao fim (EOF / stream fechado), 1 caso
 * contrario. Isto e importante: sem esta deteccao, um obrigatorio=1
 * em ler_texto entraria em loop infinito assim que o stdin acabasse
 * (ex.: input redirecionado de um ficheiro, ou Ctrl+D). */
static int ler_linha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return 0;
    }
    size_t len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    } else if (len == (size_t)(tamanho - 1)) {
        /* a linha nao coube toda no buffer: descarta o resto */
        limpar_buffer();
    }
    return 1;
}

/* Pede texto ao utilizador ate obter algo valido.
 * Se 'obrigatorio' for verdadeiro, nao aceita string vazia.
 * Se o stdin fechar (EOF), termina o programa de forma limpa em vez
 * de entrar em loop infinito. */
static void ler_texto(const char *prompt, char *destino, int tamanho, int obrigatorio) {
    while (1) {
        printf("%s", prompt);
        if (!ler_linha(destino, tamanho)) {
            printf("\n[!] Entrada terminada inesperadamente. A encerrar...\n");
            exit(1);
        }
        if (obrigatorio && destino[0] == '\0') {
            printf("  [!] Este campo nao pode ficar vazio.\n");
            continue;
        }
        return;
    }
}

/* Pede um inteiro dentro de [minimo, maximo], repetindo ate ser valido.
 * Isto substitui os scanf("%d", ...) sem validacao do main.c original -
 * la, escrever letras onde se esperava a idade deixava a variavel por
 * inicializar e corrompia o fluxo do programa. */
static int ler_inteiro(const char *prompt, int minimo, int maximo) {
    char linha[64];
    while (1) {
        printf("%s", prompt);
        if (!ler_linha(linha, sizeof(linha))) {
            printf("\n[!] Entrada terminada inesperadamente. A encerrar...\n");
            exit(1);
        }

        char *fim = NULL;
        long valor = strtol(linha, &fim, 10);

        if (fim == linha || *fim != '\0') {
            printf("  [!] Isso nao e um numero valido. Tenta novamente.\n");
            continue;
        }
        if (valor < minimo || valor > maximo) {
            printf("  [!] O valor tem de estar entre %d e %d.\n", minimo, maximo);
            continue;
        }
        return (int) valor;
    }
}

static void pausar(void) {
    char tmp[8];
    printf("\nPressione ENTER para continuar...");
    ler_linha(tmp, sizeof(tmp));
}

static void imprimir_cartao_utilizador(const Utilizador *u) {
    if (u == NULL) {
        return;
    }
    printf("  [%d] @%-15s %-25s | %s - %s (%dº ano)\n",
           utilizador_get_id(u),
           utilizador_get_username(u),
           utilizador_get_nome(u),
           utilizador_get_universidade(u),
           utilizador_get_curso(u),
           utilizador_get_ano_academico(u));
}

/* ============================================================
 * Ecrans iniciais (sem sessao)
 * ============================================================ */

static void ecra_cadastro(RedeSocial *rede) {
    char nome[MAX_NOME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char universidade[MAX_UNIVERSIDADE];
    char curso[MAX_CURSO];
    int ano_academico, idade;

    LIMPAR_ECRA();
    printf("=== CRIAR CONTA ===\n\n");

    ler_texto("Nome completo: ",  nome,         sizeof(nome),         1);
    ler_texto("Username: ",       username,     sizeof(username),     1);
    ler_texto("Password: ",       password,     sizeof(password),     1);
    ler_texto("Universidade: ",   universidade, sizeof(universidade), 1);
    ler_texto("Curso: ",          curso,        sizeof(curso),        1);
    ano_academico = ler_inteiro("Ano academico (1-10): ", 1, 10);
    idade         = ler_inteiro("Idade (16-100): ", 16, 100);

    int id = rede_social_criar_conta(rede, nome, username, password,
                                      universidade, curso, ano_academico, idade);

    if (id == ERRO_JA_EXISTE) {
        printf("\n[!] Esse username ja esta em uso. Escolhe outro.\n");
        pausar();
        return;
    }
    if (id < 0) {
        printf("\n[!] Nao foi possivel criar a conta (erro %d).\n", id);
        pausar();
        return;
    }

    int qtd = ler_inteiro("Quantos interesses queres adicionar agora? (0-5): ", 0, 5);
    if (qtd > 0) {
        const TabelaHash *tab = rede_social_get_utilizadores(rede);
        Utilizador *u = hash_procurar_id(tab, id);
        for (int i = 0; i < qtd; i++) {
            char interesse[MAX_INTERESSE];
            char prompt[48];
            snprintf(prompt, sizeof(prompt), "  Interesse %d: ", i + 1);
            ler_texto(prompt, interesse, sizeof(interesse), 1);
            if (u != NULL) {
                utilizador_adicionar_interesse(u, interesse);
            }
        }
    }

    printf("\n[OK] Conta criada com sucesso! O teu ID e: %d\n", id);
    pausar();
}

static void ecra_login(RedeSocial *rede) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    LIMPAR_ECRA();
    printf("=== LOGIN ===\n\n");
    ler_texto("Username: ", username, sizeof(username), 1);
    ler_texto("Password: ", password, sizeof(password), 1);

    int id = rede_social_iniciar_sessao(rede, username, password);
    if (id < 0) {
        printf("\n[!] Username ou password incorretos.\n");
        pausar();
        return;
    }

    menu_principal(rede, id);
}

/* ============================================================
 * Menu principal (sessao ativa)
 * ============================================================ */

static void menu_principal(RedeSocial *rede, int id_sessao) {
    char opcao[8];
    const TabelaHash *tab_login = rede_social_get_utilizadores(rede);
    Utilizador *eu = hash_procurar_id(tab_login, id_sessao);
    char nome_sessao[MAX_USERNAME];
    strncpy(nome_sessao, eu != NULL ? utilizador_get_username(eu) : "?", sizeof(nome_sessao) - 1);
    nome_sessao[sizeof(nome_sessao) - 1] = '\0';

    do {
        LIMPAR_ECRA();
        printf("======================= UniConnect =======================\n");
        printf(" Sessao: @%s (id %d)\n", nome_sessao, id_sessao);
        printf("------------------------------------------------------------\n");
        printf(" 1  - Ver o meu perfil          | 11 - Amigos em comum\n");
        printf(" 2  - Editar perfil             | 12 - Enviar mensagem\n");
        printf(" 3  - Procurar utilizadores     | 13 - Ver conversa\n");
        printf(" 4  - Enviar pedido de amizade  | 14 - Feed de amigos\n");
        printf(" 5  - Pedidos recebidos         | 15 - Criar publicacao\n");
        printf(" 6  - Pedidos enviados          | 16 - Comentar / curtir post\n");
        printf(" 7  - Sugestoes de amigos       | 17 - Ver todos os utilizadores\n");
        printf(" 8  - Top 5 compatibilidade     | 18 - Estatisticas da rede\n");
        printf(" 9  - Os meus amigos            | 19 - Eliminar a minha conta\n");
        printf(" 10 - Remover amigo             |\n");
        printf("------------------------------------------------------------\n");
        printf(" 0  - Terminar sessao\n");
        printf("============================================================\n");
        ler_texto("Opcao: ", opcao, sizeof(opcao), 1);

        if      (strcmp(opcao, "1")  == 0) ecra_ver_perfil(rede, id_sessao);
        else if (strcmp(opcao, "2")  == 0) ecra_editar_perfil(rede, id_sessao);
        else if (strcmp(opcao, "3")  == 0) ecra_procurar(rede);
        else if (strcmp(opcao, "4")  == 0) ecra_enviar_pedido(rede, id_sessao);
        else if (strcmp(opcao, "5")  == 0) ecra_pedidos_recebidos(rede, id_sessao);
        else if (strcmp(opcao, "6")  == 0) ecra_pedidos_enviados(rede, id_sessao);
        else if (strcmp(opcao, "7")  == 0) ecra_sugestoes(rede, id_sessao);
        else if (strcmp(opcao, "8")  == 0) ecra_top5(rede, id_sessao);
        else if (strcmp(opcao, "9")  == 0) ecra_meus_amigos(rede, id_sessao);
        else if (strcmp(opcao, "10") == 0) ecra_remover_amigo(rede, id_sessao);
        else if (strcmp(opcao, "11") == 0) ecra_amigos_comuns(rede, id_sessao);
        else if (strcmp(opcao, "12") == 0) ecra_enviar_mensagem(rede, id_sessao);
        else if (strcmp(opcao, "13") == 0) ecra_ver_conversa(rede, id_sessao);
        else if (strcmp(opcao, "14") == 0) ecra_feed(rede, id_sessao);
        else if (strcmp(opcao, "15") == 0) ecra_criar_post(rede, id_sessao);
        else if (strcmp(opcao, "16") == 0) ecra_comentar_curtir(rede, id_sessao);
        else if (strcmp(opcao, "17") == 0) ecra_ver_todos(rede);
        else if (strcmp(opcao, "18") == 0) ecra_estatisticas(rede);
        else if (strcmp(opcao, "19") == 0) {
            if (ecra_eliminar_conta(rede, id_sessao)) {
                return; /* conta apagada -> sai do menu, ja nao ha sessao */
            }
        }
        else if (strcmp(opcao, "0") == 0) {
            rede_social_terminar_sessao(rede);
            printf("\nSessao terminada.\n");
            pausar();
        }
        else {
            printf("\n[!] Opcao invalida.\n");
            pausar();
        }
    } while (strcmp(opcao, "0") != 0);
}

/* ============================================================
 * Perfil
 * ============================================================ */

static void ecra_ver_perfil(RedeSocial *rede, int id_sessao) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    Utilizador *u = hash_procurar_id(tab, id_sessao);

    LIMPAR_ECRA();
    printf("=== O MEU PERFIL ===\n");
    if (u == NULL) {
        printf("[!] Utilizador nao encontrado (isto nao deveria acontecer).\n");
    } else {
        utilizador_imprimir_perfil(u);
    }
    pausar();
}

static void ecra_editar_perfil(RedeSocial *rede, int id_sessao) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    Utilizador *u = hash_procurar_id(tab, id_sessao);
    if (u == NULL) {
        printf("[!] Utilizador nao encontrado.\n");
        pausar();
        return;
    }

    char opcao[4];
    do {
        LIMPAR_ECRA();
        printf("=== EDITAR PERFIL ===\n\n");
        utilizador_imprimir_perfil(u);
        printf("\n1 - Alterar nome\n2 - Alterar curso\n3 - Alterar ano academico\n");
        printf("4 - Alterar cidade\n5 - Alterar email\n6 - Alterar bio\n");
        printf("7 - Adicionar interesse\n8 - Remover interesse\n9 - Alterar password\n");
        printf("0 - Voltar\n");
        ler_texto("Opcao: ", opcao, sizeof(opcao), 1);

        if (strcmp(opcao, "1") == 0) {
            char v[MAX_NOME];
            ler_texto("Novo nome: ", v, sizeof(v), 1);
            utilizador_set_nome(u, v);
            printf("[OK] Nome atualizado.\n");
        } else if (strcmp(opcao, "2") == 0) {
            char v[MAX_CURSO];
            ler_texto("Novo curso: ", v, sizeof(v), 1);
            utilizador_set_curso(u, v);
            printf("[OK] Curso atualizado.\n");
        } else if (strcmp(opcao, "3") == 0) {
            int ano = ler_inteiro("Novo ano academico (1-10): ", 1, 10);
            utilizador_set_ano_academico(u, ano);
            printf("[OK] Ano academico atualizado.\n");
        } else if (strcmp(opcao, "4") == 0) {
            char v[MAX_CIDADE];
            ler_texto("Nova cidade: ", v, sizeof(v), 0);
            utilizador_set_cidade(u, v);
            printf("[OK] Cidade atualizada.\n");
        } else if (strcmp(opcao, "5") == 0) {
            char v[MAX_EMAIL];
            ler_texto("Novo email: ", v, sizeof(v), 0);
            utilizador_set_email(u, v);
            printf("[OK] Email atualizado.\n");
        } else if (strcmp(opcao, "6") == 0) {
            char v[MAX_BIO];
            ler_texto("Nova bio: ", v, sizeof(v), 0);
            utilizador_set_bio(u, v);
            printf("[OK] Bio atualizada.\n");
        } else if (strcmp(opcao, "7") == 0) {
            char v[MAX_INTERESSE];
            ler_texto("Novo interesse: ", v, sizeof(v), 1);
            int r = utilizador_adicionar_interesse(u, v);
            if (r == OK) printf("[OK] Interesse adicionado.\n");
            else if (r == ERRO_JA_EXISTE) printf("[!] Ja tens esse interesse.\n");
            else printf("[!] Nao foi possivel adicionar (erro %d).\n", r);
        } else if (strcmp(opcao, "8") == 0) {
            char v[MAX_INTERESSE];
            ler_texto("Interesse a remover: ", v, sizeof(v), 1);
            int r = utilizador_remover_interesse(u, v);
            if (r == OK) printf("[OK] Interesse removido.\n");
            else printf("[!] Interesse nao encontrado.\n");
        } else if (strcmp(opcao, "9") == 0) {
            char antiga[MAX_PASSWORD], nova[MAX_PASSWORD];
            ler_texto("Password atual: ", antiga, sizeof(antiga), 1);
            ler_texto("Nova password: ", nova, sizeof(nova), 1);
            int r = utilizador_alterar_password(u, antiga, nova);
            if (r == OK) printf("[OK] Password alterada.\n");
            else printf("[!] Password atual incorreta.\n");
        } else if (strcmp(opcao, "0") != 0) {
            printf("[!] Opcao invalida.\n");
        }

        if (strcmp(opcao, "0") != 0) pausar();
    } while (strcmp(opcao, "0") != 0);
}

/* ============================================================
 * Pesquisa
 * ============================================================ */

static void ecra_procurar(RedeSocial *rede) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    char opcao[4];

    LIMPAR_ECRA();
    printf("=== PROCURAR UTILIZADORES ===\n\n");
    printf("1 - Por nome\n2 - Por universidade\n3 - Por curso\n");
    printf("4 - Por interesse\n5 - Por cidade\n0 - Voltar\n");
    ler_texto("Opcao: ", opcao, sizeof(opcao), 1);
    if (strcmp(opcao, "0") == 0) {
        return;
    }

    char termo[MAX_NOME];
    Utilizador **resultados = NULL;
    int num = 0;

    if (strcmp(opcao, "1") == 0) {
        ler_texto("Nome (ou parte do nome): ", termo, sizeof(termo), 1);
        resultados = pesquisar_por_nome(tab, termo, &num);
    } else if (strcmp(opcao, "2") == 0) {
        ler_texto("Universidade: ", termo, sizeof(termo), 1);
        resultados = pesquisar_por_universidade(tab, termo, &num);
    } else if (strcmp(opcao, "3") == 0) {
        ler_texto("Curso: ", termo, sizeof(termo), 1);
        resultados = pesquisar_por_curso(tab, termo, &num);
    } else if (strcmp(opcao, "4") == 0) {
        ler_texto("Interesse: ", termo, sizeof(termo), 1);
        resultados = pesquisar_por_interesse(tab, termo, &num);
    } else if (strcmp(opcao, "5") == 0) {
        ler_texto("Cidade: ", termo, sizeof(termo), 1);
        resultados = pesquisar_por_cidade(tab, termo, &num);
    } else {
        printf("[!] Opcao invalida.\n");
        pausar();
        return;
    }

    printf("\n--- %d resultado(s) ---\n", num);
    for (int i = 0; i < num; i++) {
        imprimir_cartao_utilizador(resultados[i]);
    }
    free(resultados);
    pausar();
}

/* ============================================================
 * Pedidos de amizade
 * ============================================================ */

static void ecra_enviar_pedido(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== ENVIAR PEDIDO DE AMIZADE ===\n\n");
    int destino = ler_inteiro("ID do utilizador: ", 1, 999999);

    if (destino == id_sessao) {
        printf("\n[!] Nao podes enviar um pedido a ti mesmo.\n");
        pausar();
        return;
    }

    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    if (hash_procurar_id(tab, destino) == NULL) {
        printf("\n[!] Nao existe nenhum utilizador com esse ID.\n");
        pausar();
        return;
    }

    GestorPedidos *gp = rede_social_get_pedidos(rede);
    int r = pedidos_enviar(gp, id_sessao, destino);
    if (r == OK) printf("\n[OK] Pedido enviado.\n");
    else if (r == ERRO_JA_EXISTE) printf("\n[!] Ja existe um pedido pendente entre voces.\n");
    else printf("\n[!] Nao foi possivel enviar o pedido (erro %d).\n", r);
    pausar();
}

static void ecra_pedidos_recebidos(RedeSocial *rede, int id_sessao) {
    GestorPedidos *gp = rede_social_get_pedidos(rede);
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    int num = 0;
    PedidoAmizade *lista = pedidos_listar_recebidos(gp, id_sessao, &num);

    LIMPAR_ECRA();
    printf("=== PEDIDOS DE AMIZADE RECEBIDOS ===\n\n");
    if (num == 0) {
        printf("Nao tens pedidos pendentes.\n");
        free(lista);
        pausar();
        return;
    }
    for (int i = 0; i < num; i++) {
        Utilizador *rem = hash_procurar_id(tab, lista[i].id_remetente);
        printf("  [%d] @%s\n", lista[i].id_remetente,
               rem != NULL ? utilizador_get_username(rem) : "?");
    }

    int escolha = ler_inteiro("\nID de quem queres responder (0 para voltar): ", 0, 999999);
    if (escolha != 0) {
        int valido = 0;
        for (int i = 0; i < num; i++) {
            if (lista[i].id_remetente == escolha) { valido = 1; break; }
        }
        if (!valido) {
            printf("[!] Esse ID nao esta na lista de pedidos recebidos.\n");
        } else {
            char resp[8];
            ler_texto("Aceitar? (s/n): ", resp, sizeof(resp), 1);
            if (resp[0] == 's' || resp[0] == 'S') {
                /* Ver nota (1) no topo do ficheiro sobre este cast. */
                GrafoAmizades *g = (GrafoAmizades *) rede_social_get_amizades(rede);
                int r = pedidos_aceitar(gp, escolha, id_sessao, g);
                if (r == OK) printf("[OK] Pedido aceite. Agora sao amigos.\n");
                else printf("[!] Nao foi possivel aceitar (erro %d).\n", r);
            } else {
                int r = pedidos_recusar(gp, escolha, id_sessao);
                if (r == OK) printf("[OK] Pedido recusado.\n");
                else printf("[!] Nao foi possivel recusar (erro %d).\n", r);
            }
        }
    }
    free(lista);
    pausar();
}

static void ecra_pedidos_enviados(RedeSocial *rede, int id_sessao) {
    GestorPedidos *gp = rede_social_get_pedidos(rede);
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    int num = 0;
    PedidoAmizade *lista = pedidos_listar_enviados(gp, id_sessao, &num);

    LIMPAR_ECRA();
    printf("=== PEDIDOS DE AMIZADE ENVIADOS ===\n\n");
    if (num == 0) {
        printf("Nao tens pedidos pendentes enviados.\n");
        free(lista);
        pausar();
        return;
    }
    for (int i = 0; i < num; i++) {
        Utilizador *dest = hash_procurar_id(tab, lista[i].id_destinatario);
        printf("  [%d] @%s\n", lista[i].id_destinatario,
               dest != NULL ? utilizador_get_username(dest) : "?");
    }

    int escolha = ler_inteiro("\nID do pedido a cancelar (0 para voltar): ", 0, 999999);
    if (escolha != 0) {
        int r = pedidos_cancelar(gp, id_sessao, escolha);
        if (r == OK) printf("[OK] Pedido cancelado.\n");
        else printf("[!] Nao foi possivel cancelar (erro %d).\n", r);
    }
    free(lista);
    pausar();
}

/* ============================================================
 * Recomendacao / afinidade
 * ============================================================ */

static void ecra_sugestoes(RedeSocial *rede, int id_sessao) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    const GrafoAmizades *g = rede_social_get_amizades(rede);
    int num = 0;
    ResultadoRecomendacao *sugestoes = recomendar_amigos(id_sessao, tab, g, &num);

    LIMPAR_ECRA();
    printf("=== SUGESTOES DE AMIZADE ===\n\n");
    if (num == 0) {
        printf("Sem sugestoes por agora.\n");
    }
    for (int i = 0; i < num; i++) {
        Utilizador *u = hash_procurar_id(tab, sugestoes[i].id_utilizador);
        printf("  [%d] @%-15s pontuacao: %d\n", sugestoes[i].id_utilizador,
               u != NULL ? utilizador_get_username(u) : "?", sugestoes[i].pontuacao);
    }
    free(sugestoes);
    pausar();
}

static void ecra_top5(RedeSocial *rede, int id_sessao) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    const GrafoAmizades *g = rede_social_get_amizades(rede);
    ResultadoRecomendacao *top5 = top5_compatibilidade(id_sessao, tab, g);

    LIMPAR_ECRA();
    printf("=== TOP 5 COMPATIBILIDADE ===\n\n");
    if (top5 == NULL) {
        printf("Nao foi possivel calcular (erro interno).\n");
        pausar();
        return;
    }

    int alguma = 0;
    for (int i = 0; i < 5; i++) {
        if (top5[i].id_utilizador < 0) {
            continue; /* slot sentinela, ver nota (3) no topo do ficheiro */
        }
        alguma = 1;
        Utilizador *u = hash_procurar_id(tab, top5[i].id_utilizador);
        printf("  %d. [%d] @%-15s pontuacao: %d\n", i + 1, top5[i].id_utilizador,
               u != NULL ? utilizador_get_username(u) : "?", top5[i].pontuacao);
    }
    if (!alguma) {
        printf("Ainda nao ha utilizadores suficientes na rede.\n");
    }
    free(top5);
    pausar();
}

/* ============================================================
 * Amizades
 * ============================================================ */

static void ecra_meus_amigos(RedeSocial *rede, int id_sessao) {
    const GrafoAmizades *g = rede_social_get_amizades(rede);
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    int num = 0;
    int *amigos = grafo_listar_amigos(g, id_sessao, &num);

    LIMPAR_ECRA();
    printf("=== OS MEUS AMIGOS (%d) ===\n\n", num);
    for (int i = 0; i < num; i++) {
        Utilizador *u = hash_procurar_id(tab, amigos[i]);
        imprimir_cartao_utilizador(u);
    }
    if (num == 0) {
        printf("Ainda nao tens amigos. Usa a opcao de pesquisa ou de\n");
        printf("sugestoes para encontrar pessoas para conectares.\n");
    }
    free(amigos);
    pausar();
}

static void ecra_remover_amigo(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== REMOVER AMIGO ===\n\n");
    int alvo = ler_inteiro("ID do amigo a remover: ", 1, 999999);

    /* Ver nota (1) no topo do ficheiro sobre este cast. */
    GrafoAmizades *g = (GrafoAmizades *) rede_social_get_amizades(rede);

    if (!grafo_existe_ligacao(g, id_sessao, alvo)) {
        printf("\n[!] Nao sao amigos.\n");
    } else {
        int r = grafo_remover_aresta(g, id_sessao, alvo);
        if (r == OK) printf("\n[OK] Amizade removida.\n");
        else printf("\n[!] Nao foi possivel remover (erro %d).\n", r);
    }
    pausar();
}

static void ecra_amigos_comuns(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== AMIGOS EM COMUM ===\n\n");
    int alvo = ler_inteiro("Comparar com o ID: ", 1, 999999);

    const GrafoAmizades *g = rede_social_get_amizades(rede);
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    int num = 0;
    int *comuns = grafo_amigos_comuns(g, id_sessao, alvo, &num);

    printf("\n%d amigo(s) em comum:\n", num);
    for (int i = 0; i < num; i++) {
        Utilizador *u = hash_procurar_id(tab, comuns[i]);
        imprimir_cartao_utilizador(u);
    }
    free(comuns);
    pausar();
}

/* ============================================================
 * Mensagens
 * ============================================================ */

static void ecra_enviar_mensagem(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== ENVIAR MENSAGEM ===\n\n");
    int destino = ler_inteiro("ID do destinatario: ", 1, 999999);

    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    if (hash_procurar_id(tab, destino) == NULL) {
        printf("\n[!] Utilizador nao encontrado.\n");
        pausar();
        return;
    }

    char conteudo[MAX_CONTEUDO];
    ler_texto("Mensagem: ", conteudo, sizeof(conteudo), 1);

    ListaMensagens *lm = rede_social_get_mensagens(rede);
    int r = mensagens_enviar(lm, id_sessao, destino, conteudo);
    if (r == OK) printf("\n[OK] Mensagem enviada.\n");
    else printf("\n[!] Nao foi possivel enviar (erro %d).\n", r);
    pausar();
}

static void ecra_ver_conversa(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== VER CONVERSA ===\n\n");
    int alvo = ler_inteiro("ID da outra pessoa: ", 1, 999999);

    ListaMensagens *lm = rede_social_get_mensagens(rede);
    int num = 0;
    MensagemInfo *msgs = mensagens_obter_conversa(lm, id_sessao, alvo, &num);

    printf("\n--- %d mensagem(ns) ---\n\n", num);
    for (int i = 0; i < num; i++) {
        const char *seta = (msgs[i].id_remetente == id_sessao) ? "->" : "<-";
        printf("[%02d/%02d/%04d %02d:%02d] %s %s\n",
               msgs[i].data_hora.dia, msgs[i].data_hora.mes, msgs[i].data_hora.ano,
               msgs[i].data_hora.hora, msgs[i].data_hora.minuto, seta, msgs[i].conteudo);
    }
    free(msgs);
    pausar();
}

/* ============================================================
 * Feed / publicacoes
 * ============================================================ */

static void ecra_feed(RedeSocial *rede, int id_sessao) {
    ListaPublicacoes *lp = rede_social_get_publicacoes(rede);
    const GrafoAmizades *g = rede_social_get_amizades(rede);
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    int num = 0;
    PublicacaoInfo *feed = publicacoes_feed_amigos(lp, g, id_sessao, &num);

    LIMPAR_ECRA();
    printf("=== FEED DE AMIGOS (%d publicacoes) ===\n\n", num);
    for (int i = 0; i < num; i++) {
        Utilizador *autor = hash_procurar_id(tab, feed[i].id_autor);
        printf("[Post #%d] @%s (%02d/%02d %02d:%02d)\n", feed[i].id_post,
               autor != NULL ? utilizador_get_username(autor) : "?",
               feed[i].data_hora.dia, feed[i].data_hora.mes,
               feed[i].data_hora.hora, feed[i].data_hora.minuto);
        printf("  %s\n", feed[i].conteudo);
        printf("  %d like(s) | %d comentario(s)\n\n", feed[i].num_likes, feed[i].num_comentarios);
    }
    if (num == 0) {
        printf("Sem publicacoes por agora. Adiciona amigos para veres o feed deles!\n");
    }
    free(feed);
    pausar();
}

static void ecra_criar_post(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== CRIAR PUBLICACAO ===\n\n");
    char conteudo[MAX_CONTEUDO];
    ler_texto("O que estas a pensar? ", conteudo, sizeof(conteudo), 1);

    ListaPublicacoes *lp = rede_social_get_publicacoes(rede);
    int r = publicacoes_criar_post(lp, id_sessao, conteudo);
    if (r >= 0) printf("\n[OK] Publicado (post #%d).\n", r);
    else printf("\n[!] Nao foi possivel publicar (erro %d).\n", r);
    pausar();
}

static void ecra_comentar_curtir(RedeSocial *rede, int id_sessao) {
    ListaPublicacoes *lp = rede_social_get_publicacoes(rede);

    LIMPAR_ECRA();
    printf("=== COMENTAR / CURTIR PUBLICACAO ===\n\n");
    int id_post = ler_inteiro("ID do post: ", 0, 999999);

    char opcao[4];
    printf("\n1 - Gostar\n2 - Remover gosto\n3 - Comentar\n4 - Ver comentarios\n0 - Voltar\n");
    ler_texto("Opcao: ", opcao, sizeof(opcao), 1);

    if (strcmp(opcao, "1") == 0) {
        int r = publicacoes_curtir(lp, id_post, id_sessao);
        if (r == OK) printf("\n[OK] Gostaste da publicacao.\n");
        else printf("\n[!] Nao foi possivel (erro %d).\n", r);
    } else if (strcmp(opcao, "2") == 0) {
        int r = publicacoes_descurtir(lp, id_post, id_sessao);
        if (r == OK) printf("\n[OK] Gosto removido.\n");
        else printf("\n[!] Nao foi possivel (erro %d).\n", r);
    } else if (strcmp(opcao, "3") == 0) {
        char conteudo[MAX_CONTEUDO];
        ler_texto("Comentario: ", conteudo, sizeof(conteudo), 1);
        int r = publicacoes_comentar(lp, id_post, id_sessao, conteudo);
        if (r == OK) printf("\n[OK] Comentario adicionado.\n");
        else printf("\n[!] Nao foi possivel (erro %d).\n", r);
    } else if (strcmp(opcao, "4") == 0) {
        const TabelaHash *tab = rede_social_get_utilizadores(rede);
        int num = 0;
        ComentarioInfo *coms = publicacoes_listar_comentarios(lp, id_post, &num);
        printf("\n--- %d comentario(s) ---\n", num);
        for (int i = 0; i < num; i++) {
            Utilizador *autor = hash_procurar_id(tab, coms[i].id_autor);
            printf("  @%s: %s\n", autor != NULL ? utilizador_get_username(autor) : "?",
                   coms[i].conteudo);
        }
        free(coms);
    }
    pausar();
}

/* ============================================================
 * Consulta geral / estatisticas
 * ============================================================ */

static void ecra_ver_todos(RedeSocial *rede) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    int num = 0;
    Utilizador **todos = hash_listar_todos(tab, &num);

    LIMPAR_ECRA();
    printf("=== TODOS OS UTILIZADORES (%d) ===\n\n", num);
    for (int i = 0; i < num; i++) {
        imprimir_cartao_utilizador(todos[i]);
    }
    free(todos);
    pausar();
}

static void ecra_estatisticas(RedeSocial *rede) {
    const TabelaHash *tab = rede_social_get_utilizadores(rede);
    const GrafoAmizades *g = rede_social_get_amizades(rede);
    Estatisticas stats = calcular_estatisticas(tab, g);

    LIMPAR_ECRA();
    printf("=== ESTATISTICAS DA REDE ===\n\n");
    estatisticas_imprimir(&stats);
    pausar();
}

/* ============================================================
 * Eliminar conta
 * ============================================================ */

/* Devolve 1 se a conta foi mesmo eliminada (o chamador deve encerrar o
 * menu principal, pois a sessao deixou de existir), 0 caso contrario. */
static int ecra_eliminar_conta(RedeSocial *rede, int id_sessao) {
    LIMPAR_ECRA();
    printf("=== ELIMINAR CONTA ===\n\n");
    printf("[!] Esta acao e irreversivel. O teu perfil, amizades e\n");
    printf("    pedidos associados serao apagados permanentemente.\n\n");

    char confirmacao[16];
    ler_texto("Escreve CONFIRMAR para continuar: ", confirmacao, sizeof(confirmacao), 1);
    if (strcmp(confirmacao, "CONFIRMAR") != 0) {
        printf("\nOperacao cancelada.\n");
        pausar();
        return 0;
    }

    int r = rede_social_eliminar_conta(rede, id_sessao);
    if (r == OK) {
        printf("\n[OK] Conta eliminada. Ate a proxima!\n");
        pausar();
        return 1;
    }
    printf("\n[!] Nao foi possivel eliminar a conta (erro %d).\n", r);
    pausar();
    return 0;
}

/* ============================================================
 * main
 * ============================================================ */

int main(void) {
    RedeSocial *rede = rede_social_iniciar();
    if (rede == NULL) {
        fprintf(stderr, "Erro fatal: nao foi possivel iniciar a RedeSocial.\n");
        return 1;
    }

    char opcao[8];
    do {
        LIMPAR_ECRA();
        printf("+----------------------------------------+\n");
        printf("|               UniConnect                |\n");
        printf("+----------------------------------------+\n");
        printf("| 1 | Criar conta                         |\n");
        printf("| 2 | Iniciar sessao                      |\n");
        printf("| 0 | Sair                                |\n");
        printf("+----------------------------------------+\n\n");
        ler_texto("Opcao: ", opcao, sizeof(opcao), 1);

        if (strcmp(opcao, "1") == 0) {
            ecra_cadastro(rede);
        } else if (strcmp(opcao, "2") == 0) {
            ecra_login(rede);
        } else if (strcmp(opcao, "0") != 0) {
            printf("\n[!] Opcao invalida.\n");
            pausar();
        }
    } while (strcmp(opcao, "0") != 0);

    rede_social_terminar(rede);
    printf("\nDados guardados. Ate a proxima!\n");
    return 0;
}
