#ifndef MAPA_H
#define MAPA_H
#include<QWidget>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include "grafo.h"
#include"personaje.h"


class QPushButton;
class QLabel;

class Mapa:public QWidget
{

    Q_OBJECT

public:
    explicit Mapa(QWidget *parent=nullptr);
    void setModoCompacto(bool compacto, QGraphicsView* vistaPrincipal = nullptr);
    QPixmap obtenerVistaMinimizada() const;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* event);
    void actualizarVistaCompacta();
protected:


private slots:
    void nodoSeleccionado();
    void dibujarRuta(const QList<QString>& ruta);

signals:
    void nodoSeleccionadoDesdeCompacto(const QString& nombreNodo);


private:
    void visualizarGrafo(const Grafo& grafo);
    QPushButton *btnVolver;
    personaje*jugador;
    Grafo* grafoMapa;
    QGraphicsScene* escena;
    QGraphicsView* vista;
    QList<QGraphicsItem*> rutaActual;


    QLabel* labelDistancia;
    bool modoCompacto = false;
    QGraphicsView* vistaPrincipal = nullptr;
    QPixmap cacheVista;

};

#endif // INICIO_H
