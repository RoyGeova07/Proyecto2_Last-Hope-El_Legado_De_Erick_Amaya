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

private slots:
    void nodoSeleccionado();
    void dibujarRuta(const QList<QString>& ruta);


private:
    void visualizarGrafo(const Grafo& grafo);
    personaje*jugador;
    Grafo* grafoMapa;
    QGraphicsScene* escena;
    QGraphicsView* vista;
    QList<QGraphicsItem*> rutaActual;

};

#endif // INICIO_H
