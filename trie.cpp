#include "trie.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <functional>
#include <unordered_map>

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

void Trie::insert(const std::string &word, const std::unordered_map<unsigned, unsigned> &movieScores)
{
    if (word.empty() || movieScores.empty()) {
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

    for (const auto &par : movieScores) {
        actual->scores_[par.first] += par.second;
    }
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

void SearchEngine::loadCSV(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << filename << std::endl;
        return;
    }

    std::string line;

    // Saltar encabezado
    std::getline(file, line);

    unsigned curr_id = 0;

    // Existe un bug donde loadCSV() carga mas peliculas de las que estan en 
    // el csv. Esto es porque en la columna Plot a veces hay caracteres '\n'
    // y la funcion getline() toma este char como delimitador. Esto se arreglara.
    // Aparte de eso, el codigo funciona como deberia.
   while (std::getline(file, line))
{
    // Si el número de comillas es impar, significa que
    // el registro continúa en la siguiente línea.
int quoteCount = std::count(line.begin(), line.end(), '"');

while (quoteCount % 2 != 0) {
    std::string extra;

    if (!std::getline(file, extra))
        break;

    line += '\n';
    line += extra;

    quoteCount += std::count(extra.begin(), extra.end(), '"');
}
    std::vector<std::string> fields = utils::parseCSVLine(line);

    // Dataset esperado:
    // 0 Release Year
    // 1 Title
    // 2 Origin/Ethnicity
    // 3 Director
    // 4 Cast
    // 5 Genre
    // 6 Wiki Page
    // 7 Plot

    if (fields.size() < 8)
        continue;

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

    std::vector<std::string> titleWords =
        utils::tokenize(utils::normalize(m.title_));
    std::vector<std::string> plotWords =
        utils::tokenize(utils::normalize(m.plot_));

    for (std::string &w : titleWords)
        for (size_t i = 0; i < w.size(); i++)
            titlePlotTrie_.insert(w.substr(i), m.id_, 10);

    for (std::string &w : plotWords)
        for (size_t i = 0; i < w.size(); i++)
            titlePlotTrie_.insert(w.substr(i), m.id_, 5);

    std::vector<std::string> directorWords =
        utils::tokenize(utils::normalize(m.director_));
    std::vector<std::string> castWords =
        utils::tokenize(utils::normalize(m.cast_));

    for (std::string &w : directorWords)
        for (size_t i = 0; i < w.size(); i++)
            directorTrie_.insert(w.substr(i), m.id_, 5);

    for (std::string &w : castWords)
        for (size_t i = 0; i < w.size(); i++)
            castTrie_.insert(w.substr(i), m.id_, 5);

    std::vector<std::string> originWords =
        utils::tokenize(utils::normalize(m.origin_));
    std::vector<std::string> genreWords =
        utils::tokenize(utils::normalize(m.genre_));

    yearMap_[m.year_].push_back(m.id_);

    for (std::string &w : originWords)
        originMap_[w].push_back(m.id_);

    for (std::string &w : genreWords)
        genreMap_[w].push_back(m.id_);
}

    std::cout << "Peliculas cargadas: " << movies_.size() << std::endl;
}

std::vector<unsigned> SearchEngine::search(std::string str, SearchEngine::CATEGORIA_BUSQUEDA categ)
{
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

