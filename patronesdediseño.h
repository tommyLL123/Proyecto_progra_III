#ifndef PROYECTO_PROGRA_III_MAIN_MOVIE_ITERATOR_H
#define PROYECTO_PROGRA_III_MAIN_MOVIE_ITERATOR_H

#pragma once
#include <vector>
#include <algorithm>
#include "trie.h"
#include <iostream>
#include <string>

class ResultsIterator {
    std::vector<unsigned> ids_;
    std::size_t cursor_;
    std::size_t pageSize_;

public:
    explicit ResultsIterator(std::vector<unsigned> ids, std::size_t pageSize = 5)
        : ids_(std::move(ids)), cursor_(0), pageSize_(pageSize) {}

    bool hasNext() const { return cursor_ + pageSize_ < ids_.size(); }
    bool hasPrev() const { return cursor_ >= pageSize_; }
    void next() { if (hasNext()) cursor_ += pageSize_; }
    void prev() { if (hasPrev()) cursor_ -= pageSize_; else cursor_ = 0; }

    std::vector<unsigned> currentPage() const
    {
        std::size_t fin = std::min(cursor_ + pageSize_, ids_.size());
        return std::vector<unsigned>(ids_.begin() + cursor_, ids_.begin() + fin);
    }

    std::size_t totalPages() const
    {
        if (ids_.empty()) return 1;
        return (ids_.size() + pageSize_ - 1) / pageSize_;
    }

    std::size_t currentPageNumber() const { return (cursor_ / pageSize_) + 1; }
    bool empty() const { return ids_.empty(); }
};



// ---------------- OBSERVER ----------------
class IObserver{
public:                                    // <-- agregado
    virtual ~IObserver() = default;
    virtual void onUserChange(const std::string& mensaje) = 0;
};

class ConsoleNotifier : public IObserver{
public:
    void onUserChange(const std::string& mensaje) override
    {
        std::cout << "[NOTIFICACION] " << mensaje << "\n";
    }
};

// ---------------- MEMENTO ----------------
class UserMemento
{
private:
    std::unordered_set<unsigned> liked_;
    std::unordered_set<unsigned> watchLater_;
    bool hasSnapshot_ = false;

public:

    void save(const User& user)
    {
        liked_ = user.getLiked();
        watchLater_ = user.getWatchLater();
        hasSnapshot_ = true;
    }

    bool restore(User& user)
    {
        if (!hasSnapshot_) return false;
        user.setState(liked_, watchLater_);
        hasSnapshot_ = false;
        return true;
    }

    bool hasCheckpoint() const { return hasSnapshot_; }
};


class UserActionProxy{
    User& user_;
    UserMemento memento_;
    std::vector<IObserver*> observers_;

    void notify(const std::string& mensaje)
    {
        for (IObserver* obs : observers_) obs->onUserChange(mensaje);
    }

    bool confirmar(const std::string& pregunta)
    {
        std::cout << pregunta << " (s/n): ";
        std::string resp;
        std::getline(std::cin, resp);
        return resp == "s" || resp == "S" || resp == "si" || resp == "Si" || resp == "SI";
    }

    public:

    explicit UserActionProxy(User& user) : user_(user) {}

    void addObserver(IObserver* obs) { observers_.push_back(obs); }

    void likeMovie(unsigned movieId, const std::string& titulo)
    {
        if (!confirmar("Deseas dar Like a \"" + titulo + "\"?"))
        {
            std::cout << "Accion cancelada.\n";
            return;
        }
        memento_.save(user_);
        user_.addLiked(movieId);
        notify("Se marco con Like: " + titulo);
    }

    void watchLaterMovie(unsigned movieId, const std::string& titulo)
    {
        if (!confirmar("Deseas agregar \"" + titulo + "\" a Ver mas tarde?"))
        {
            std::cout << "Accion cancelada.\n";
            return;
        }
        memento_.save(user_);
        user_.addwatchLater(movieId);
        notify("Se agrego a Ver mas tarde: " + titulo);
    }

    bool undo()
    {
        if (memento_.restore(user_))
        {
            notify("Se deshizo el ultimo cambio.");
            return true;
        }
        std::cout << "No hay cambios para deshacer.\n";
        return false;
    }

};

#endif //PROYECTO_PROGRA_III_MAIN_MOVIE_ITERATOR_H