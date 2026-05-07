#include <bits/stdc++.h>
using namespace std;

// ===================== UTILIDADES =====================

string normalize(const string& text) {
    string res;
    for (char c : text) {
        if (isalnum(c) || c == ' ')
            res += tolower(c);
    }
    return res;
}

vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string word;
    while (ss >> word) tokens.push_back(word);
    return tokens;
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
    vector<int> movieIds; // películas asociadas
    bool end = false;
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() { root = new TrieNode(); }

    void insert(const string& word, int movieId) {
        TrieNode* node = root;
        for (char c : word) {
            if (!node->children[c])
                node->children[c] = new TrieNode();
            node = node->children[c];
            node->movieIds.push_back(movieId);
        }
        node->end = true;
    }

    vector<int> searchPrefix(const string& prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            if (!node->children.count(c))
                return {};
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

    unordered_map<string, vector<int>> index; // índice invertido

    set<int> liked;
    set<int> watchLater;

public:
    void loadCSV(const string& filename) {
        ifstream file(filename);
        string line;

        int id = 0;

        while (getline(file, line)) {
            stringstream ss(line);
            string title, plot, genre, director;

            getline(ss, title, ',');
            getline(ss, plot, ',');
            getline(ss, genre, ',');
            getline(ss, director, ',');

            Movie m;
            m.id = id++;
            m.title = normalize(title);
            m.plot = normalize(plot);
            m.genre = normalize(genre);
            m.director = normalize(director);

            movies.push_back(m);

            // indexar
            auto words = tokenize(m.title + " " + m.plot);

            for (auto& w : words) {
                index[w].push_back(m.id);
                trie.insert(w, m.id);
            }
        }
    }

    vector<int> search(string query) {
        query = normalize(query);
        auto words = tokenize(query);

        unordered_map<int, int> score;

        for (auto& w : words) {
            // índice invertido
            if (index.count(w)) {
                for (int id : index[w])
                    score[id] += 3;
            }

            // prefijo con Trie
            auto pref = trie.searchPrefix(w);
            for (int id : pref)
                score[id] += 1;
        }

        // ordenar resultados
        vector<pair<int,int>> temp(score.begin(), score.end());

        sort(temp.begin(), temp.end(), [](auto& a, auto& b) {
            return a.second > b.second;
        });

        vector<int> result;
        for (auto& p : temp) result.push_back(p.first);

        return result;
    }

    void showResults(const vector<int>& results, int page) {
        int start = page * 5;
        int end = min((int)results.size(), start + 5);

        for (int i = start; i < end; i++) {
            cout << i << ". " << movies[results[i]].title << endl;
        }
    }

    void showMovie(int id) {
        cout << "\nTITLE: " << movies[id].title << endl;
        cout << "PLOT: " << movies[id].plot << endl;
    }

    void like(int id) {
        liked.insert(id);
    }

    void addWatchLater(int id) {
        watchLater.insert(id);
    }

    void showWatchLater() {
        cout << "\n--- WATCH LATER ---\n";
        for (int id : watchLater) {
            cout << movies[id].title << endl;
        }
    }
};

// ===================== MAIN =====================

int main() {
    SearchEngine engine;

    engine.loadCSV("movies.csv");

    engine.showWatchLater();

    while (true) {
        cout << "\nBuscar: ";
        string query;
        getline(cin, query);

        auto results = engine.search(query);

        int page = 0;
        while (true) {
            engine.showResults(results, page);

            cout << "\n(n) siguiente | (s) seleccionar | (q) salir: ";
            char op;
            cin >> op;

            if (op == 'n') {
                page++;
            } else if (op == 's') {
                int idx;
                cout << "Indice: ";
                cin >> idx;

                int movieId = results[idx];
                engine.showMovie(movieId);

                cout << "(l) like | (w) watch later: ";
                char c;
                cin >> c;

                if (c == 'l') engine.like(movieId);
                if (c == 'w') engine.addWatchLater(movieId);

            } else break;
        }

        cin.ignore();
    }
}