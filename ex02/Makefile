# Compiler and Flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Executable Name
NAME = PmergeMe

# Source Files
SRCS = main.cpp PmergeMe.cpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Header Files
HDRS = PmergeMe.hpp

# Default Rule: Build the executable
all: $(NAME)

# Rule to link the executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# Rule to compile source files into object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean object files
clean:
	rm -f $(OBJS)

# Rule to clean executable and object files
fclean: clean
	rm -f $(NAME)

# Rule to rebuild the project
re: fclean all

# Phony rules
.PHONY: all clean fclean re
