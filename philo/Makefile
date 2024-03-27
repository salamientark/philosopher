### COMPILATION ###
CC = cc
CFLAGS = -Wall -Wextra -Werror

### INCLUDES ###
PROJECT = philo
PROJECT_DIR = ./

SRC_DIR = src
SRC_FILE = utils.c \
			init_simulation.c \
			philo.c \
			main.c
			   
### HEADER FILE ###
HEADER_DIR = includes

## OBJECT FILE ###
OBJ_DIR = .obj
OBJ_SRC = $(addprefix $(OBJ_DIR)/, $(SRC_FILE:.c=.o))

.PHONY=bonus

### RULES ###
all : $(PROJECT)

$(PROJECT) : $(OBJ_SRC)
	$(CC) -g3 $(CFLAGS) $(OBJ_SRC) -o $(PROJECT) -lpthread

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -g3 $(CFLAGS) -I ./$(HEADER_DIR)  -c $< -o $@ -lpthread

fclean : clean
	rm -f $(PROJECT)

#Suppresion des fichiers objet
clean :
	rm -f $(OBJ_DIR)/*.o
	@rm -df $(OBJ_DIR)/

re : fclean all

norminette :
	norminette $(FT_DIR) $(SRC_DIR)/ $(HEADER_DIR)/