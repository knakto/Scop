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

BONUS_FLAGS := -D__USE_VT -D__USE_VN -D__USE_MTL -D__USE_MTL_COLOR -D__USE_MTL_TEXTURE -D__USE_MOUSE_CLICK -D__USE_MOUSE_SCROLL -D__USE_WIREFRAME
CFLAGS := -MMD -MP -g -Wall -Wextra -Werror -Wpedantic
VKFLAGS := -lglfw -lGLEW -lGL
FLAGS = $(VKFLAGS) $(CFLAGS) $(BONUS_FLAGS)

all: $(NAME)

$(NAME): $(OBJ)
	c++ $^ $(FLAGS) $(INC) -o $@

$(OBJ_DIR):
	$(MKDIR) $@

-include $(DEP)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile | $(OBJ_DIR)
	c++ $(CFLAGS) $(BONUS_FLAGS) -c $< $(INC) -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
