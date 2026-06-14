NAME := scop
CPP := c++
MKDIR := mkdir -p
RM := rm -rf

SRC_DIR := srcs
INC_DIR := include
OBJ_DIR := obj

FILES := main.cpp Application.cpp RotationMatrix.cpp Object.cpp Image.cpp

INC  = -I$(INC_DIR)
SRC  = $(addprefix $(SRC_DIR)/, $(FILES))
OBJ  = $(addprefix $(OBJ_DIR)/, $(FILES:%.cpp=%.o))
DEP  = $(OBJ:%.o=%.d)

CFLAGS := -MMD -MP -g -Wall -Wextra -Werror -Wpedantic
VKFLAGS := -lglfw -lGLEW -lGL
FLAGS = $(VKFLAGS) $(CFLAGS)

all: $(NAME)

$(NAME): $(OBJ)
	c++ $^ $(FLAGS) $(INC) -o $@

$(OBJ_DIR):
	$(MKDIR) $@

-include $(DEP)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile | $(OBJ_DIR)
	c++ $(FLAGS) -c $< $(INC) -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
