#include "Grafo.h"

Grafo::Grafo() {}

void Grafo::agregarNodo(const QString& nombre, const QPointF& posicion) {
    if (!listaAdyacencia.contains(nombre)) {
        listaAdyacencia[nombre] = QList<Arista>();
        posicionesNodos[nombre] = posicion;
    }
}

void Grafo::agregarArista(const QString& origen, const QString& destino, int peso,
                          const QList<QPointF>& puntosIntermedios) {
    if (listaAdyacencia.contains(origen) && listaAdyacencia.contains(destino)) {
        listaAdyacencia[origen].append(Arista(destino, peso, puntosIntermedios));
        listaAdyacencia[destino].append(Arista(origen, peso, puntosIntermedios)); // Grafo no dirigido
    }
}

QList<QString> Grafo::obtenerNodos() const {
    return listaAdyacencia.keys();
}

QList<Grafo::Arista> Grafo::obtenerAristas(const QString& nodo) const {
    return listaAdyacencia.value(nodo);
}

QPointF Grafo::obtenerPosicionNodo(const QString& nodo) const {
    return posicionesNodos.value(nodo);
}

bool Grafo::existeNodo(const QString& nodo) const {
    return listaAdyacencia.contains(nodo);
}

void Grafo::crearGrafoCiudad() {
    listaAdyacencia.clear();
    posicionesNodos.clear();

    // nodos
    agregarNodo("Lobby", QPointF(-400, 330));
    agregarNodo("Gasolinera", QPointF(100, 100));
    agregarNodo("Ciudad", QPointF(-220, 265));
    agregarNodo("Mall", QPointF(100, 265));
    agregarNodo("Supermercado", QPointF(450, 265));
    agregarNodo("Laboratorio", QPointF(700, 265));
    agregarNodo("Gimnasio", QPointF(100, 500));

    // las aristas con sus pesos y puntos intermedios
    agregarArista("Ciudad", "Lobby", 1);
    agregarArista("Gasolinera", "Ciudad", 2, {QPointF(-220, 100)});
    agregarArista("Ciudad", "Mall", 3);
    agregarArista("Mall", "Supermercado", 1);
    agregarArista("Supermercado", "Laboratorio", 2);
    agregarArista("Supermercado", "Gimnasio", 1, {QPointF(450, 500)});
    agregarArista("Supermercado", "Gasolinera", 1, {QPointF(450, 100)});
    agregarArista("Gimnasio", "Lobby", 2, {QPointF(-300, 500)});
    agregarArista("Gimnasio", "Mall", 2);
    agregarArista("Gasolinera", "Mall", 2);
}
