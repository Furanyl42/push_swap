# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nide-wil <nide-wil@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/14 12:39:34 by nide-wil          #+#    #+#              #
#    Updated: 2026/11/05 12:00:00 by nide-wil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= libftprintf.a

LIBFT_DIR	= ../libft

SRCS		= ft_printf.c \
			  print_percent.c \
			  print_char.c \
			  print_str.c \
			  print_nbr.c \
			  print_unsigned.c \
			  print_hex.c \
			  hex_utils.c \
			  print_ptr.c \
			  parse.c \
			  utils.c \
			  ptr_utils.c \
			  unsigned_utils.c

OBJS		= $(SRCS:.c=.o)

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f
AR			= ar rcs

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

%.o: %.c ft_printf.h
	$(CC) $(CFLAGS) -I$(LIBFT_DIR) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
