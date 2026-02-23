NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -I include #-fsanitize=address
# -g3 = ajoute les info de debug
# -fsanitize=adress = message d'erreur rouge si on accede a une adresse non permise ou un pointeur mort

# Dossiers
SRC_DIR = srcs
OBJ_DIR = objs
SRCS = $(addprefix $(SRC_DIR)/, $(addprefix parsing/, Global_Config.cpp Location_config.cpp parsing.cpp Server_config.cpp utils_parsing.cpp) \
								$(addprefix request_response/, DeleteRequest.cpp GetRequest.cpp HTTPRequest.cpp HTTPResponse.cpp PostRequest.cpp) \
								$(addprefix serv_handler/, Client.cpp Server.cpp utils_webserv.cpp webserv.cpp) \
								main.cpp \
		)

# Transformation des sources en objets
# Cela va créer une structure miroir dans objs/ (ex: objs/parsing/Config.o)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Dépendances (Headers)
DEPS = $(addprefix include/, $(addprefix parsing/, Global_Config.hpp Location_config.hpp parsing.hpp Server_config.hpp) \
							 $(addprefix request_response/, DeleteRequest.hpp GetRequest.hpp HTTPRequest.hpp HTTPResponse.hpp PostRequest.hpp) \
							 $(addprefix serv_handler/, Client.hpp Server.hpp Webserv.hpp) \
		)

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
