#include "trie.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// UTILS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool utils::is_num(const std::string &str)
{
    for(auto i = str.cbegin(); i != str.cend(); ++i)
    {
        if(!std::isdigit(*i))
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
        if (std::isalnum(c) || c == ' ') {
            res += std::tolower(c);
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

    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        }
        else {
            if(c == '.' || c == '-'){
                c = ' ';
            }

            field += c;
        }
    }

    fields.push_back(field);
    return fields;
}



///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// TRIE NODE //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TrieNode::TrieNode(): letra_('\0'), peso_(0)
{
}

TrieNode::TrieNode(char letra, unsigned peso): letra_(letra), peso_(peso)
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
}

std::vector<unsigned> Trie::search(const std::string &text)
{
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

    while(std::getline(file, line)) {
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

        for (std::string &w : titleWords) {
            // Insertamos todos los sufijos para permitir búsqueda por sub-palabra.
            // Ejemplo: "desembarcar" permite encontrar "bar".
            for (std::size_t i = 0; i < w.size(); i++) {
                titlePlotTrie_.insert(w.substr(i), m.id_, 10);
            }
        }

        for (std::string &w : plotWords) {
            for (std::size_t i = 0; i < w.size(); i++) {
                titlePlotTrie_.insert(w.substr(i), m.id_, 5);
            }
        }
    }

    std::cout << "Peliculas cargadas: " << movies_.size() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// USER /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


