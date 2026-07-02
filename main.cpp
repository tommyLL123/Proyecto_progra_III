#include "trie.h"
#include "patronesdediseño.h"
#include <iostream>
#include <algorithm>
#include <map>

namespace {

void mostrarPagina(const SearchEngine& engine, const ResultsIterator& it)
{
    std::cout << "\n--- RESULTADOS (pagina " << it.currentPageNumber()
              << " de " << it.totalPages() << ") ---\n\n";

    std::vector<unsigned> pagina = it.currentPage();

    if (pagina.empty())
    {
        std::cout << "No se encontraron peliculas.\n";
        return;
    }

    for (std::size_t i = 0; i < pagina.size(); i++)
    {
        std::cout << i << ". " << engine.getMovie(pagina[i]).title_ << "\n";
    }
}


long seleccionarPelicula(const SearchEngine& engine, ResultsIterator& it)
{
    while (true)
    {
        mostrarPagina(engine, it);

        if (it.empty())
        {
            return -1;
        }

        std::cout << "\n(n) siguiente | (p) anterior | (s) seleccionar | (c) cancelar | (x) salir: ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) return -2;

        if (cmd == "n")
        {
            it.next();
        }
        else if (cmd == "p")
        {
            it.prev();
        }
        else if (cmd == "s")
        {
            std::cout << "Indice: ";
            std::string idxStr;
            if (!std::getline(std::cin, idxStr)) return -2;

            try
            {
                std::size_t idx = std::stoul(idxStr);
                std::vector<unsigned> pagina = it.currentPage();

                if (idx < pagina.size())
                {
                    return static_cast<long>(pagina[idx]);
                }

                std::cout << "Indice invalido.\n";
            }
            catch (...)
            {
                std::cout << "Indice invalido.\n";
            }
        }
        else if (cmd == "c")
        {
            return -1;
        }
        else if (cmd == "x")
        {
            return -2;
        }
    }
}

enum class ResultadoMenuPelicula
{
    CAMBIO_REALIZADO,
    VOLVER,
    SALIR_PROGRAMA
};


ResultadoMenuPelicula menuAccionesPelicula(const SearchEngine& engine, UserActionProxy& actions, unsigned movieId)
{
    const Movie& m = engine.getMovie(movieId);

    std::cout << "\n--- PELICULA SELECCIONADA ---\n\n";
    std::cout << "Titulo: " << m.title_ << "\n";
    std::cout << "Director: " << m.director_ << "\n";
    std::cout << "Genero: " << m.genre_ << "\n";
    std::cout << "Sinopsis: " << m.plot_ << "\n";

    std::cout << "\n¿Que deseas hacer?\n";
    std::cout << "(1) Like\n";
    std::cout << "(2) Ver mas tarde\n";
    std::cout << "(3) Salir\n";
    std::cout << "Opcion: ";

    std::string opcion;
    if (!std::getline(std::cin, opcion)) return ResultadoMenuPelicula::SALIR_PROGRAMA;

    if (opcion == "1")
    {
        actions.likeMovie(movieId, m.title_);
        return ResultadoMenuPelicula::CAMBIO_REALIZADO;
    }
    else if (opcion == "2")
    {
        actions.watchLaterMovie(movieId, m.title_);
        return ResultadoMenuPelicula::CAMBIO_REALIZADO;
    }
    else if (opcion == "3")
    {
        return ResultadoMenuPelicula::VOLVER;
    }

    std::cout << "Opcion invalida, no se hizo ningun cambio.\n";
    return ResultadoMenuPelicula::VOLVER;
}

void mostrarListaVerMasTarde(const SearchEngine& engine, const User& user)
{
    std::cout << "\n--- VER MAS TARDE ---\n\n";

    if (user.getWatchLater().empty())
    {
        std::cout << "Esta vacio.\n";
        return;
    }

    for (unsigned id : user.getWatchLater())
    {
        std::cout << "- " << engine.getMovie(id).title_ << "\n";
    }
}

void mostrarRecomendados(const SearchEngine& engine, const User& user)
{
  std::cout << "\n--- RECOMENDACIONES ---\n\n";

  std::unordered_set<unsigned> likedPelis = user.getLiked();

  if(likedPelis.empty())
  {
    std::cout << "Denle like a una pelicula para acceder a recomendaciones.\n";
    return;
  }

  std::unordered_map<std::string, unsigned> generos;
  std::unordered_map<std::string, unsigned> directores;
  std::string titulos;

  for(auto i = likedPelis.cbegin(); i != likedPelis.cend(); ++i)
  {
    const Movie curr_mov = engine.getMovie(*i);

    generos[curr_mov.genre_]++;
    directores[curr_mov.director_]++;
    titulos += curr_mov.title_ + " ";
  }
  
  auto cmp = [](const std::pair<std::string, unsigned> &lhs, std::pair<std::string, unsigned> &rhs)
               { return lhs.second > rhs.second;};

  std::vector<std::pair<std::string, unsigned>> ord_generos(generos.begin(), generos.end());
  std::sort(ord_generos.begin(), ord_generos.end(), cmp);
  std::vector<std::pair<std::string, unsigned>> ord_directores(directores.begin(), directores.end());
  std::sort(ord_directores.begin(), ord_directores.end(), cmp);

  
  std::unordered_set<unsigned> generos_match;
  unsigned num_max = 0;

  for(auto i = ord_generos.begin(); i != ord_generos.end() && num_max < 3; ++i, ++num_max)
  { 
    std::vector<unsigned> temp = engine.search(i->first, SearchEngine::CATEGORIA_BUSQUEDA::GENRE);
    generos_match.insert(temp.begin(), temp.end());
  }

  std::unordered_set<unsigned> directores_match;
  num_max = 0;

  for(auto i = ord_directores.begin(); i != ord_directores.end() && num_max < 3; ++i, ++num_max)
  { 
    std::vector<unsigned> temp = engine.search(i->first, SearchEngine::CATEGORIA_BUSQUEDA::DIRECTOR);
    directores_match.insert(temp.begin(), temp.end());
  }

  std::vector<unsigned> titulos_match = engine.search(titulos, SearchEngine::CATEGORIA_BUSQUEDA::TITLE_PLOT);

  std::vector<unsigned> recomendaciones;

  num_max = 0;
  for(auto i = titulos_match.begin(); i != titulos_match.end() && num_max < 5; ++i)
  {
    if(likedPelis.find(*i) != likedPelis.end())
    {
      continue;
    }
    else if(directores_match.find(*i) != directores_match.end())
    {
      recomendaciones.push_back(*i);
      num_max++;
    }
    else if(generos_match.find(*i) != generos_match.end())
    {
      recomendaciones.push_back(*i);
      num_max++;
    }
  }


  for (unsigned id : recomendaciones)
  {
      std::cout << "- " << engine.getMovie(id).title_ << "\n";
  }
}

bool preguntarSiNo(const std::string& pregunta)
{
    std::cout << pregunta << " (si/no): ";
    std::string resp;
    if (!std::getline(std::cin, resp)) return false;
    return resp == "si" || resp == "s" || resp == "S" || resp == "Si";
}



}

