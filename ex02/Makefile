
NAME		= PmergeMe

SRCS_DIR	= sources
INCL_DIR	= include

SRCS		= $(SRCS_DIR)/main.cpp \
		  $(SRCS_DIR)/PmergeMe.cpp

OBJS		= $(SRCS:.cpp=.o)

CXX		= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -I$(INCL_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

