#include "trie.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <thread>

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// UTILS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool utils::is_num(const std::string &str)
{
    for (unsigned char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }

    return !str.empty();
}

std::string utils::normalize(const std::string &text)
{
    std::string res;

    for (unsigned char c : text) {
        if (std::isalnum(c)) {
            res += static_cast<char>(std::tolower(c));
        }
        else {
            // Se convierte puntuacion a espacio para separar correctamente palabras.
            res += ' ';
        }
    }

    return res;
}

std::vector<std::string> utils::tokenize(const std::string &text)
{
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string word;

    while (ss >> word) {
        tokens.push_back(word);
    }

    return tokens;
}

std::vector<std::string> utils::parseCSVLine(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            // CSV usa "" para representar una comilla dentro del campo.
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i;
            }
            else {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        }
        else {
            field += c;
        }
    }

    fields.push_back(field);
    return fields;
}

static bool csvRecordIsComplete(const std::string &record)
{
    bool inQuotes = false;

    for (std::size_t i = 0; i < record.size(); ++i) {
        if (record[i] == '"') {
            if (inQuotes && i + 1 < record.size() && record[i + 1] == '"') {
                ++i;
            }
            else {
                inQuotes = !inQuotes;
            }
        }
    }

    return !inQuotes;
}

static bool readCSVRecord(std::ifstream &file, std::string &record)
{
    record.clear();
    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        if (!firstLine) {
            record += '\n';
        }
        record += line;
        firstLine = false;

        if (csvRecordIsComplete(record)) {
            return true;
        }
    }

    return !record.empty();
}



///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// TRIE NODE //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TrieNode::TrieNode(): letra_('\0')
{
}

