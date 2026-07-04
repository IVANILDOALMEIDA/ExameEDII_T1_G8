#include "utilizador.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



Utilizador *utilizador_criar(int id,
                              const char *nome_completo,
                              const char *username,
                              const char *password,
                              const char *universidade,
                              const char *curso,
                              int ano_academico,
                              int idade) {
    if (nome_completo == NULL || username == NULL || password == NULL ||
        universidade == NULL || curso == NULL) {
        return NULL;
    }

    Utilizador *u = malloc(sizeof(Utilizador));
    if (u == NULL) {
        return NULL;
    }

    u->id = id;
    strncpy(u->nome_completo, nome_completo, MAX_NOME - 1);
    u->nome_completo[MAX_NOME - 1] = '\0';
    strncpy(u->username, username, MAX_USERNAME - 1);
    u->username[MAX_USERNAME - 1] = '\0';
    strncpy(u->password, password, MAX_PASSWORD - 1);
    u->password[MAX_PASSWORD - 1] = '\0';
    strncpy(u->universidade, universidade, MAX_UNIVERSIDADE - 1);
    u->universidade[MAX_UNIVERSIDADE - 1] = '\0';
    strncpy(u->curso, curso, MAX_CURSO - 1);
    u->curso[MAX_CURSO - 1] = '\0';
    u->ano_academico = ano_academico;
    u->idade = idade;

    u->cidade[0] = '\0';
    u->email[0] = '\0';
    u->bio[0] = '\0';
    u->estado = ESTADO_OFFLINE;

    u->interesses = lista_interesses_criar();
    if (u->interesses == NULL) {
        free(u);
        return NULL;
    }

    return u;
}

void utilizador_destruir(Utilizador *u) {
    if (u == NULL) {
        return;
    }
    lista_interesses_destruir(u->interesses);
    free(u);
}

int utilizador_autenticar(const Utilizador *u, const char *password) {
    if (u == NULL || password == NULL) {
        return 0;
    }
    return strcmp(u->password, password) == 0;
}

int utilizador_alterar_password(Utilizador *u, const char *password_antiga, const char *password_nova) {
    if (u == NULL || password_antiga == NULL || password_nova == NULL || password_nova[0] == '\0') {
        return ERRO_PARAMETRO_INVALIDO;
    }
    if (!utilizador_autenticar(u, password_antiga)) {
        return ERRO_NAO_AUTORIZADO;
    }
    strncpy(u->password, password_nova, MAX_PASSWORD - 1);
    u->password[MAX_PASSWORD - 1] = '\0';
    return OK;
}



int utilizador_get_id(const Utilizador *u) {
    return (u != NULL) ? u->id : -1;
}

const char *utilizador_get_nome(const Utilizador *u) {
    return (u != NULL) ? u->nome_completo : NULL;
}

const char *utilizador_get_username(const Utilizador *u) {
    return (u != NULL) ? u->username : NULL;
}

const char *utilizador_get_universidade(const Utilizador *u) {
    return (u != NULL) ? u->universidade : NULL;
}

const char *utilizador_get_curso(const Utilizador *u) {
    return (u != NULL) ? u->curso : NULL;
}

int utilizador_get_ano_academico(const Utilizador *u) {
    return (u != NULL) ? u->ano_academico : 0;
}

int utilizador_get_idade(const Utilizador *u) {
    return (u != NULL) ? u->idade : 0;
}

const char *utilizador_get_cidade(const Utilizador *u) {
    return (u != NULL) ? u->cidade : NULL;
}

const char *utilizador_get_email(const Utilizador *u) {
    return (u != NULL) ? u->email : NULL;
}

const char *utilizador_get_bio(const Utilizador *u) {
    return (u != NULL) ? u->bio : NULL;
}

EstadoUtilizador utilizador_get_estado(const Utilizador *u) {
    return (u != NULL) ? u->estado : ESTADO_OFFLINE;
}

const ListaInteresses *utilizador_get_interesses(const Utilizador *u) {
    return (u != NULL) ? u->interesses : NULL;
}



int utilizador_set_nome(Utilizador *u, const char *novo_nome) {
    if (u == NULL || novo_nome == NULL || novo_nome[0] == '\0') {
        return ERRO_PARAMETRO_INVALIDO;
    }
    strncpy(u->nome_completo, novo_nome, MAX_NOME - 1);
    u->nome_completo[MAX_NOME - 1] = '\0';
    return OK;
}

int utilizador_set_curso(Utilizador *u, const char *novo_curso) {
    if (u == NULL || novo_curso == NULL || novo_curso[0] == '\0') {
        return ERRO_PARAMETRO_INVALIDO;
    }
    strncpy(u->curso, novo_curso, MAX_CURSO - 1);
    u->curso[MAX_CURSO - 1] = '\0';
    return OK;
}

int utilizador_set_ano_academico(Utilizador *u, int novo_ano) {
    if (u == NULL || novo_ano <= 0) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    u->ano_academico = novo_ano;
    return OK;
}

int utilizador_set_cidade(Utilizador *u, const char *nova_cidade) {
    if (u == NULL || nova_cidade == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    strncpy(u->cidade, nova_cidade, MAX_CIDADE - 1);
    u->cidade[MAX_CIDADE - 1] = '\0';
    return OK;
}

int utilizador_set_email(Utilizador *u, const char *novo_email) {
    if (u == NULL || novo_email == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    strncpy(u->email, novo_email, MAX_EMAIL - 1);
    u->email[MAX_EMAIL - 1] = '\0';
    return OK;
}

int utilizador_set_bio(Utilizador *u, const char *nova_bio) {
    if (u == NULL || nova_bio == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    strncpy(u->bio, nova_bio, MAX_BIO - 1);
    u->bio[MAX_BIO - 1] = '\0';
    return OK;
}

void utilizador_set_estado(Utilizador *u, EstadoUtilizador estado) {
    if (u == NULL) {
        return;
    }
    u->estado = estado;
}

int utilizador_adicionar_interesse(Utilizador *u, const char *interesse) {
    if (u == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    return lista_interesses_inserir(u->interesses, interesse);
}

int utilizador_remover_interesse(Utilizador *u, const char *interesse) {
    if (u == NULL) {
        return ERRO_PARAMETRO_INVALIDO;
    }
    return lista_interesses_remover(u->interesses, interesse);
}



void utilizador_imprimir_perfil(const Utilizador *u) {
    if (u == NULL) {
        printf("(utilizador inexistente)\n");
        return;
    }
    printf("----------------------------------------\n");
    printf("ID: %d\n", u->id);
    printf("Nome: %s\n", u->nome_completo);
    printf("Username: @%s\n", u->username);
    printf("Universidade: %s\n", u->universidade);
    printf("Curso: %s (%dº ano)\n", u->curso, u->ano_academico);
    //printf("Idade: %d\n", u->idade); /*Não será apresentado para outros usuários*/
    if (u->cidade[0] != '\0') {
        printf("Cidade: %s\n", u->cidade);
    }
    if (u->email[0] != '\0') {
        printf("Email: %s\n", u->email);
    }
    if (u->bio[0] != '\0') {
        printf("Bio: %s\n", u->bio);
    }
    printf("Estado: %s\n", u->estado == ESTADO_ONLINE ? "online" : "offline");
    printf("Interesses: ");
    lista_interesses_imprimir(u->interesses);
    printf("\n----------------------------------------\n");
}

int utilizador_comparar_por_nome(const Utilizador *a, const Utilizador *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    return comparar_strings_ci(a->nome_completo, b->nome_completo);
}