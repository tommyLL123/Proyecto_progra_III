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
            if(c == '.' || c == '-' || c == ':'){
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
    /* Pseudocodigo de insercion
    Este es un Suffix Tree en teoria pero el proceso de encontrar todos los sufijos
    de una palabra es realizado en SearchEngine::LoadCSV() de momento. Insert() recibe
    ya los sufijos de la palabra a insertar.

    El algoritmo de insercion funciona asi:
    1) Se va recorriendo los nodos del arbol (desde la raiz), siguiendo el patron
       de la cadena dada por el usuario hasta que se agote la cadena.
      a) Si existen hijos del nodo actual que siguen el patron, seguir recorriendo
         el arbol.
      b) Si el nodo actual no tiene hijos que sigan el patron de la cadena
        I. Crear nodo hijo al nodo actual con el siguiente caracter de la cadena.
    2) Una vez se acabe la cadena:
      a) Si el vector de IDs del nodo actual no contiene el ID de la pelicula
         asociada a la cadena, insertamos el ID de la pelicula al vector de IDs.
         Tambien insertamos el peso de la cadena al vector de pesos.

    */
}

std::vector<std::pair<unsigned, unsigned>> Trie::search(const std::string &word)
{
    /* Pseudocodigo de algoritmo de busqueda
    1) Proporcionada una cadena, recorremos los nodos del arbol (desde la raiz), siguiendo el patron
       de la cadena dada por el usuario hasta que se agote la cadena.
      a) Si existen hijos del nodo actual que siguen el patron, seguir recorriendo
         el arbol.
      b) Si el nodo actual no tiene hijos que sigan el patron de la cadena
        I. La palabra no esta en el arbol y, por lo tanto, no hay pelicula
           asociada a la palabra. Detener el algoritmo y devolver un vector de
           tuplas (ID, peso) vacio.
    2) Si la cadena se agoto:
      a) Si el nodo actual no tiene hijos, devolver el vector de IDs y el vector 
         pesos del nodo actual como un vector de tuplas (ID, peso).
      b) Si el nodo actual tiene hijos, crear un vector de tuplas (ID, peso):
        I. Recorrer el subarbol usando Depth-First Search, insertando los valores
           respectivos del nodo actual al vector de tuplas. Si el ID ya existe
           en el vector de tuplas, simplemente sumar los pesos.
        II. Una vez recorrido el subarbol, devolver al usuario el vector de tuplas.
    */
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

        std::vector<std::string> directorWords = utils::tokenize(utils::normalize(m.director_));
        std::vector<std::string> castWords = utils::tokenize(utils::normalize(m.cast_));

        for (std::string &w : directorWords) {
            for (std::size_t i = 0; i < w.size(); i++) {
                directorTrie_.insert(w.substr(i), m.id_, 5);
            }
        }

        for (std::string &w : castWords) {
            for (std::size_t i = 0; i < w.size(); i++) {
                castTrie_.insert(w.substr(i), m.id_, 5);
            }
        }

        std::vector<std::string> originWords = utils::tokenize(utils::normalize(m.origin_));
        std::vector<std::string> genreWords = utils::tokenize(utils::normalize(m.genre_));

        // El operador [] de unordered_map encuentra el key y, si no existe, crea una nueva con esa key.
        yearMap_[m.year_].push_back(m.id_);
        for (std::string &w : originWords) {
            originMap_[w].push_back(m.id_);
        }
        for (std::string &w : genreWords) {
            genreMap_[w].push_back(m.id_);
        }
    }

    std::cout << "Peliculas cargadas: " << movies_.size() << std::endl;
}

std::vector<unsigned> SearchEngine::search(std::string str, SearchEngine::CATEGORIA_BUSQUEDA categ)
{
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// USER /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


