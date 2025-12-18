NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Werror -Wextra -std=c++98

SRC =	srcs/main.cpp \
		srcs/webserv.cpp \
		srcs/Client.cpp \
		srcs/Server.cpp 

OBJ = $(SRC:.cpp=.o)

all : ${NAME}

${NAME} : ${OBJ}
	@c++ ${CXXFLAGS} ${OBJ} -o ${NAME}

%.o: %.cpp
	c++ ${CXXFLAGS} -c $< -o $@

clean :
	rm -f ${OBJ}

fclean : clean
	rm -f ${NAME}

re : fclean all

.PHONY : all clean fclean re