NAME = ft_malcolm

GREEN = \033[3;32m
RED = \033[3;31m
RESET = \033[0m

SRCS = main.c validate.c libft.c parse.c poison.c nic.c
		
SRCPATH = srcs/
OBJPATH = objects/
LIBS    = -I includes/ 

SOURCES = $(addprefix $(SRCPATH), $(SRCS))
OBJECTS = $(addprefix $(OBJPATH), $(SRCS:.c=.o))
DEPENDENCIES = $(addprefix $(OBJPATH), $(SRCS:.c=.d))

CC = cc

CFLAGS = -Wall -Werror -Wextra  -MMD -MP $(LIBS)


CREATETEXT = @echo "$(GREEN)Creating ft_malcolm...!$(RESET)"
REMOVETEXT = @echo "$(RED)CLEANING UP!$(RESET)"

all: $(OBJPATH) $(NAME)

-include $(DEPENDENCIES)

$(OBJPATH)%.o: $(SRCPATH)%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJPATH):
	@mkdir $(OBJPATH)
	
$(NAME):  $(OBJECTS) 
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)
	$(CREATETEXT)

clean:
	@rm -rf $(OBJPATH)

fclean: clean
	@rm -f $(NAME)
	$(REMOVETEXT)

re: fclean all

.PHONY: all clean fclean re