TrieNode::TrieNode(char letra): letra_(letra)
{
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TRIE /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Trie::Trie()
{
  root_ = new TrieNode();
}

void Trie::insert(const std::string &word, unsigned movieId, unsigned peso)
{
    if (word.empty()) {
        return;
    }

    TrieNode* actual = root_;

    for (unsigned char c : word) {
        if (!std::isalnum(c)) {
            continue;
        }

        char letra = static_cast<char>(std::tolower(c));

        if (actual->children_.find(letra) == actual->children_.end()) {
            actual->children_[letra] = new TrieNode(letra);
        }

        actual = actual->children_[letra];
    }

    if (actual == root_) {
        return;
    }

    actual->scores_[movieId] += peso;
}

std::vector<std::pair<unsigned, unsigned>> Trie::search(const std::string &word)
{
    std::vector<std::pair<unsigned, unsigned>> resultados;

    if (word.empty()) {
        return resultados;
    }

    TrieNode* actual = root_;

    for (unsigned char c : word) {
        if (!std::isalnum(c)) {
            continue;
        }

        char letra = static_cast<char>(std::tolower(c));
        auto it = actual->children_.find(letra);

        if (it == actual->children_.end()) {
            return resultados;
        }

        actual = it->second;
    }

    if (actual == root_) {
        return resultados;
    }

    // Si el usuario busco una sub-palabra, el nodo actual representa el prefijo
    // de varios sufijos. Recorremos el subarbol para recuperar todas las palabras
    // que contienen esa sub-palabra.
    std::unordered_map<unsigned, unsigned> puntajes;

    std::function<void(TrieNode*)> dfs = [&](TrieNode* nodo) {
        for (const auto &par : nodo->scores_) {
            puntajes[par.first] += par.second;
        }

        for (auto &hijo : nodo->children_) {
            dfs(hijo.second);
        }
    };

    dfs(actual);

    for (auto &par : puntajes) {
        resultados.push_back(par);
    }

    std::sort(resultados.begin(), resultados.end(),
              [](const std::pair<unsigned, unsigned>& a,
                 const std::pair<unsigned, unsigned>& b) {
                  if (a.second != b.second) {
                      return a.second > b.second;
                  }
                  return a.first < b.first;
              });

    return resultados;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// SEARCH ENGINE /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////// TEMPLATES ///////////////////////////////////
template<typename TrieType>
void insertFullWords(TrieType& trie,
                     const std::vector<std::string>& words,
                     unsigned movieId,
                     unsigned peso)
{
    std::unordered_set<std::string> inserted;

    for (const std::string& w : words)
    {
        if (!w.empty() && inserted.insert(w).second)
        {
            trie.insert(w, movieId, peso);
        }
    }
}

template<typename TrieType>
void insertTitleSuffixes(TrieType& trie,
                         const std::vector<std::string>& words,
                         unsigned movieId,
                         unsigned peso)
{
    std::unordered_set<std::string> inserted;

    for (const std::string& w : words)
    {
        if (w.empty())
            continue;

        for (size_t i = 0; i < w.size(); i++)
        {
            if (i != 0 && w.size() - i < 3)
                continue;

            std::string suffix = w.substr(i);

            if (inserted.insert(suffix).second)
            {
                trie.insert(suffix, movieId, peso);
            }
        }
    }
}

template<typename MapType>
void fillMap(MapType& map,
            const std::vector<std::string>& words,
            unsigned movieId)
{
    for (const auto& w : words)
    {
        map[w].push_back(movieId);
    }
}

///////////////////////////////// TEMPLATES ///////////////////////////////////


void SearchEngine::loadCSV(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << filename << std::endl;
        return;
    }

    std::string record;

    // Saltar encabezado.
    readCSVRecord(file, record);

    unsigned curr_id = 0;

    while (readCSVRecord(file, record)) {
        std::vector<std::string> fields = utils::parseCSVLine(record);

        // Dataset esperado:
        // 0 Release Year
        // 1 Title
        // 2 Origin/Ethnicity
        // 3 Director
        // 4 Cast
        // 5 Genre
        // 6 Wiki Page
        // 7 Plot
        if (fields.size() < 8) {
            continue;
        }

        Movie m;
        m.id_ = curr_id++;
        m.year_ = utils::is_num(fields[0]) ? std::stoul(fields[0]) : 0;
        m.title_ = fields[1];
        m.origin_ = fields[2];
        m.director_ = fields[3];
        m.cast_ = fields[4];
        m.genre_ = fields[5];
        m.wiki_ = fields[6];
        m.plot_ = fields[7];

        movies_.push_back(m);

        std::vector<std::string> titleWords = utils::tokenize(utils::normalize(m.title_));
        std::vector<std::string> plotWords = utils::tokenize(utils::normalize(m.plot_));
        std::vector<std::string> directorWords = utils::tokenize(utils::normalize(m.director_));
        std::vector<std::string> castWords = utils::tokenize(utils::normalize(m.cast_));
        std::vector<std::string> originWords = utils::tokenize(utils::normalize(m.origin_));
        std::vector<std::string> genreWords = utils::tokenize(utils::normalize(m.genre_));

        // En titulos usamos sufijos para soportar sub-palabras de forma rapida con el Trie.
        insertTitleSuffixes(titlePlotTrie_, titleWords, m.id_, 10);

        // En sinopsis se insertan palabras completas para no hacer demasiado pesada la carga.
        // Las sub-palabras de la sinopsis se verifican en SearchEngine::search con escaneo del texto.
        insertFullWords(titlePlotTrie_, plotWords, m.id_, 5);

        insertFullWords(directorTrie_, directorWords, m.id_, 5);
        insertFullWords(castTrie_, castWords, m.id_, 5);



        yearMap_[m.year_].push_back(m.id_);

        fillMap(originMap_, originWords, m.id_);
        fillMap(genreMap_, genreWords, m.id_);
    }

    std::cout << "Peliculas cargadas: " << movies_.size() << std::endl;
}

std::vector<unsigned> SearchEngine::search(std::string str, SearchEngine::CATEGORIA_BUSQUEDA categ)
{
    std::vector<unsigned> finalResults;
    std::unordered_map<unsigned, unsigned> scoreByMovie;

    std::string normalized = utils::normalize(str);
    std::vector<std::string> words = utils::tokenize(normalized);

    if (words.empty()) {
        return finalResults;
    }

    if (categ == TITLE_PLOT || categ == DIRECTOR || categ == CAST) {
        Trie* selectedTrie = nullptr;

        if (categ == TITLE_PLOT) {
            selectedTrie = &titlePlotTrie_;
        }
        else if (categ == DIRECTOR) {
            selectedTrie = &directorTrie_;
        }
        else if (categ == CAST) {
            selectedTrie = &castTrie_;
        }

        for (const std::string& word : words) {
            std::vector<std::pair<unsigned, unsigned>> partialResults = selectedTrie->search(word);

            for (const auto& result : partialResults) {
                scoreByMovie[result.first] += result.second;
            }
        }

        // Para TITLE_PLOT, además del Trie, verificamos sub-palabras dentro de la sinopsis.
        // Esto evita que la carga sea excesivamente lenta con el CSV completo.

        if (categ == TITLE_PLOT)
        {
            unsigned int numThreads = std::thread::hardware_concurrency();

            if (numThreads == 0)
                numThreads = 2;

            size_t totalMovies = movies_.size();
            
            if (numThreads > totalMovies)
                numThreads = static_cast<unsigned int>(totalMovies);

            std::vector<std::thread> threads;
            
            std::vector<std::unordered_map<unsigned, unsigned>> localScores(numThreads);

            size_t chunkSize = (totalMovies + numThreads - 1) / numThreads;

            for (unsigned int t = 0; t < numThreads; t++)
            {
                size_t startIdx = t * chunkSize;
                size_t endIdx = std::min(startIdx + chunkSize, totalMovies);

                threads.emplace_back([&, t, startIdx, endIdx]()
                {
                    for (size_t i = startIdx; i < endIdx; i++)
                    {
                        const Movie& m = movies_[i];

                        std::string normalizedTitle = utils::normalize(m.title_);
                        std::string normalizedPlot = utils::normalize(m.plot_);

                        for (const std::string& word : words)
                        {
                            if (normalizedTitle.find(word) != std::string::npos)
                            {
                                localScores[t][m.id_] += 10;
                            }

                            if (normalizedPlot.find(word) != std::string::npos)
                            {
                                localScores[t][m.id_] += 5;
                            }
                        }
                    }
                });
            }
            
            for (auto& th : threads)
            {
                th.join();
            }


            for (const auto& localMap : localScores)
            {
                for (const auto& par : localMap)
                {
                    scoreByMovie[par.first] += par.second;
                }
            }
        }
            
    else if (categ == YEAR) {
        if (utils::is_num(words[0])) {
            unsigned year = std::stoul(words[0]);

            auto it = yearMap_.find(year);
            if (it != yearMap_.end()) {
                for (unsigned movieId : it->second) {
                    scoreByMovie[movieId] += 20;
                }
            }
        }
    }
    else if (categ == ORIGIN) {
        for (const std::string& word : words) {
            auto it = originMap_.find(word);
            if (it != originMap_.end()) {
                for (unsigned movieId : it->second) {
                    scoreByMovie[movieId] += 15;
                }
            }
        }
    }
    else if (categ == GENRE) {
        for (const std::string& word : words) {
            auto it = genreMap_.find(word);
            if (it != genreMap_.end()) {
                for (unsigned movieId : it->second) {
                    scoreByMovie[movieId] += 15;
                }
            }
        }
    }

    std::vector<std::pair<unsigned, unsigned>> orderedResults;

    for (const auto& item : scoreByMovie) {
        orderedResults.push_back(item);
    }

    std::sort(
        orderedResults.begin(),
        orderedResults.end(),
        [](const std::pair<unsigned, unsigned>& a,
           const std::pair<unsigned, unsigned>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            return a.first < b.first;
        }
    );

    for (const auto& item : orderedResults) {
        finalResults.push_back(item.first);
    }

    return finalResults;
}
}
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// USER /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void User::addLiked(unsigned movieId)
{
    liked_.insert(movieId);
}

void User::addwatchLater(unsigned movieId)
{
    watchLater_.insert(movieId);
}

const std::unordered_set<unsigned>& User::getLiked() const
{
    return liked_;
}

const std::unordered_set<unsigned>& User::getWatchLater() const
{
    return watchLater_;
}

void User::setState(const std::unordered_set<unsigned>& liked,
                     const std::unordered_set<unsigned>& watchLater)
{
    liked_ = liked;
    watchLater_ = watchLater;
}
