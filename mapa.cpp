//mapa.cpp
#include "mapa.h"
#include <lobby.h>
#include <QPixmap>
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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Configuración de escena y vista
    escena = new QGraphicsScene(this);
    vista = new QGraphicsView(escena, this);
    vista->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setStyleSheet("background: transparent; border: none;");

    // Ajustes de vista
    vista->setSceneRect(0, 0, 2000, 2000);
    vista->fitInView(escena->sceneRect(), Qt::KeepAspectRatio);
    vista->scale(0.6, 0.6);
    vista->setSceneRect(escena->itemsBoundingRect());
    vista->fitInView(escena->itemsBoundingRect(), Qt::KeepAspectRatio);
    vista->centerOn(escena->itemsBoundingRect().center());

    mainLayout->addWidget(vista);

    // Carga de fondo
    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mapa.jpg");
    if(fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/Mapa.jpg";
    } else {
        QGraphicsPixmapItem *fondoItem = escena->addPixmap(fondoPixmap);
        fondoItem->setZValue(-1);
    }

    // Inicializacion del grafo
    grafoMapa = new Grafo();
    grafoMapa->crearGrafoCiudad();
    visualizarGrafo(*grafoMapa);

    // Botón de volver
    btnVolver = new QPushButton(this);
    btnVolver->setText("⬅");
    btnVolver->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 40px; }");
    btnVolver->move(10, 10);
    btnVolver->raise();

    connect(btnVolver, &QPushButton::clicked, this, [this]() {
        if(!jugador) {
            jugador = new personaje();
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
        }
        lobby* l = new lobby(jugador);
        l->show();
        this->deleteLater();
    });

    labelDistancia = new QLabel(vista);
    labelDistancia->setStyleSheet("/* mismo estilo */");
    labelDistancia->setGeometry(20, 20, 180, 50);
    labelDistancia->setText("Distancias");
    labelDistancia->show();
    labelDistancia->raise();
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
    // Limpiar rutas anteriores
    for (auto item : rutaActual) {
        escena->removeItem(item);
    }
    for (auto item : segundaRuta) {
        escena->removeItem(item);
    }
    rutaActual.clear();
    segundaRuta.clear();
    labelDistancia->setText("Distancia: 0 km");

    QList<QGraphicsItem*> items = escena->selectedItems();
    if (!items.isEmpty()) {
        QGraphicsEllipseItem* nodoItem = dynamic_cast<QGraphicsEllipseItem*>(items.first());
        if (nodoItem) {
            destSeleccionado = nodoItem->data(0).toString();

            // Si no hay origen seleccionado, este nodo será el origen
            if (origenSeleccionado.isEmpty()) {
                origenSeleccionado = destSeleccionado;
                // Puedes cambiar el color del nodo origen para distinguirlo
                nodoItem->setBrush(Qt::gray);
                emit nodoOrigenSeleccionado(origenSeleccionado);
            }
            // Si ya hay origen seleccionado, este nodo será el destino
            else if (destSeleccionado != origenSeleccionado) {
                QString destino = destSeleccionado;
                QString origen = origenSeleccionado;

                QList<QString> rutaCorta = grafoMapa->dijkstra(origen, destino);
                QList<QString> rutaAlternativa = grafoMapa->encontrarSegundaMejorRuta(origen, destino, rutaCorta);

                float distanciaPrincipal = 0.0f;
                float distanciaAlternativa = 0.0f;

                if (!rutaAlternativa.isEmpty()) {
                    distanciaAlternativa = dibujarRuta(rutaAlternativa, Qt::blue, segundaRuta, false);
                }

                if (!rutaCorta.isEmpty()) {
                    distanciaPrincipal = dibujarRuta(rutaCorta, Qt::yellow, rutaActual, true);
                }

                emit actualizarDistancias(distanciaPrincipal, distanciaAlternativa, origenSeleccionado, destSeleccionado);
                procesarDistancias(distanciaPrincipal, distanciaAlternativa);

                // Resetear la selección para la próxima vez
                origenSeleccionado.clear();
            }
            else {
                origenSeleccionado.clear();
                nodoItem->setBrush(Qt::red);
            }
        }
    }
}


float Mapa::dibujarRuta(const QList<QString>& ruta, const QColor& color, QList<QGraphicsItem*>& itemsList, bool esPrincipal) {
    QPen penRuta(color);
    penRuta.setWidth(esPrincipal ? 4 : 3);
    float distanciaTotal = 0.0f;

    for (int i = 0; i < ruta.size() - 1; i++) {
        QString nodoActual = ruta[i];
        QString nodoSiguiente = ruta[i + 1];

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
                lineaRuta->setZValue(esPrincipal ? 1 : 0);
                escena->addItem(lineaRuta);
                itemsList.append(lineaRuta);
                break;
            }
        }

        QPointF posicion = grafoMapa->obtenerPosicionNodo(nodoActual);
        QGraphicsEllipseItem* nodoResaltado = new QGraphicsEllipseItem(
            posicion.x() - (esPrincipal ? 25 : 22),
            posicion.y() - (esPrincipal ? 25 : 22),
            esPrincipal ? 50 : 44,
            esPrincipal ? 50 : 44);
        nodoResaltado->setPen(QPen(color, esPrincipal ? 3 : 2));
        nodoResaltado->setBrush(Qt::NoBrush);
        nodoResaltado->setZValue(esPrincipal ? 1 : 0);
        escena->addItem(nodoResaltado);
        itemsList.append(nodoResaltado);
    }

    if (!ruta.isEmpty()) {
        QPointF posicion = grafoMapa->obtenerPosicionNodo(ruta.last());
        QGraphicsEllipseItem* nodoResaltado = new QGraphicsEllipseItem(
            posicion.x() - (esPrincipal ? 25 : 22),
            posicion.y() - (esPrincipal ? 25 : 22),
            esPrincipal ? 50 : 44,
            esPrincipal ? 50 : 44);
        nodoResaltado->setPen(QPen(color, esPrincipal ? 3 : 2));
        nodoResaltado->setBrush(Qt::NoBrush);
        nodoResaltado->setZValue(esPrincipal ? 1 : 0);
        escena->addItem(nodoResaltado);
        itemsList.append(nodoResaltado);
    }

    return distanciaTotal;
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
        vista->scale(0.8, 0.8);
        vista->centerOn(escena->sceneRect().center());

        cacheVista = QPixmap(size());
        cacheVista.fill(Qt::transparent);
        QPainter painter(&cacheVista);
        escena->render(&painter, QRectF(), escena->sceneRect());
    }
}


void Mapa::procesarDistancias(float distanciaPrincipal, float distanciaAlternativa) {
    emit actualizarDistancias(distanciaPrincipal, distanciaAlternativa,origenSeleccionado, destSeleccionado);
}

void Mapa::setDistanciaText(const QString& text) {
    if (labelDistancia) {
        labelDistancia->setText(text);
        labelDistancia->setStyleSheet("background: rgba(0,0,0,150); color: white; border-radius: 5px;");
    }
}
