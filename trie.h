#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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
  TrieNode(char letra, unsigned peso);

  unordered_map<char, TrieNode*> children_;
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

  std::unordered_map<std::string, std::vector<unsigned>> yearMap_;
  std::unordered_map<std::string, std::vector<unsigned>> originMap_;
  std::unordered_map<std::string, std::vector<unsigned>> genreMap_;
};

class User
{
private:
  std::unordered_set<unsigned> liked;
  std::unordered_set<unsigned> watchLater;
};