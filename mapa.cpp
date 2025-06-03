#include "mapa.h"
#include <Ciudad.h>
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
    grafoCiudad.crearGrafoCiudad([this](QString nombreNodo) {
        abrirPestana(nombreNodo);
    });
    visualizarGrafo(grafoCiudad);

    // Boton para volver al lobby
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

void Mapa::abrirPestana(const QString& nombreNodo) {
    qDebug() << "Abriendo pestaña:" << nombreNodo;

    if (nombreNodo == "Lobby") {
        lobby* l = new lobby();
        l->show();
        this->close();
    }
    else if (nombreNodo == "Ciudad") {
        Ciudad* g = new Ciudad();
        g->show();
        this->close();
    }

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

        // Crear un circulo para el nodo
        QGraphicsEllipseItem* circulo = new QGraphicsEllipseItem(-15, -15, 30, 30);
        circulo->setPos(posicion);
        circulo->setBrush(Qt::gray);

        // Hacer el círculo interactivo
        circulo->setFlag(QGraphicsItem::ItemIsSelectable, true);
        circulo->setFlag(QGraphicsItem::ItemIsFocusable, true);
        circulo->setAcceptHoverEvents(true);
        circulo->setData(0, nodo);

        escena->addItem(circulo);
    }

    // Conectar la señal de click de la escena
    connect(escena, &QGraphicsScene::selectionChanged, this, [this, grafo, escena]() {
        QGraphicsItem* item = escena->selectedItems().value(0);
        if (item) {
            QString nombreNodo = item->data(0).toString();
            if (!nombreNodo.isEmpty()) {
                abrirPestana(nombreNodo);
            }
        }
    });
}


