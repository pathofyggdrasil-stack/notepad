NAME	:= emipad

SRC_DIR		:= source
OBJ_DIR		:= obj
BUILD_DIR	:= build

SRC		:= $(wildcard $(SRC_DIR)/*.c)

OBJ		:= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# .D files

DEPS	:= $(OBJ:.o=.d)

#flags

CC			:= gcc
CFLAG		:= -g -Iinclude -Og -std=c99
LDFLAG		:= -lraylib -lm -lpthread -ldl

ifeq ($(OS), Windows_NT)
LDFLAG		+= -lgdi32 -lwinmm
else
LDFLAG		+= -lX11 -lrt -lGL
endif


all:	$(NAME)

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAG) -c $< -o $@

# Main Exec

$(NAME): $(OBJ) | $(BUILD_DIR)
	$(CC) $(OBJ) $(LDFLAG) -o $(BUILD_DIR)/$(NAME)

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BUILD_DIR)

re: fclean all

# Include generated header dependency files (.d)
-include $(DEPS)

.PHONY: all clean fclean re
