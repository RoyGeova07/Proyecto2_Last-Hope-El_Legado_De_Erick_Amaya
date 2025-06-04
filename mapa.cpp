#include "mapa.h"
#include <lobby.h>
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>

Mapa::Mapa(QWidget* parent) : QWidget(parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Mapa - Last hope");

    // Cargar imagen de fondo
    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mapa.png");
    if(fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/Mapa.png";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }

    // Crear y configurar el grafo
    Grafo grafoCiudad;
    grafoCiudad.crearGrafoCiudad();
    visualizarGrafo(grafoCiudad);

    // Botón para volver al lobby
    QPushButton *btnVolver = new QPushButton(this);
    btnVolver->setText("⬅");
    btnVolver->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 40px; }");
    btnVolver->move(10, 10);

    connect(btnVolver, &QPushButton::clicked, this, [this]() {
        lobby* l = new lobby();
        l->show();
        this->close();
        qDebug() << "Volviendo al lobby...!";
    });
}

void Mapa::visualizarGrafo(const Grafo& grafo) {
    QGraphicsScene* escena = new QGraphicsScene(this);
    QGraphicsView* vista = new QGraphicsView(escena, this);
    vista->setGeometry(0, 0, width(), height());
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setStyleSheet("background: transparent; border: none;");

    // Dibujar aristas
    for (const QString& nodoOrigen : grafo.obtenerNodos()) {
        for (const Grafo::Arista& arista : grafo.obtenerAristas(nodoOrigen)) {
            QPointF puntoInicio = grafo.obtenerPosicionNodo(nodoOrigen);
            QPointF puntoFinal = grafo.obtenerPosicionNodo(arista.destino);

            QPainterPath camino;
            camino.moveTo(puntoInicio);

            for (const QPointF& punto : arista.puntosIntermedios) {
                camino.lineTo(punto);
            }

            camino.lineTo(puntoFinal);

            QGraphicsPathItem* linea = new QGraphicsPathItem(camino);
            QPen pen(Qt::yellow);
            pen.setWidth(2);
            linea->setPen(pen);
            escena->addItem(linea);

            QPointF centro = camino.pointAtPercent(0.5);
            QGraphicsTextItem* textoPeso = new QGraphicsTextItem(QString::number(arista.peso));
            textoPeso->setPos(centro);
            textoPeso->setDefaultTextColor(Qt::yellow);
            textoPeso->setFont(QFont("Arial", 10, QFont::Bold));
            escena->addItem(textoPeso);
        }
    }

    // Dibujar nodos
    for (const QString& nodo : grafo.obtenerNodos()) {
        QPointF posicion = grafo.obtenerPosicionNodo(nodo);

        QGraphicsEllipseItem* nodoItem = new QGraphicsEllipseItem(
            posicion.x() - 20, posicion.y() - 20, 40, 40);
        nodoItem->setBrush(QBrush(Qt::gray));
        nodoItem->setPen(QPen(Qt::black, 2));
        escena->addItem(nodoItem);
    }
}
