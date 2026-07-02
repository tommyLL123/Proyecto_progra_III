#include "trie.h"
#include "patronesdediseño.h"
#include <iostream>

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
            std::cout << "Opcion: ";

            std::string opcionMenu;
            if (!std::getline(std::cin, opcionMenu)) return 0;

            if (opcionMenu == "2")
            {
                mostrarListaVerMasTarde(engine, user);
            }

            volverABuscar = true;
        }
    }

    std::cout << "\nHasta luego.\n";
    return 0;
};
