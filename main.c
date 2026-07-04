#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "rede_social.h"


static int gerarIdUnico(Grafo* rede) {
    int id;
    do { id = rand() % 9000 + 1000; }  
    while (buscarIndicePorId(rede, id) != -1);
    return id;
}


void menu_inicio_sessao() {
    system("cls");
    printf("\t\t\t+-----------------------------------+\n");
    printf("\t\t\t|             UniConnect            |\n");
    printf("\t\t\t+-----------------------------------+\n");
    printf("\t\t\t| 1 | Fazer Cadastro                |\n");
    printf("\t\t\t+-----------------------------------+\n");
    printf("\t\t\t| 2 | Fazer Login                   |\n");
    printf("\t\t\t+-----------------------------------+\n");
    printf("\t\t\t| 0 | Sair                          |\n");
    printf("\t\t\t+-----------------------------------+\n\n");
}


void cadastrar_usuario(Grafo* rede) {
    char nome[50], universidade[50], senha[50], curso[50];
    int qtd_interesses;

    system("cls");
    printf("\t\t\t+----------------------------------------+\n");
    printf("\t\t\t|              CRIAR CONTA               |\n");
    printf("\t\t\t+----------------------------------------+\n\n");

    printf("\t\t\t[?] Nome de usuario: ");
    scanf(" %49[^\n]", nome);

    printf("\n\t\t\t[?] Senha: ");
    scanf(" %49[^\n]", senha);

    printf("\n\t\t\t[?] Universidade: ");
    scanf(" %49[^\n]", universidade);

    printf("\n\t\t\t[?] Curso: ");
    scanf(" %49[^\n]", curso);

    printf("\n\t\t\t[?] Quantos interesses? (Max 5): ");
    scanf(" %d", &qtd_interesses);
    if (qtd_interesses > 5) qtd_interesses = 5;
    if (qtd_interesses < 1) qtd_interesses = 1;

    char interesses[5][20];
    for (int i = 0; i < qtd_interesses; i++) {
        printf("\t\t\t[?] Interesse %d: ", i + 1);
        scanf(" %19[^\n]", interesses[i]);
    }

    int id_usuario = gerarIdUnico(rede);
    adicionarPessoa(rede, id_usuario, nome, universidade, curso, senha,
                    qtd_interesses, interesses);

    system("cls");
    printf("\n\n\t\t\t+----------------------------------------+\n");
    printf("\t\t\t|  [!] Conta criada! O teu ID: %-9d|\n", id_usuario);
    printf("\t\t\t+----------------------------------------+\n\n");
    printf("Pressione ENTER para continuar...");
    getchar(); getchar();
}

void menu_logado(Grafo* rede, Sessao* sessao) {
    char opcao[10];

    do {
        system("cls");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t|                                         U n i C o n n e c t                                               |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| Seja Bem Vindo, @%-89s|\n", sessao->nome);
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 1 | Ver Perfil                                   | 8 | Remover Amigo                                      |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 2 | Procurar Usuario                             | 9 | Seguir Usuario                                     |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 3 | Editar Perfil                                | A | Amigos em Comum                                    |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 4 | Enviar Pedido de Amizade                     | B | Enviar Mensagem                                    |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 5 | Ver Pedidos de Amizade                       | C | Ver Todos Usuarios                                 |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 6 | Sugestao de Amigos                           |                                                        |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 7 | Meus Amigos                                  |                                                        |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 0 | Terminar sessao                                                                                       |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n\n");
        printf("\t\t\tOpcao: ");
        scanf(" %9s", opcao);

       
        for (int i = 0; opcao[i]; i++)
            if (opcao[i] >= 'A' && opcao[i] <= 'Z') opcao[i] += 32;

        if (strcmp(opcao, "4") == 0) {
           
            int idDest;
            system("cls");
            printf("\t[?] ID do utilizador a quem enviar pedido: ");
            scanf("%d", &idDest);
            enviarPedidoAmizade(rede, sessao->id, idDest);
            printf("\nPressione ENTER para continuar...");
            getchar(); getchar();

        } else if (strcmp(opcao, "5") == 0) {
           
            system("cls");
            imprimirPedidosPendentes(rede, sessao->id);

          
            int idRem;
            printf("\n\t[?] ID de quem quer responder (0 para voltar): ");
            scanf("%d", &idRem);
            if (idRem != 0) {
                char resp[4];
                printf("\t[?] Aceitar? (s/n): ");
                scanf(" %3s", resp);
                int aceitar = (resp[0] == 's' || resp[0] == 'S');
                responderPedidoAmizade(rede, sessao->id, idRem, aceitar);
            }
            printf("\nPressione ENTER para continuar...");
            getchar(); getchar();

        } else if (strcmp(opcao, "c") == 0) {
            // --- Ver todos os utilizadores ---
            system("cls");
            imprimirRede(rede);
            printf("\n\nPressione ENTER para continuar...");
            getchar(); getchar();
        }

    } while (strcmp(opcao, "0") != 0);

   
    sessao->id = -1;
    sessao->nome[0] = '\0';
}


void login_usuario(Grafo* rede) {
    char nome[50], senha[50];
    Sessao sessao;

    system("cls");
    printf("\t\t\t+----------------------------------------+\n");
    printf("\t\t\t|              LOGIN                     |\n");
    printf("\t\t\t+----------------------------------------+\n\n");

    printf("\t\t\t[?] Username: ");
    scanf(" %49[^\n]", nome);

    printf("\t\t\t[?] Senha: ");
    scanf(" %49[^\n]", senha);

    if (fazerLogin(rede, nome, senha, &sessao)) {
        
        menu_logado(rede, &sessao);
    } else {
        system("cls");
        printf("\n\t\t\t+----------------------------------------+\n");
        printf("\t\t\t|  [!] Nome ou senha incorrectos.        |\n");
        printf("\t\t\t+----------------------------------------+\n\n");
        printf("Pressione ENTER para continuar...");
        getchar(); getchar();
    }
}


int main() {
    int opcao;
    srand(time(NULL));
    Grafo* rede = criarGrafo(5);

    do {
        menu_inicio_sessao();
        printf("\t\t\t");
        scanf("%d", &opcao);

        if      (opcao == 1) cadastrar_usuario(rede);
        else if (opcao == 2) login_usuario(rede);

    } while (opcao != 0);

    liberarGrafo(rede);
    return 0;
}
