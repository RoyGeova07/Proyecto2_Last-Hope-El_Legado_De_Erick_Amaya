#include "escenabase.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>

EscenaBase::EscenaBase(QWidget* parent) : QWidget(parent),
    jugador(nullptr), movimientoTimer(nullptr),
    shiftPresionado(false), izquierdaPresionada(false),
    derechaPresionada(false), arribaPresionado(false),
    abajoPresionado(false), ZPresionado(false) {
}

EscenaBase::~EscenaBase() {
    delete jugador;
    delete movimientoTimer;
}

void EscenaBase::inicializarJugador() {
    jugador = new personaje(this);
    jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
    jugador->show();
    jugador->raise();
}

void EscenaBase::Movimientos() {
    movimientoTimer = new QTimer(this);
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {
        bool moviendo = false;

        if (ZPresionado) {
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Shot_1.png", 4);
            return;
        }

        if (izquierdaPresionada) {
            jugador->MoverSiNoColisiona(-jugador->getVelocidadMoviento(), 0, obstaculos);
            moviendo = true;
        } else if (derechaPresionada) {
            jugador->MoverSiNoColisiona(jugador->getVelocidadMoviento(), 0, obstaculos);
            moviendo = true;
        }

        if (arribaPresionado) {
            jugador->MoverSiNoColisiona(0, -jugador->getVelocidadMoviento(), obstaculos);
            moviendo = true;
        } else if (abajoPresionado) {
            jugador->MoverSiNoColisiona(0, jugador->getVelocidadMoviento(), obstaculos);
            moviendo = true;
        }

        if (moviendo) {
            if (shiftPresionado) {
                jugador->SetAnimacionMovimiento(6);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Run.png", 8);
            } else {
                jugador->SetAnimacionMovimiento(2);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Walk.png", 7);
            }
        } else {
            movimientoTimer->stop();
            jugador->DetenerAnimacion();
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
        }
        onMovimientoUpdate();
    });

    movimientoTimer->setInterval(30);
}

void EscenaBase::keyPressEvent(QKeyEvent* event) {
    if (!jugador) return;

    switch (event->key()) {
    case Qt::Key_Shift:
        shiftPresionado = true;
        break;
    case Qt::Key_Left:
        izquierdaPresionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Right:
        derechaPresionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Up:
        arribaPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Down:
        abajoPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Z:
        ZPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    }
}

void EscenaBase::keyReleaseEvent(QKeyEvent* event) {
    if (!jugador) return;

    switch (event->key()) {
    case Qt::Key_Shift:
        shiftPresionado = false;
        break;
    case Qt::Key_Left:
        izquierdaPresionada = false;
        break;
    case Qt::Key_Right:
        derechaPresionada = false;
        break;
    case Qt::Key_Up:
        arribaPresionado = false;
        break;
    case Qt::Key_Down:
        abajoPresionado = false;
        break;
    case Qt::Key_Z:
        ZPresionado = false;
        break;
    }
}
