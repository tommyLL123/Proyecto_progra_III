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


class TrieNode
{
public:
  TrieNode();
  TrieNode(char letra, unsigned peso);

  std::unordered_map<char, TrieNode*> children_;
  std::vector<unsigned> movieIds_;
  char letra_;
  unsigned peso_;
};

class Trie
{
private:
  TrieNode* root_;
public:
  Trie();

  void insert(const std::string &word, unsigned movieId, unsigned peso);
  std::vector<unsigned> search(const std::string &text);
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