#ifndef COLA_H
#define COLA_H

#include <vector>
#include <algorithm>
#include "Elemento.h"

template <class T>
class Cola {
private:
    std::vector<Elemento<T>> elementos;

public:
    bool colaVacia() const {
        return elementos.empty();
    }

    void insertar(T dato, const QString& nivel) {
        Elemento<T> nuevo(dato, nivel);

        // Insertar manteniendo el orden (cola de prioridad)
        auto it = std::lower_bound(elementos.begin(), elementos.end(), nuevo,
                                   [](const Elemento<T>& a, const Elemento<T>& b) {
                                       return a.dato < b.dato;
                                   });

        elementos.insert(it, nuevo);
    }

    Elemento<T> desencolar() {
        if (colaVacia()) {
            return Elemento<T>(T(), QString());            // Retornar un elemento inválido

        }

        Elemento<T> primero = elementos.front();
        elementos.erase(elementos.begin());
        return primero;
    }
};

#endif // COLA_H
