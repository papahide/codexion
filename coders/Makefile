# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/17 21:27:53 by paapahid          #+#    #+#              #
#    Updated: 2026/08/26 20:53:20 by paapahid         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = 	codexion

CFLAGS = -Wall -Wextra -Werror -pthread

CC = cc

SRC = codexion.c \
	parse.c \
	validator.c \
	heap.c \
	initialization.c \
	logs.c \
	request_creation.c \
	routine_utils.c \
	routines.c \
	simulation.c \
	take_dongle.c \
	coder_routine_utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re