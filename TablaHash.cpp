#include "tablahash.h"

TablaHash::TablaHash(QObject *parent, int cap)
    : QObject(parent), capacidad(cap), tabla(cap) {}

int TablaHash::funcionHash(const QString &clave) {
    int hash = 0;
    for (QChar c : clave) {
        hash += c.unicode();
    }
    return hash % capacidad;
}

TablaHash& TablaHash::getInstance() {
    static TablaHash instance;
    return instance;
}

void TablaHash::insertar(const QString &clave, bool descubierto) {
    int indice = funcionHash(clave);
    tabla[indice].push_back({clave, descubierto});
}

bool TablaHash::estaDescubierto(const QString &clave) {
    int indice = funcionHash(clave);
    for (auto &par : tabla[indice]) {
        if (par.first == clave) {
            return par.second;
        }
    }
    return false;
}

void TablaHash::descubrir(const QString &clave) {
    int indice = funcionHash(clave);
    for (auto &par : tabla[indice]) {
        if (par.first == clave) {
            if (!par.second) {
                par.second = true;
                emit datoModificado(clave);
            }
            return;
        }
    }
    insertar(clave, true);
    emit datoModificado(clave);
}
