# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/19 17:10:39 by nide-wil          #+#    #+#              #
#    Updated: 2026/10/19 17:14:59 by nide-wil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = push_swap

CC          = cc
CFLAGS      = -Wall -Wextra -Werror

# Configuration de la Libft
LIBFT_DIR   = libft
LIBFT       = $(LIBFT_DIR)/libft.a
LIBFT_INC   = -I$(LIBFT_DIR)
LIBFT_LIB   = -L$(LIBFT_DIR) -lft

# Fichiers sources et objets
SRC         = main.c ops.c parsing.c sort.c stack.c utils.c
OBJ         = $(SRC:.c=.o)

# Dossiers d'inclusion des headers
INC         = -I. $(LIBFT_INC)

all: $(LIBFT) $(NAME)

# Compilation de la libft
$(LIBFT):
	@make -C $(LIBFT_DIR)

# Compilation de l'exécutable push_swap
$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT_LIB) -o $(NAME)

# Règle générique pour les fichiers .o
%.o: %.c push_swap.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)
	@make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	@make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re
