#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace utils
{
  bool is_num(const std::string &str);
  std::string normalize(const std::string& text);
  std::vector<std::string> tokenize(const std::string &text);
  std::vector<std::string> parseCSVLine(const std::string &line);
}


class Movie
{
public:
  unsigned id_;
  unsigned year_;
  std::string title_;
  std::string origin_;
  std::string director_;
  std::string cast_;
  std::string genre_;
  std::string wiki_;
  std::string plot_;
};

/*
Cada nodo contiene:
  1) Un char que representa la letra de ese nodo.
  2) Un contenedor unordered_map de los hijos de el nodo actual. Se utilizo
     unordered_map ya que la busqueda es O(1) la mayoria de los casos, lo cual
     nos permite navegar el arbol de manera rapida y consistente.
  3) Un vector de IDs de peliculas asociadas a la palabra formada por el camino
     de un nodo despues de la raiz hasta el nodo actual. Si el vector es vacio
     sabemos que el nodo actual no es una hoja.
  4) Un vector de enteros llamado pesos que sirve para decidir cual pelicula
     encaja mas con la cadena dada por el usuario en busquedas.
     De momento, si la palabra aparece en el titulo tiene un peso de 10, mientras
     que palabras en el plot valen 5 puntos.
  Los indices de pesos corresponden con los indices del vector de IDs, es decir,
  si el ID de una pelicula esta en el indice 5, el peso de la cadena asociada a
  esa pelicula tambien esta en el indice 5 de pesos.
*/
class TrieNode
{
public:
  TrieNode();
  TrieNode(char letra);

  std::unordered_map<char, TrieNode*> children_;
  std::vector<unsigned> movieIds_;
  std::vector<unsigned> pesos_;
  char letra_;
  
};

class Trie
{
private:
  TrieNode* root_;
public:
  Trie();

  void insert(const std::string &word, unsigned movieId, unsigned peso);
  std::vector<std::pair<unsigned, unsigned>> search(const std::string &word);
};

class SearchEngine
{
private:
  std::vector<Movie> movies_;

  Trie titlePlotTrie_;
  Trie directorTrie_;
  Trie castTrie_;

  std::unordered_map<unsigned, std::vector<unsigned>> yearMap_;
  std::unordered_map<std::string, std::vector<unsigned>> originMap_;
  std::unordered_map<std::string, std::vector<unsigned>> genreMap_;
public:
  enum CATEGORIA_BUSQUEDA
  {
    TITLE_PLOT,
    DIRECTOR,
    CAST,
    YEAR,
    ORIGIN,
    GENRE
  };

  void loadCSV(const std::string& filename);
  std::vector<unsigned> search(std::string str, CATEGORIA_BUSQUEDA categ);
};

class User
{
private:
  std::unordered_set<unsigned> liked_;
  std::unordered_set<unsigned> watchLater_;
public:
  void addLiked(unsigned movieId);
  void addwatchLater(unsigned movieId);
};