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
Trie de busqueda por caracteres.
Cada nodo guarda un caracter y sus hijos. En los nodos terminales se guarda un
mapa pelicula -> puntaje. Se usa puntaje para ordenar las coincidencias por
importancia.

Para permitir busqueda por sub-palabra se insertan sufijos de cada palabra.
Ejemplo: para "desembarcar" se insertan "desembarcar", "esembarcar", ...,
"barcar", etc. Asi una busqueda por "bar" puede encontrar la pelicula.
*/
class TrieNode
{
public:
  TrieNode();
  TrieNode(char letra);

  std::unordered_map<char, TrieNode*> children_;
  std::unordered_map<unsigned, unsigned> scores_; // movieId -> puntaje acumulado
  char letra_;
};

class Trie
{
private:
  TrieNode* root_;

public:
  Trie();

  void insert(const std::string &word, unsigned movieId, unsigned peso);
  void insert(const std::string &word, const std::unordered_map<unsigned, unsigned> &movieScores);
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

  const Movie& getMovie(unsigned id) const { return movies_.at(id); }
  unsigned movieCount() const { return movies_.size(); }
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
