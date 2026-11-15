NAME		= ft_shield
CC		= cc
CFLAGS		= -Wall -Wextra -Werror -O2
LDFLAGS		= -Wl,-x
PASSWORD	= 4242

SRC_DIR		= src
OBJ_DIR		= obj
INC_DIR		= inc
CRED		= $(SRC_DIR)/credentials.h
GEN		= tools/gen_hash

SRCS		= $(SRC_DIR)/main.c \
		  $(SRC_DIR)/identity.c \
		  $(SRC_DIR)/install.c \
		  $(SRC_DIR)/init_script.c \
		  $(SRC_DIR)/daemonize.c \
		  $(SRC_DIR)/lock.c \
		  $(SRC_DIR)/server.c \
		  $(SRC_DIR)/auth.c \
		  $(SRC_DIR)/session.c \
		  $(SRC_DIR)/shell.c \
		  $(SRC_DIR)/signal.c \
		  $(SRC_DIR)/sha256.c \
		  $(SRC_DIR)/utils.c

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/ft_shield.h $(CRED)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(CRED): $(GEN)
	./$(GEN) "$(PASSWORD)" > $(CRED)

$(GEN): tools/gen_hash.c $(SRC_DIR)/sha256.c $(INC_DIR)/ft_shield.h
	$(CC) $(CFLAGS) -I$(INC_DIR) tools/gen_hash.c $(SRC_DIR)/sha256.c -o $(GEN)

clean:
	rm -rf $(OBJ_DIR) $(GEN)

fclean: clean
	rm -f $(NAME) $(CRED)

re: fclean all

.PHONY: all clean fclean re
