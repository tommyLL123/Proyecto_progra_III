proy.exe: main.cpp trie.cpp trie.h makefile
	g++ -Wall -Wextra -ansi -pedantic -std=c++17 main.cpp trie.cpp -o proy.exe