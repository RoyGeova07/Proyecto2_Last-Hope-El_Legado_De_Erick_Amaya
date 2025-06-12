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
#include <QVBoxLayout>

Mapa::Mapa(QWidget* parent) : QWidget(parent), jugador(nullptr) {
    this->setWindowTitle("Mapa - Last hope");

    // Crear layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Configurar escena y vista
    escena = new QGraphicsScene(this);
    vista = new QGraphicsView(escena, this);
    vista->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setStyleSheet("background: transparent; border: none;");

    // Después de crear la vista
    vista->setSceneRect(0, 0, 2000, 2000); // Ajusta según tu escena
    vista->fitInView(escena->sceneRect(), Qt::KeepAspectRatio);

    // O usa scale directamente
    vista->scale(0.6, 0.6); // 60% del tamaño original

    vista->setSceneRect(escena->itemsBoundingRect()); // Ajustar al contenido
    vista->fitInView(escena->itemsBoundingRect(), Qt::KeepAspectRatio);

    // O si prefieres un enfoque manual:
    vista->centerOn(escena->itemsBoundingRect().center());

    // Añadir vista al layout principal
    mainLayout->addWidget(vista);

    // Cargar imagen de fondo directamente en la escena
    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mapa.jpg");
    if(fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/Mapa.jpg";
    } else {
        // Añadir fondo como item de la escena
        QGraphicsPixmapItem *fondoItem = escena->addPixmap(fondoPixmap);
        fondoItem->setZValue(-1); // Para que esté detrás de todo
    }

    // Crear y configurar el grafo
    grafoMapa = new Grafo();
    grafoMapa->crearGrafoCiudad();

    visualizarGrafo(*grafoMapa);

    // Boton para volver al lobby
    btnVolver = new QPushButton(this);
    btnVolver->setText("⬅");
    btnVolver->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 40px; }");
    btnVolver->move(10, 10);
    btnVolver->raise(); // Para que esté encima de la vista

    connect(btnVolver, &QPushButton::clicked, this, [this]() {
        if(!jugador) {
            jugador = new personaje();
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
        }
        lobby* l = new lobby(jugador);
        l->show();
        this->deleteLater();
    });

    // Crear label para mostrar la distancia
    labelDistancia = new QLabel(this);
    labelDistancia->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 150); color: white; font-size: 16px; padding: 5px; border-radius: 5px; }");
    labelDistancia->setAlignment(Qt::AlignCenter);
    labelDistancia->setGeometry(width() - 200, 20, 180, 40);
    labelDistancia->setText("Distancia: 0 km");
    labelDistancia->raise(); // Para que esté encima de la vista
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

    // Dibujar nodos
    for (const QString& nodo : grafo.obtenerNodos()) {
        QPointF posicion = grafo.obtenerPosicionNodo(nodo);

        QGraphicsEllipseItem* nodoItem = new QGraphicsEllipseItem(
            //posicion.x() - 20, posicion.y() - 20, 40, 40);
            posicion.x() - 10, posicion.y() - 10, 20, 20);
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

    // Resetear distancia
    labelDistancia->setText("Distancia: 0 km");

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
    float distanciaTotal = 0.0f;

    for (int i = 0; i < ruta.size() - 1; i++) {
        QString nodoActual = ruta[i];
        QString nodoSiguiente = ruta[i + 1];

        // Buscar la arista entre estos nodos
        for (const Grafo::Arista& arista : grafoMapa->obtenerAristas(nodoActual)) {
            if (arista.destino == nodoSiguiente) {
                distanciaTotal += arista.peso;

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

    // Actualizar label con la distancia total
    labelDistancia->setText(QString("Distancia: %1 km").arg(distanciaTotal));
}

void Mapa::setModoCompacto(bool compacto, QGraphicsView* vistaPrincipal) {
    modoCompacto = compacto;
    this->vistaPrincipal = vistaPrincipal;

    vista->setFrameShape(compacto ? QFrame::NoFrame : QFrame::StyledPanel);
    btnVolver->setVisible(!compacto);
    labelDistancia->setVisible(!compacto);

    if (compacto) {
        cacheVista = QPixmap(size());
        cacheVista.fill(Qt::transparent);
        QPainter painter(&cacheVista);
        vista->render(&painter);
    }
}

QPixmap Mapa::obtenerVistaMinimizada() const {
    return cacheVista;
}

QSize Mapa::sizeHint() const {
    return modoCompacto ? QSize(400, 300) : QSize(1280, 720);
}
void Mapa::mousePressEvent(QMouseEvent* event) {
    if (modoCompacto) {
        QPointF scenePos = vista->mapToScene(event->pos());
        QGraphicsItem* item = escena->itemAt(scenePos, QTransform());

        if (item) {
            QString nodo = item->data(0).toString();
            if (!nodo.isEmpty()) {
                emit nodoSeleccionadoDesdeCompacto(nodo);
                return;
            }
        }
    }
    QWidget::mousePressEvent(event);
}

void Mapa::actualizarVistaCompacta() {
    if (modoCompacto) {
        qreal scaleFactor = qMin(
            width() / escena->sceneRect().width(),
            height() / escena->sceneRect().height()
            );
        vista->resetTransform();
        //vista->scale(scaleFactor, scaleFactor);
        vista->scale(0.8, 0.8);
        vista->centerOn(escena->sceneRect().center());

        cacheVista = QPixmap(size());
        cacheVista.fill(Qt::transparent);
        QPainter painter(&cacheVista);
        escena->render(&painter, QRectF(), escena->sceneRect());
    }
}
