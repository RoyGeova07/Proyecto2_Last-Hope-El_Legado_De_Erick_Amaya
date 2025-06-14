#ifndef GRAFO_H
#define GRAFO_H

#include <QString>
#include <QMap>
#include <QList>
#include <QPointF>
#include <QRectF>


class Grafo {
public:
    struct Arista {
        QString destino;
        int peso;
        QList<QPointF> puntosIntermedios;

        Arista(const QString& dest, int p, const QList<QPointF>& puntos = {})
            : destino(dest), peso(p), puntosIntermedios(puntos) {}
    };

    Grafo();


    void agregarNodo(const QString& nombre, const QPointF& posicion);
    void agregarArista(const QString& origen, const QString& destino, int peso,
                       const QList<QPointF>& puntosIntermedios = {});

    QList<QString> obtenerNodos() const;
    QList<Arista> obtenerAristas(const QString& nodo) const;
    QPointF obtenerPosicionNodo(const QString& nodo) const;
    bool existeNodo(const QString& nodo) const;
    QList<QString> dijkstra(const QString& origen, const QString& destino);
    QRectF obtenerRectanguloDelimitar() const;
    QList<QString> encontrarSegundaMejorRuta(const QString& origen, const QString& destino, const QList<QString>& rutaMasCorta);
    float calcularDistanciaRuta(const QList<QString>& ruta);

    void crearGrafoCiudad();

private:
    QMap<QString, QList<Arista>> listaAdyacencia;
    QMap<QString, QPointF> posicionesNodos;
};

#endif // GRAFO_H
