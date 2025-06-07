// mapa.cpp
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

Mapa::Mapa(QWidget* parent) : QWidget(parent), jugador(nullptr) {
    this->resize(1280, 720);
    this->setWindowTitle("Mapa - Last hope");

    // Cargar imagen de fondo
    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mapa.jpeg");
    if(fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/Mapa.png";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }

    // Crear y configurar el grafo
    grafoMapa = new Grafo();
    grafoMapa->crearGrafoCiudad();

    // Configurar escena y vista
    escena = new QGraphicsScene(this);
    vista = new QGraphicsView(escena, this);
    vista->setGeometry(0, 0, width(), height());
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setStyleSheet("background: transparent; border: none;");

    visualizarGrafo(*grafoMapa);

    // Botón para volver al lobby (manteniendo tu código original)
    QPushButton *btnVolver = new QPushButton(this);
    btnVolver->setText("⬅");
    btnVolver->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 40px; }");
    btnVolver->move(10, 10);

    connect(btnVolver, &QPushButton::clicked, this, [this]() {
        if(!jugador) {
            jugador = new personaje();
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
        }
        lobby* l = new lobby(jugador);
        l->show();
        this->close();
        qDebug() << "Volviendo al lobby...!";
    });
}

void Mapa::visualizarGrafo(const Grafo& grafo) {
    escena->clear();

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
            QPen pen(Qt::gray);
            pen.setWidth(2);
            linea->setPen(pen);
            escena->addItem(linea);

            QPointF centro = camino.pointAtPercent(0.5);
            QGraphicsTextItem* textoPeso = new QGraphicsTextItem(QString::number(arista.peso));
            textoPeso->setPos(centro);
            textoPeso->setDefaultTextColor(Qt::gray);
            textoPeso->setFont(QFont("Arial", 10, QFont::Bold));
            escena->addItem(textoPeso);
        }
    }

    // Dibujar nodos y hacerlos clickeables
    for (const QString& nodo : grafo.obtenerNodos()) {
        QPointF posicion = grafo.obtenerPosicionNodo(nodo);

        QGraphicsEllipseItem* nodoItem = new QGraphicsEllipseItem(
            posicion.x() - 20, posicion.y() - 20, 40, 40);
        nodoItem->setBrush(QBrush(Qt::gray));
        nodoItem->setPen(QPen(Qt::black, 2));
        nodoItem->setData(0, nodo);
        nodoItem->setFlag(QGraphicsItem::ItemIsSelectable);
        escena->addItem(nodoItem);
    }

    connect(escena, &QGraphicsScene::selectionChanged, this, &Mapa::nodoSeleccionado);
}

void Mapa::nodoSeleccionado() {
    // Limpiar cualquier ruta previa
    for (auto item : rutaActual) {
        escena->removeItem(item);
    }
    rutaActual.clear();

    QList<QGraphicsItem*> items = escena->selectedItems();
    if (!items.isEmpty()) {
        QGraphicsEllipseItem* nodoItem = dynamic_cast<QGraphicsEllipseItem*>(items.first());
        if (nodoItem) {
            QString destino = nodoItem->data(0).toString();
            QString origen = "Lobby";

            if (destino != origen) {
                // Calcular ruta mas corta
                QList<QString> ruta = grafoMapa->dijkstra(origen, destino);

                if (!ruta.isEmpty()) {
                    dibujarRuta(ruta);
                }
            }
        }
    }
}

void Mapa::dibujarRuta(const QList<QString>& ruta) {
    QPen penRuta(Qt::yellow);
    penRuta.setWidth(4);

    for (int i = 0; i < ruta.size() - 1; i++) {
        QString nodoActual = ruta[i];
        QString nodoSiguiente = ruta[i + 1];

        // Buscar la arista entre estos nodos
        for (const Grafo::Arista& arista : grafoMapa->obtenerAristas(nodoActual)) {
            if (arista.destino == nodoSiguiente) {
                QPointF puntoInicio = grafoMapa->obtenerPosicionNodo(nodoActual);
                QPointF puntoFinal = grafoMapa->obtenerPosicionNodo(nodoSiguiente);

                QPainterPath camino;
                camino.moveTo(puntoInicio);

                for (const QPointF& punto : arista.puntosIntermedios) {
                    camino.lineTo(punto);
                }

                camino.lineTo(puntoFinal);

                QGraphicsPathItem* lineaRuta = new QGraphicsPathItem(camino);
                lineaRuta->setPen(penRuta);
                escena->addItem(lineaRuta);
                rutaActual.append(lineaRuta);
                break;
            }
        }

        // Resaltar nodos de la ruta
        QPointF posicion = grafoMapa->obtenerPosicionNodo(nodoActual);
        QGraphicsEllipseItem* nodoResaltado = new QGraphicsEllipseItem(
            posicion.x() - 25, posicion.y() - 25, 50, 50);
        nodoResaltado->setPen(QPen(Qt::yellow, 3));
        nodoResaltado->setBrush(Qt::NoBrush);
        escena->addItem(nodoResaltado);
        rutaActual.append(nodoResaltado);
    }

    // Resaltar el ultimo nodo (destino)
    if (!ruta.isEmpty()) {
        QPointF posicion = grafoMapa->obtenerPosicionNodo(ruta.last());
        QGraphicsEllipseItem* nodoResaltado = new QGraphicsEllipseItem(
            posicion.x() - 25, posicion.y() - 25, 50, 50);
        nodoResaltado->setPen(QPen(Qt::yellow, 3));
        nodoResaltado->setBrush(Qt::NoBrush);
        escena->addItem(nodoResaltado);
        rutaActual.append(nodoResaltado);
    }
}
