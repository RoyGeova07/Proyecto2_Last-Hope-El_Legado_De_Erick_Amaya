#include "Grafo.h"

Grafo::Grafo() {}

void Grafo::agregarNodo(const QString& nombre, const QPointF& posicion, FuncionAbrirPestana funcion) {
    if (!listaAdyacencia.contains(nombre)) {
        listaAdyacencia[nombre] = QList<Arista>();
        posicionesNodos[nombre] = posicion;
        funcionesNodos[nombre] = funcion;
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

Grafo::FuncionAbrirPestana Grafo::obtenerFuncionNodo(const QString& nodo) const {
    return funcionesNodos.value(nodo);
}

void Grafo::crearGrafoCiudad(std::function<void(QString)> manejadorClics) {
    listaAdyacencia.clear();
    posicionesNodos.clear();
    funcionesNodos.clear();

    // Agregar nodos con sus posiciones y funciones
    agregarNodo("Lobby", QPointF(-400, 330), [manejadorClics]() { if(manejadorClics) manejadorClics("Lobby"); });
    agregarNodo("Gasolinera", QPointF(120, 100), [manejadorClics]() { if(manejadorClics) manejadorClics("Gasolinera"); });
    agregarNodo("Ciudad", QPointF(-180, 265), [manejadorClics]() { if(manejadorClics) manejadorClics("Ciudad"); });
    agregarNodo("Mall", QPointF(120, 265), [manejadorClics]() { if(manejadorClics) manejadorClics("Mall"); });
    agregarNodo("Supermercado", QPointF(450, 265), [manejadorClics]() { if(manejadorClics) manejadorClics("Supermercado"); });
    agregarNodo("Laboratorio", QPointF(700, 265), [manejadorClics]() { if(manejadorClics) manejadorClics("Laboratorio"); });
    agregarNodo("Gimnasio", QPointF(120, 500), [manejadorClics]() { if(manejadorClics) manejadorClics("Gimnasio"); });

    // las aristas con sus pesos y puntos intermedios
    agregarArista("Ciudad", "Lobby", 1);
    agregarArista("Gasolinera", "Ciudad", 2, {QPointF(-180, 100)});
    agregarArista("Ciudad", "Mall", 3);
    agregarArista("Mall", "Supermercado", 1);
    agregarArista("Supermercado", "Laboratorio", 2);
    agregarArista("Supermercado", "Gimnasio", 1, {QPointF(450, 500)});
    agregarArista("Supermercado", "Gasolinera", 1, {QPointF(450, 100)});
    agregarArista("Gimnasio", "Lobby", 2, {QPointF(-300, 500)});
    agregarArista("Gimnasio", "Mall", 2);
    agregarArista("Gasolinera", "Mall", 2, {QPointF(120, 150)});
}
