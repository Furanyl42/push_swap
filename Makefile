# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adesmet <adesmet@student.42belgium.be>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/19 17:10:39 by nide-wil          #+#    #+#              #
#    Updated: 2026/11/05 12:00:00 by adesmet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= push_swap

CC			= cc
CFLAGS		= -MMD -MP -Wall -Wextra -Werror

PRINTF_DIR	= ft_printf
PRINTF		= $(PRINTF_DIR)/libftprintf.a
PRINTF_SRC	= $(wildcard $(PRINTF_DIR)/*.c) \
			  $(PRINTF_DIR)/ft_printf.h \
			  $(PRINTF_DIR)/Makefile
PRINTF_INC	= -I$(PRINTF_DIR)

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
LIBFT_SRC	= $(wildcard $(LIBFT_DIR)/*.c) \
			  $(LIBFT_DIR)/libft.h \
			  $(LIBFT_DIR)/Makefile
LIBFT_INC	= -I$(LIBFT_DIR)

SRC			= main.c \
			  bench.c \
			  bench_utils.c \
			  ops/ops_push.c \
			  ops/ops_reverse.c \
			  ops/ops_rotate.c \
			  ops/ops_swap.c \
			  parsing/parsing.c \
			  parsing/stack.c \
			  parsing/numbers.c \
			  parsing/flags.c \
			  parsing/count.c \
			  simple/simple.c \
			  medium/medium.c \
			  medium/m_utils.c \
			  complex/cost_target_utils.c \
			  complex/presort.c \
			  complex/pull1.c \
			  complex/pull2.c \
			  adaptative/disorder.c

BONUS_NAME	= checker
BONUS_SRC	= bonus/checker_bonus.c \
			  bonus/checker_parse_bonus.c \
			  bonus/checker_ops_bonus.c \
			  bonus/checker_exec_bonus.c
BONUS_DEPS	= $(OBJ_DIR)/parsing/numbers.o $(OBJ_DIR)/parsing/stack.o

TEST_SRC	= testing/test.c testing/test_utils.c
GEN_SRC		= testing/generator.c

OBJ_DIR		= obj
TEST_NAME	= test

OBJ			= $(SRC:%.c=$(OBJ_DIR)/%.o)
BONUS_OBJ	= $(BONUS_SRC:%.c=$(OBJ_DIR)/%.o)
TEST_OBJ	= $(TEST_SRC:%.c=$(OBJ_DIR)/%.o)
GEN_OBJ		= $(GEN_SRC:%.c=$(OBJ_DIR)/%.o)
COMMON_OBJ	= $(filter-out $(OBJ_DIR)/main.o,$(OBJ))
DEP			= $(OBJ:.o=.d) $(BONUS_OBJ:.o=.d) $(TEST_OBJ:.o=.d) \
			  $(GEN_OBJ:.o=.d)

INC			= -I. -Icomplex $(LIBFT_INC) $(PRINTF_INC)
HEADERS		= push_swap.h bench.h complex/complex.h testing/test.h
BONUS_HEADERS	= bonus/checker_bonus.h

all: $(NAME)

$(LIBFT): $(LIBFT_SRC)
	$(MAKE) -C $(LIBFT_DIR)

$(PRINTF): $(LIBFT) $(PRINTF_SRC)
	$(MAKE) -C $(PRINTF_DIR)

$(NAME): $(OBJ) $(PRINTF) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) $(PRINTF) $(LIBFT) -o $(NAME)

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJ) $(BONUS_DEPS) $(LIBFT)
	$(CC) $(CFLAGS) $(BONUS_OBJ) $(BONUS_DEPS) $(LIBFT) -o $(BONUS_NAME)

$(BONUS_OBJ): $(OBJ_DIR)/%.o: %.c $(BONUS_HEADERS) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

test: $(PRINTF) $(LIBFT) $(TEST_OBJ) $(GEN_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(GEN_OBJ) $(COMMON_OBJ) \
		$(PRINTF) $(LIBFT) -o $(TEST_NAME)
	@./$(TEST_NAME)

$(OBJ_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) clean -C $(PRINTF_DIR)
	$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(BONUS_NAME) $(TEST_NAME)
	$(MAKE) fclean -C $(PRINTF_DIR)
	$(MAKE) fclean -C $(LIBFT_DIR)

re: fclean all

-include $(DEP)

.PHONY: all bonus clean fclean re test
