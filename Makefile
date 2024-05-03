# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sprodatu <sprodatu@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/02 00:27:53 by sprodatu          #+#    #+#              #
#    Updated: 2024/05/02 23:09:07 by sprodatu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -g

# Library Name
NAME = pipex

# Bonus library name
BONUS_NAME = pipex_bonus

# Source files
SRCS = pipex.c utils.c ft_split.c ft_strjoin.c ft_strncmp.c ft_strdup.c ft_memcpy.c

# Bonus source files
BONUS_SRCS = pipex_bonus.c utils_bonus.c

# Object files: replace .c with .o
# main object files
OBJS = $(SRCS:.c=.o)

# Bonus object files
BONUS_OBJS = $(BONUS_SRCS:.c=.o)

# Complete list of Rules
# Rule to make the library
all: $(NAME)

# Rule to compile the library
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[0;32m pipex compiled succesfully\033[0m"

# # Rule to compile the bonus
# bonus:$(NAME) $(BONUS_OBJS)
# 	$(CC) $(CFLAGS) $(BONUS_OBJS) -o $(BONUS_NAME)
# 	@echo "\033[0;32m pipex bonus compiled succesfully\033[0m"

# Rule to clean the object files
clean:
	rm -f $(OBJS) $(BONUS_OBJS)

# Rule to clean everything
fclean: clean
	# find . -type f ! -name '*.c' ! -name '*.h' ! -name 'Makefile' -delete
	rm -f $(NAME) $(BONUS_NAME)

# Rule to recompile
re: fclean all

# Phony targets
.PHONY: all clean fclean re bonus