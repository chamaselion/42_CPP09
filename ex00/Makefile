NAME		=	btc
CC		=	c++
CFLAGS		=	-Wall -Wextra -Werror -std=c++98 -Iinclude
INCL_DIR	=	includes
SRC_DIR		=	sources
OBJ_DIR		=	objects

SRCS		=	$(wildcard $(SRC_DIR)/*.cpp)

OBJS		=	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all:			$(NAME)

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCL_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME):		$(OBJS)
	$(CC) $(CFLAGS) -I$(INCL_DIR) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean:			clean
	rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re