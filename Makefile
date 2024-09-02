NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g


SRC_DIR = sources
OBJ_DIR = objects
INCLUDE_DIR = includes


SRCS = $(addprefix $(SRC_DIR)/, 	main.cpp 				\
									CgiRequestHandler.cpp 	\
									Config.cpp 				\
									Log.cpp 				\
									Parser.cpp 				\
									Request.cpp 			\
									RequestBuffer.cpp 		\
									RequestHandlers.cpp 	\
									Response.cpp 			\
									Server.cpp 				\
									signal.cpp )


HEADERS = $(addprefix $(INCLUDE_DIR)/,  CgiRequestHandler.hpp 	\
										Config.hpp 				\
										Log.hpp 				\
										Parser.hpp 				\
										Request.hpp 			\
										RequestHandlers.hpp 	\
										Response.hpp 			\
										Server.hpp)


OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))



# Colors
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

all: $(NAME)
	@$(MAKE) -s manual

$(NAME): $(OBJS)
	@echo "\n"
	@echo "$(GREEN)Linking object files...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE_DIR)
	@echo "$(GREEN)$(NAME) successfully compiled!$(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp  $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	@printf "\033[K$(GREEN)Compiling $< ...$(NC)\r"
	@$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Cleaning $(NAME)...$(NC)"
	@rm -fr $(NAME)

re: fclean all










manual: 
	@echo "\n\n$(BLUE)__| |________________________________________________________________________| |__"
	@echo "__   ________________________________________________________________________   __"
	@echo "  | |                                                                        | |  "
	@echo "  | |                          ,,                                            | |  "
	@echo "  | |                         *MM                                            | |  "
	@echo "  | |                          MM                                            | |  "
	@echo "  | | 7M'    ,A     MF'.gP*Ya  MM,dMMb.  ,pP*Ybd  .gP*Ya  7Mb,od8  7M'    MF'| |"
	@echo "  | |  VA   ,VAA   ,V ,M'   Yb MM     Mb 8I    '' ,M'   Yb  MM' ''   VA   ,V | |  "
	@echo "  | |   VA ,V  VA ,V  8M****** MM     M8  YMMMa. 8M******  MM        VA ,V   | |  "
	@echo "  | |    VVV    VVV   YM.    , MM.   ,M9 L.   I8 YM.    ,  MM         VVV    | |  "
	@echo "  | |     W      W      Mbmmd' P^YbmdP'  M9mmmP'   Mbmmd'.JMML.        W     | |  "
	@echo "__| |________________________________________________________________________| |__"
	@echo "__   ________________________________________________________________________   __"
	@echo "  | |                                                                        | |  $(NC)"
	@echo "\n"
	@echo "$(GREEN)                      Webserv Configuration File Guide$(NC)"
	@echo "                         Directives and Arguments:"
	@echo ""
	@echo "====================================================================================="
	@echo "1. server_name: Defines the name of the server."
	@echo "2. error_page: Specifies the error page for a given error code."
	@echo "3. listen: Specifies the port number on which the server listens."
	@echo "4. client_max_body_size: Sets the maximum size for client request bodies."
	@echo "\n5. $(RED)[IMPORTANT]$(NC) CGI HANDLING: To enable CGI handling for a location,"
	@echo "\n   you must specify both cgi_path and cgi_extension within the location directive."
	@echo "The appropriate interpreter for each extension $(RED)MUST$(NC) correspond to the"
	@echo "index of the extension in the list. For example, if '.py' is the first extension"
	@echo "in the list, then the appropriate Python interpreter $(RED)MUST$(NC) also be the"
	@echo "first entry in the 'cgi_path' list."
	@echo "\n$(BLUE)[EXCEPTION]$(NC): The $(YELLOW).cgi$(NC) extension is intended for binaries"
	@echo "that do not require an interpreter."
	@echo "====================================================================================="
	@echo ""
	@echo "\n                                  Usage:"
	@echo "====================================================================================="
	@echo "1. Define the directives in the configuration file (e.g., webserv.conf)."
	@echo "2. Ensure the configuration file is in the correct format and location."
	@echo "3. Start the server, and it will read the configuration file for settings."
	@echo "====================================================================================="
	@echo "\nFor more information, refer to the README.md file."


.PHONY: all clean fclean re