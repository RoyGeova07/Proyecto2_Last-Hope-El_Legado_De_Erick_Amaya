#ifndef ELEMENTO_H
#define ELEMENTO_H

#include <QString>

template <class T>

struct Elemento {
    T dato;          // Prioridad distancia
    QString nivel;  // Nombre del nivel

    Elemento(T datos, const QString& nombre) : dato(datos), nivel(nombre) {}
};

#endif // ELEMENTO_H
