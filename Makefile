
CC       = g++
CFLAGS   = -W -o3 -std=c++11

PATH_SRC = ./src/
PATH_BIN = ./bin/

ENGN_DEP = $(addprefix $(PATH_SRC), heap.h trie.h engine.h engine.cpp)
TRIE_DEP = $(addprefix $(PATH_SRC), stack.h trie.h trie.cpp)
MAIN_DEP = $(addprefix $(PATH_SRC), engine.h main.cpp)
OBJS     = $(addprefix $(PATH_BIN), engine.o trie.o main.o)

minisearch : $(OBJS)
	@echo Compiling executable "minisearch"
	$(CC) $(CFLAGS) $(OBJS) -o $(PATH_BIN)minisearch

$(PATH_BIN)engine.o : $(ENGN_DEP)
	@echo Compiling object file "engine.o"
	$(CC) $(CFLAGS) $(PATH_SRC)engine.cpp -c -o $(PATH_BIN)engine.o

$(PATH_BIN)trie.o : $(TRIE_DEP)
	@echo Compiling object file "trie.o"
	$(CC) $(CFLAGS) $(PATH_SRC)trie.cpp -c -o $(PATH_BIN)trie.o

$(PATH_BIN)main.o : $(MAIN_DEP)
	@echo Compiling object file "main.o"
	$(CC) $(CFLAGS) $(PATH_SRC)main.cpp -c -o $(PATH_BIN)main.o

.PHONY clean :
	rm -i $(addprefix $(PATH_BIN), *)
