#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "rede_social.h"

void menu_inicio_sessao()
{
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

void cadastrar_usuario(Grafo* rede)
{
    //Variaveis de TESTE
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

    printf("\n\t\t\t[?] Quantos interesses? (Max: 5): ");
    scanf(" %d", &qtd_interesses);

    if (qtd_interesses > 5) qtd_interesses = 5;
    if (qtd_interesses < 1) qtd_interesses = 1;

    char interesses[5][20];
    for (int i = 0; i < qtd_interesses; i++) {
        printf("\t\t\t[?] Interesse %d: ", i + 1);
        scanf(" %19[^\n]", interesses[i]);
    }

    //Insercao do usuario no grafo

    //Gerar o ID
    int id_usuario = rand()%50;
    int indice_do_novo_user = adicionarPessoa(rede,id_usuario,nome,universidade,curso,senha,qtd_interesses,interesses);

    system("cls");
    printf("ID gerado %d",id_usuario);
    printf("ID do vertice do user-> %d", indice_do_novo_user);
    printf("\n\n\t\t\t+----------------------------------------+\n");
    printf("\t\t\t|  [!] Usuario Cadastrado                |\n");
    printf("\t\t\t+----------------------------------------+\n\n\n\n");

    printf("Pressione qualquer tecla para continuar... ");
    getchar();
    getchar();


}

void imprimirMenu(char *username, Grafo* rede) {
    char opcao[10];
    system("cls");
    do{
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t|                                         U n i C o n n e c t                                               |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| Seja Bem Vindo, @%-89s|\n", username);
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 1 | Ver Perfil                                   | 8 | Remover Amigo                                      |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 2 | Procurar Usuario                             | 9 | Seguir Usuario                                     |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 3 | Editar Perfil                                | A |  Amigos em Comum                                   |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 4 | Enviar Pedido de Amizade                     | B |  Enviar Mensagem                                   |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 5 | Ver Pedidos de Amizade                       | C |  Ver Todos Usuarios                                |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 6 | Sugestao de Amigos                           |                                                        |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 7 | Meus Amigos                                  |                                                        |\n");
        printf("\t+--------------------------------------------------+--------------------------------------------------------+\n");
        printf("\t| 0 | Terminar sessao                                                                                       |\n");
        printf("\t+---------a-----------------------------------------+--------------------------------------------------------+\n\n");
        printf("\t\t");
        gets(opcao);
        if(strcmp(strlwr(opcao),"c") == 0 )
        {
                system("cls");
                imprimirRede(rede);
                printf("\n\n");
        }
    }while(strcmp(opcao,"0")!=0);
}


void login_usuario(Grafo* rede)
{
    // VARIAVEIS PARA o MOCK
    char nome[50], senha[50];
    system("cls");
    printf("\t\t\t+----------------------------------------+\n");
    printf("\t\t\t|              LOGIN                     |\n");
    printf("\t\t\t+----------------------------------------+\n\n");

    printf("\t\t\t[?] USERNAME: ");
    scanf(" %49[^\n]", nome);

    printf("\t\t\t[?] SENHA: ");
    scanf(" %49[^\n]", senha);

    imprimirMenu(nome, rede);
}


int main()
{
    int opcao;
    srand(time(NULL));
    Grafo* rede = criarGrafo(5);
    do{
         menu_inicio_sessao();
         printf("\t\t\t");
         scanf("%d", &opcao);

         if(opcao == 1)
             cadastrar_usuario(rede);
         else if(opcao == 2)
        {
            login_usuario(rede);
         }

    }while(opcao != 0);

    return 0;
}
