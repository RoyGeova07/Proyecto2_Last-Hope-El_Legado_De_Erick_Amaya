#include "mall.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>

Mall::Mall(QWidget* parent) : AtributosPersonaje(parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Mall - Last hope");

    configurarEscena();
    inicializarJugador();
    configurarObstaculos();


    Movimientos();

    //aqui se crea y registrar los zombies
    Zombie* z1 = new Zombie(Zombie::Tipo::Z1, this);
    z1->move(1100,500);
    z1->show();
    zombies.append(z1); //aqui se registra en el vector global

    Zombie* z2 = new Zombie(Zombie::Tipo::Z2, this);
    z2->move(800,500);
    z2->show();
    zombies.append(z2); //aqui se registra en el vector global

    Zombie* z3 = new Zombie(Zombie::Tipo::Z3, this);
    z3->move(900,500);
    z3->show();
    zombies.append(z3); //aqui se registra en el vector global
}

void Mall::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mall.jpeg");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/War.png";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void Mall::configurarObstaculos() {
    // OBSTÁCULOS FIJOS
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}
