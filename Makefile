# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: astripeb <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/31 20:08:29 by astripeb          #+#    #+#              #
#    Updated: 2022/09/12 21:56:39 by astripeb         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

GREEN				:= \033[0;32m
RED 				:= \033[0;31m
RESET				:= \033[0m

NAME				:= gomoku

SRC_DIR			:= ./algorithm/src
INC_DIR			:= ./algorithm/include
OBJ_DIR			:= ./obj

SRCS				:= main.cpp Server.cpp GomokuGuiConnection.cpp Gomoku.cpp \
							 serialization.cpp utils.cpp gomoku_utils.cpp


OBJ					:= $(SRCS:.cpp=.o)

CXX					= g++
CXXFLAGS		?= -Wall -Werror -Wextra -std=c++14
IFLAGS			:= -I$(INC_DIR)
DEPEND			:= -MD -MT

OS = $(shell uname)
ifeq ($(OS), Darwin)
  CXX = clang++
  IFLAGS += -I${HOME}/homebrew/include
else
	LIBS = -lpthread -lboost_system
endif

ifeq ($(DEBUG), 1)
  CXXFLAGS += -D DEBUG
endif

vpath %.cpp		$(SRC_DIR)
vpath %.h			$(INC_DIR)
vpath %.o			$(OBJ_DIR)

all: $(NAME)

run:
	python3 run_gui.py

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJ)) -o $@ $(LIBS)
	echo "$(GREEN)$@ was created ✅$(RESET)"

$(OBJ):%.o:%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(IFLAGS) -o $(OBJ_DIR)/$@ -c $< $(DEPEND) $@ $(LIBS)
	echo "$(GREEN)$@ was created$(RESET)"

$(OBJ_DIR):
	mkdir -p $@

Debug:
	$(MAKE) CXXFLAGS="-g"

include $(wildcard $(OBJ_DIR)/*.d)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean
	$(MAKE) all

.SILENT: all clean fclean re $(NAME) $(OBJ_DIR) $(OBJ) Debug run

.PHONY: clean fclean re all Debug
