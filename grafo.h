// En Grafo.h
#include <QString>
#include <QMap>
#include <QList>
#include <QPointF>
#include <functional>

class Grafo {
public:
    struct Arista {
        QString destino;
        int peso;
        QList<QPointF> puntosIntermedios;

        Arista(const QString& dest, int p, const QList<QPointF>& puntos = {})
            : destino(dest), peso(p), puntosIntermedios(puntos) {}
    };

    // Tipo para las funciones que abren pestañas
    using FuncionAbrirPestana = std::function<void()>;

    Grafo();

    void agregarNodo(const QString& nombre, const QPointF& posicion, FuncionAbrirPestana funcion = nullptr);
    void agregarArista(const QString& origen, const QString& destino, int peso,
                       const QList<QPointF>& puntosIntermedios = {});

    QList<QString> obtenerNodos() const;
    QList<Arista> obtenerAristas(const QString& nodo) const;
    QPointF obtenerPosicionNodo(const QString& nodo) const;
    FuncionAbrirPestana obtenerFuncionNodo(const QString& nodo) const;
    bool existeNodo(const QString& nodo) const;

     void crearGrafoCiudad(std::function<void(QString)> manejadorClics = nullptr);

private:
    QMap<QString, QList<Arista>> listaAdyacencia;
    QMap<QString, QPointF> posicionesNodos;
    QMap<QString, FuncionAbrirPestana> funcionesNodos;
};
