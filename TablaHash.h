#ifndef TABLAHASH_H
#define TABLAHASH_H

#include <QString>
#include <vector>
#include <list>

class TablaHash {
private:
    int capacidad;
    std::vector<std::list<std::pair<QString, bool>>> tabla;

    int funcionHash(const QString &clave) {
        int hash = 0;
        for (QChar c : clave) {
            hash += c.unicode();
        }
        return hash % capacidad;
    }

public:
    TablaHash(int cap = 10) : capacidad(cap), tabla(cap) {}

    void insertar(const QString &clave, bool descubierto) {
        int indice = funcionHash(clave);
        tabla[indice].push_back({clave, descubierto});
    }

    bool estaDescubierto(const QString &clave) {
        int indice = funcionHash(clave);
        for (auto &par : tabla[indice]) {
            if (par.first == clave) {
                return par.second;
            }
        }
        return false; // Por defecto, no descubierto
    }

    void descubrir(const QString &clave) {
        int indice = funcionHash(clave);
        for (auto &par : tabla[indice]) {
            if (par.first == clave) {
                par.second = true;
                return;
            }
        }
        insertar(clave, true); // Si no existe, lo inserta
    }

};

#endif // TABLAHASH_H
