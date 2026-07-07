# -----------------------------------------------------------------------
# Makefile - UniConnect
# -----------------------------------------------------------------------
# Estrutura assumida:
#   estrutura/   TAD.h, utils.h, utils.c   (tipos e utilitarios base)
#   include/     todos os outros .h
#   src/         todos os outros .c
#   main.c       ponto de entrada (raiz do projeto)
#
# NOTA: nao tenho a certeza se "estrutura/" e mesmo o nome de pasta usado
# no teu projeto original - o documento fornecido so dizia
# "./estrutura/* (3 files)" sem indicar o caminho completo. Se o teu
# projeto real usar outro nome de pasta, ajusta as variaveis abaixo.
# -----------------------------------------------------------------------

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -g -Iestrutura -Iinclude
LDFLAGS :=

SRC_ESTRUTURA := $(wildcard estrutura/*.c)
SRC_SRC       := $(wildcard src/*.c)
SRC_MAIN      := main.c

SRCS := $(SRC_MAIN) $(SRC_ESTRUTURA) $(SRC_SRC)
OBJS := $(SRCS:.c=.o)

TARGET := uniconnect

.PHONY: all clean run debug limpar-dados

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

# Compila com sanitizers para apanhar bugs de memoria (usar em
# desenvolvimento, nunca para entregar/distribuir).
debug: CFLAGS += -fsanitize=address,undefined -O0
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean all

# Apaga apenas os binarios/objetos, NAO apaga os dados persistidos.
clean:
	rm -f $(OBJS) $(TARGET)

# Apaga os ficheiros .dat gerados pela persistencia (cuidado: perdes
# todos os utilizadores/amizades/mensagens/posts guardados).
limpar-dados:
	rm -f usuarios.dat amizades.dat pedidos.dat mensagens.dat posts.dat
