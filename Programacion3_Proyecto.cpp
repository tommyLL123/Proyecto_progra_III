#include <bits/stdc++.h>
using namespace std;

// ===================== UTILIDADES =====================

string normalize(const string& text) {
    string res;

    for (unsigned char c : text) {
        if (isalnum(c) || c == ' ') {
            res += tolower(c);
        }
    }

    return res;
}

vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string word;

    while (ss >> word) {
        tokens.push_back(word);
    }

    return tokens;
}

vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string field;
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
            field += c;
        }
    }

    fields.push_back(field);
    return fields;
}

// ===================== MOVIE =====================

struct Movie {
    int id;
    string title;
    string plot;
    string genre;
    string director;
};

// ===================== TRIE =====================

struct TrieNode {
    unordered_map<char, TrieNode*> children;
    vector<int> movieIds;
    bool end = false;
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    void insert(const string& word, int movieId) {
        TrieNode* node = root;

        for (char c : word) {
            if (!node->children.count(c)) {
                node->children[c] = new TrieNode();
            }

            node = node->children[c];
            node->movieIds.push_back(movieId);
        }

        node->end = true;
    }

    vector<int> search(const string& text) {
        TrieNode* node = root;

        for (char c : text) {
            if (!node->children.count(c)) {
                return {};
            }

            node = node->children[c];
        }

        return node->movieIds;
    }
};

// ===================== SEARCH ENGINE =====================

class SearchEngine {
private:
    vector<Movie> movies;
    Trie trie;

    unordered_map<string, vector<int>> exactIndex;

    set<int> liked;
    set<int> watchLater;

public:
    void loadCSV(const string& filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "No se pudo abrir el archivo: " << filename << endl;
            return;
        }

        string line;

        // Saltar encabezado
        getline(file, line);

        int id = 0;

        while (getline(file, line)) {
            vector<string> fields = parseCSVLine(line);

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
            m.id = id++;
            m.title = normalize(fields[1]);
            m.director = normalize(fields[3]);
            m.genre = normalize(fields[5]);
            m.plot = normalize(fields[7]);

            movies.push_back(m);

            string fullText = m.title + " " + m.plot;
            vector<string> words = tokenize(fullText);

            for (string& w : words) {
                exactIndex[w].push_back(m.id);

                // Insertamos todos los sufijos para permitir búsqueda por sub-palabra.
                // Ejemplo: "desembarcar" permite encontrar "bar".
                for (int i = 0; i < (int)w.size(); i++) {
                    trie.insert(w.substr(i), m.id);
                }
            }
        }

        cout << "Peliculas cargadas: " << movies.size() << endl;
    }

    vector<int> search(string query) {
        query = normalize(query);
        vector<string> words = tokenize(query);

        unordered_map<int, int> score;

        for (string& w : words) {
            // Coincidencia exacta
            if (exactIndex.count(w)) {
                for (int id : exactIndex[w]) {
                    score[id] += 3;
                }
            }

            // Coincidencia parcial / sub-palabra
            vector<int> partial = trie.search(w);

            for (int id : partial) {
                score[id] += 1;
            }
        }

        vector<pair<int, int>> temp;

        for (auto& p : score) {
            temp.push_back({p.first, p.second});
        }

        sort(temp.begin(), temp.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second > b.second;
        });

        vector<int> result;

        for (auto& p : temp) {
            result.push_back(p.first);
        }

        return result;
    }

    void showResults(const vector<int>& results, int page) {
        if (results.empty()) {
            cout << "No se encontraron resultados.\n";
            return;
        }

        int start = page * 5;
        int end = min((int)results.size(), start + 5);

        if (start >= (int)results.size()) {
            cout << "No hay mas resultados.\n";
            return;
        }

        cout << "\n--- RESULTADOS ---\n";

        for (int i = start; i < end; i++) {
            int movieId = results[i];
            cout << i << ". " << movies[movieId].title << endl;
        }
    }

    void showMovie(int id) {
        if (id < 0 || id >= (int)movies.size()) {
            cout << "Pelicula no valida.\n";
            return;
        }

        cout << "\n--- PELICULA SELECCIONADA ---\n";
        cout << "Titulo: " << movies[id].title << endl;
        cout << "Director: " << movies[id].director << endl;
        cout << "Genero: " << movies[id].genre << endl;
        cout << "Sinopsis: " << movies[id].plot << endl;
    }

    void likeMovie(int id) {
        liked.insert(id);
        cout << "Pelicula marcada con Like.\n";
    }

    void addWatchLater(int id) {
        watchLater.insert(id);
        cout << "Pelicula agregada a Ver mas tarde.\n";
    }

    void showWatchLater() {
        cout << "\n--- WATCH LATER ---\n";

        if (watchLater.empty()) {
            cout << "No hay peliculas guardadas en Ver mas tarde.\n";
            return;
        }

        for (int id : watchLater) {
            cout << "- " << movies[id].title << endl;
        }
    }

    bool validMovieId(int id) {
        return id >= 0 && id < (int)movies.size();
    }
};

// ===================== MAIN =====================

int main() {
    SearchEngine engine;

    // Cambia este nombre si tu archivo CSV tiene otro nombre.
    engine.loadCSV("wiki_movie_plots_deduped.csv");

    while (true) {
        engine.showWatchLater();

        cout << "\nBuscar: ";
        string query;
        getline(cin, query);

        if (query == "salir" || query == "q") {
            break;
        }

        vector<int> results = engine.search(query);

        int page = 0;

        while (true) {
            engine.showResults(results, page);

            if (results.empty()) {
                break;
            }

            cout << "\n(n) siguiente | (p) anterior | (s) seleccionar | (q) nueva busqueda: ";

            char op;
            cin >> op;

            if (op == 'n') {
                page++;
            }
            else if (op == 'p') {
                if (page > 0) {
                    page--;
                }
            }
            else if (op == 's') {
                int idx;
                cout << "Indice: ";
                cin >> idx;

                if (idx < 0 || idx >= (int)results.size()) {
                    cout << "Indice invalido.\n";
                    continue;
                }

                int movieId = results[idx];

                engine.showMovie(movieId);

                cout << "\n(l) like | (w) watch later | (b) volver: ";

                char action;
                cin >> action;

                if (action == 'l') {
                    engine.likeMovie(movieId);
                }
                else if (action == 'w') {
                    engine.addWatchLater(movieId);
                }
            }
            else if (op == 'q') {
                break;
            }

            cin.ignore();
        }

        cin.ignore();
    }

    return 0;
}
