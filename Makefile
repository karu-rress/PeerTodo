# Makefile for Project 4

# Use the GNU C++ compiler
CC = g++

# 1. Using C++26 standard
# 2. Using Ncurse library
# 3. Using Boost library
# 4. Using Crypto++ library
CXXWARNINGS = # -Wall -Wextra

CXXFLAGS = $(CXXWARNINGS) -std=c++26 -lboost_system -lboost_serialization -lboost_program_options -lcryptopp -I/usr/include/cryptopp -L/usr/lib/cryptopp -lncurses

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

all: p2d

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(BIN_DIR)/session.o: $(INCLUDE_DIR)/session.h $(SRC_DIR)/session.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c $(SRC_DIR)/session.cpp -o $(BIN_DIR)/session.o

$(BIN_DIR)/todo_list.o: $(INCLUDE_DIR)/todo_list.h $(SRC_DIR)/todo_list.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c $(SRC_DIR)/todo_list.cpp -o $(BIN_DIR)/todo_list.o

$(BIN_DIR)/todo.o: $(INCLUDE_DIR)/todo.h $(SRC_DIR)/todo.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c $(SRC_DIR)/todo.cpp -o $(BIN_DIR)/todo.o

$(BIN_DIR)/ui_manager.o: $(INCLUDE_DIR)/ui_manager.h $(SRC_DIR)/ui_manager.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c $(SRC_DIR)/ui_manager.cpp -o $(BIN_DIR)/ui_manager.o

$(BIN_DIR)/user_list.o: $(INCLUDE_DIR)/user_list.h $(SRC_DIR)/user_list.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c $(SRC_DIR)/user_list.cpp -o $(BIN_DIR)/user_list.o

$(BIN_DIR)/user.o: $(INCLUDE_DIR)/user.h $(SRC_DIR)/user.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c $(SRC_DIR)/user.cpp -o $(BIN_DIR)/user.o

$(BIN_DIR)/main.o: main.cpp | $(BIN_DIR)
	$(CC) $(CXXFLAGS) -c main.cpp -o $(BIN_DIR)/main.o

p2d: $(BIN_DIR)/session.o $(BIN_DIR)/todo_list.o $(BIN_DIR)/todo.o $(BIN_DIR)/ui_manager.o $(BIN_DIR)/user_list.o $(BIN_DIR)/user.o $(BIN_DIR)/main.o
	$(CC) $(CXXFLAGS) -o p2d $(BIN_DIR)/session.o $(BIN_DIR)/todo_list.o $(BIN_DIR)/todo.o $(BIN_DIR)/ui_manager.o $(BIN_DIR)/user_list.o $(BIN_DIR)/user.o $(BIN_DIR)/main.o

clean:
	rm -f $(BIN_DIR)/*.o p2d
