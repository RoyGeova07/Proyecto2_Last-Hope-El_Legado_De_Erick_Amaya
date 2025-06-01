#include "Nivel01.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>

Nivel01::Nivel01(QWidget* parent) : EscenaBase(parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Nivel 1 - Last hope");

    configurarEscena();
    inicializarJugador();
    configurarObstaculos();

    Movimientos();
}

void Nivel01::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/War.png");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/War.png";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void Nivel01::configurarObstaculos() {
    // OBSTÁCULOS FIJOS
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}
