#include "trie.h"

int main(void)
{
  SearchEngine engine;

  // Cambia este nombre si tu archivo CSV tiene otro nombre.
  engine.loadCSV("wiki_movie_plots_deduped.csv");

  return 0;
}