int main(void)
{

    std::cout << "\nCargando peliculas...\n";
    SearchEngine engine;
    engine.loadCSV("wiki_movie_plots_deduped.csv");

    User user;
    UserActionProxy actions(user);
    ConsoleNotifier notifier;
    actions.addObserver(&notifier);

    while (true)
    {
        std::cout << "\nBuscar pelicula: ";
        std::string consulta;
        if (!std::getline(std::cin, consulta)) break;

        if (consulta.empty()) continue;

        std::vector<unsigned> resultados = engine.search(consulta, SearchEngine::TITLE_PLOT);
        ResultsIterator it(resultados);

        bool volverABuscar = false;

        while (!volverABuscar)
        {
            long movieId = seleccionarPelicula(engine, it);

            if (movieId == -2)
            {
                std::cout << "\nHasta luego.\n";
                return 0;
            }

            if (movieId < 0)
            {
                break;
            }

            ResultadoMenuPelicula resultado = menuAccionesPelicula(engine, actions, static_cast<unsigned>(movieId));

            if (resultado == ResultadoMenuPelicula::SALIR_PROGRAMA)
            {
                std::cout << "\nHasta luego.\n";
                return 0;
            }
            else if (resultado == ResultadoMenuPelicula::VOLVER)
            {
                continue;
            }

            if (!preguntarSiNo("\n¿Deseas continuar?"))
            {
                std::cout << "\nHasta luego.\n";
                return 0;
            }

            std::cout << "\n(1) Buscar otra pelicula\n";
            std::cout << "(2) Ver lista\n";
            std::cout << "(3) Ver recomendaciones\n";
            std::cout << "Opcion: ";

            std::string opcionMenu;
            if (!std::getline(std::cin, opcionMenu)) return 0;

            if (opcionMenu == "2")
            {
                mostrarListaVerMasTarde(engine, user);
            }
            else if (opcionMenu == "3")
            {
                mostrarRecomendados(engine, user);
            }

            volverABuscar = true;
        }
    }

    std::cout << "\nHasta luego.\n";
    return 0;
};
