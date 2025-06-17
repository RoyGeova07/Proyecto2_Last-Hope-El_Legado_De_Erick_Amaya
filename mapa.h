#ifndef MAPA_H
#define MAPA_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QLabel>
#include "grafo.h"
#include "personaje.h"

class QPushButton;
class QLabel;

class Mapa : public QWidget
{
    Q_OBJECT

public:
    explicit Mapa(QWidget *parent = nullptr);
    void setModoCompacto(bool compacto, QGraphicsView* vistaPrincipal = nullptr);
    QPixmap obtenerVistaMinimizada() const;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* event);
    void actualizarVistaCompacta();
    QLabel* getDistanciaLabel() const { return labelDistancia; }
    void setDistanciaText(const QString& text);  // Nuevo método
    void procesarDistancias(float distanciaPrincipal, float distanciaAlternativa);

protected:

private slots:
    void nodoSeleccionado();

signals:
    void nodoSeleccionadoDesdeCompacto(const QString& nombreNodo);
    void nodoOrigenSeleccionado(const QString& nombreNodo);
    //void actualizarDistancias(float principal, float alternativa);
    void actualizarDistancias(float principal, float alterna, const QString& origen, const QString& destino);
    void distanciaActualizada(const QString& textoDistancia);

private:
    void visualizarGrafo(const Grafo& grafo);
    float dibujarRuta(const QList<QString>& ruta, const QColor& color, QList<QGraphicsItem*>& itemsList, bool esPrincipal);

    QLabel* distanciaLabel;

    QPushButton *btnVolver;
    personaje* jugador;
    Grafo* grafoMapa;
    QGraphicsScene* escena;
    QGraphicsView* vista;
    QList<QGraphicsItem*> rutaActual;
    QList<QGraphicsItem*> segundaRuta;
    QLabel* labelDistancia;
    bool modoCompacto = false;
    QGraphicsView* vistaPrincipal = nullptr;
    QPixmap cacheVista;

    QString origenSeleccionado;
    QString destSeleccionado;
};

#endif // MAPA_H
