NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -I include #-fsanitize=address
# -g3 = ajoute les info de debug
# -fsanitize=adress = message d'erreur rouge si on accede a une adresse non permise ou un pointeur mort

# Dossiers
SRC_DIR = srcs
OBJ_DIR = objs
SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(SRC_DIR)/parsing/*.cpp) \
       $(wildcard $(SRC_DIR)/CGI/*.cpp) \
       $(wildcard $(SRC_DIR)/request_response/*.cpp) \
       $(wildcard $(SRC_DIR)/serv_handler/*.cpp)

# Transformation des sources en objets
# Cela va créer une structure miroir dans objs/ (ex: objs/parsing/Config.o)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Dépendances (Headers)
DEPS = $(wildcard include/*.hpp)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

# # Nom du programme
# NAME = webserv

# # Compilateur et flags
# CXX = c++
# CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I include

# # Dossiers
# SRC_DIR = srcs
# OBJ_DIR = objs

# # Recherche de tous les .cpp automatiquement
# SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
#        $(wildcard $(SRC_DIR)/parsing/*.cpp)

# OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# DEPS = $(wildcard include/*.hpp)

# # Règle par défaut
# all: $(NAME)

# # Link final
# $(NAME): $(OBJS)
# 	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# # Compilation des .o
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
# 	@mkdir -p $(dir $@)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Nettoyage
# clean:
# 	rm -rf $(OBJ_DIR)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all

# .PHONY: all clean fclean re