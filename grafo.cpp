#include "Grafo.h"
#include "Cola.h"
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <queue>


Grafo::Grafo() {}

void Grafo::agregarNodo(const QString& nombre, const QPointF& posicion) {
    if (!listaAdyacencia.contains(nombre)) {
        listaAdyacencia[nombre] = QList<Arista>();
        posicionesNodos[nombre] = posicion;
    }
}

void Grafo::agregarArista(const QString& origen, const QString& destino, int peso,
                          const QList<QPointF>& puntosIntermedios) {
    if (listaAdyacencia.contains(origen) && listaAdyacencia.contains(destino))
    {

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

void Grafo::crearGrafoCiudad()
{
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
    agregarArista("Ciudad", "Lobby", 10);
    agregarArista("Gasolinera", "Ciudad", 20, {QPointF(-220, 100)});
    agregarArista("Ciudad","Mall", 15);
    agregarArista("Mall","Supermercado",15);
    agregarArista("Supermercado","Laboratorio",10);
    agregarArista("Supermercado","Gimnasio",20,{QPointF(450, 500)});
    agregarArista("Supermercado","Gasolinera", 25, {QPointF(450, 100)});
    agregarArista("Gimnasio","Lobby",25,{QPointF(-300,500)});
    agregarArista("Gimnasio","Mall",5);
    agregarArista("Gasolinera", "Mall", 5);
}

QList<QString> Grafo::dijkstra(const QString& origen, const QString& destino) {
    //  almacenar distancias y predecesores
    std::unordered_map<QString, float> distancias;
    std::unordered_map<QString, QString> predecesores;

    // inicializar todas las distancias como infinito
    for (const auto& nodo : listaAdyacencia.keys()) {
        distancias[nodo] = std::numeric_limits<float>::infinity();
    }
    distancias[origen] = 0.0f;

    // Cola de prioridad usando un comparador personalizado
    auto comparar = [](const std::pair<QString, float>& a, const std::pair<QString, float>& b) {
        return a.second > b.second;
    };
    Cola<float> cola;
    cola.insertar(0.0f, origen);

    while (!cola.colaVacia()) {
        auto elemento = cola.desencolar();
        QString actual = elemento.nivel;
        float distanciaActual = elemento.dato;

        if (actual == destino) break;

        for (const auto& arista : listaAdyacencia[actual]) {
            float nuevaDistancia = distancias[actual] + arista.peso;

            if (nuevaDistancia < distancias[arista.destino]) {
                distancias[arista.destino] = nuevaDistancia;
                predecesores[arista.destino] = actual;
                cola.insertar(nuevaDistancia, arista.destino);
            }
        }
    }

    // reconstruir la ruta
    QList<QString> ruta;
    QString nodo = destino;
    while (nodo != "") {
        ruta.prepend(nodo);
        nodo = predecesores.count(nodo) ? predecesores[nodo] : "";
    }

    if (ruta.first() != origen) {
        return QList<QString>(); // retorna lista vacía si no hay ruta
    }

    return ruta;
}

QRectF Grafo::obtenerRectanguloDelimitar() const {
    QRectF rect;
    for (const QString& nodo : obtenerNodos()) {
        QPointF pos = obtenerPosicionNodo(nodo);
        rect |= QRectF(pos - QPointF(30, 30), QSizeF(60, 60));
    }
    return rect;
}
