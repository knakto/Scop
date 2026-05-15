NAME := scop
CPP := c++
MKDIR := mkdir -p
RM := rm -rf

SRC_DIR := srcs
INC_DIR := include
OBJ_DIR := obj

FILES := main.cpp render.cpp

INC  = -I$(INC_DIR)
SRC  = $(addprefix $(SRC_DIR)/, $(FILES))
OBJ  = $(addprefix $(OBJ_DIR)/, $(FILES:%.cpp=%.o))

VKFLAGS := -lglfw -lvulkan
FLAGS = $(VKFLAGS)

all: $(NAME)

$(NAME): $(OBJ)
	c++ $^ $(FLAGS) $(INC) -o $@

$(OBJ_DIR):
	$(MKDIR) $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	c++ -c $< $(INC) -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
