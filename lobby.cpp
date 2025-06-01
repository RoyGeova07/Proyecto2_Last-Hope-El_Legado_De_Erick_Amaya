#include "lobby.h"
#include <QPixmap>
#include <QLabel>
#include <QKeyEvent>
#include <QDebug>

lobby::lobby(QWidget* parent) : QWidget(parent)
{
    this->resize(1280, 720);
    this->setWindowTitle("Lobby - Last hope");

    QPixmap fondoPixmap(":/imagenes/assets/mapas/lobby.jpg");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/lobby.jpg";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }

    jugador = new personaje(this);
    jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
    jugador->show();
    jugador->raise();

    shiftPresionado = false;
    izquierdaPresionada = false;
    derechaPresionada = false;
    arribaPresionado = false;
    abajoPresionado = false;

    // QLabel para mostrar el diálogo
    lblDialogo = new QLabel(this);
    lblDialogo->setStyleSheet("background: black; color: white; padding: 15px; border-radius: 10px;");
    lblDialogo->setAlignment(Qt::AlignCenter);
    lblDialogo->setGeometry(100, 100, 400, 100);
    lblDialogo->hide();

    // Crear y registrar NPCs
    NPC* npc2 = new NPC(NPC::Tipo::NPC2, this);
    npc2->move(700, 250);
    npc2->show();
    npcs.append(npc2); // Registrar en el vector global

    // Obstáculo físico del NPC
    obstaculos.append(QRect(750, 250, 40, 70));

    // Crear timer de movimiento
    movimientoTimer = new QTimer(this);
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {
        bool moviendo = false;

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

        // Detección de proximidad con NPCs
        QRect rectJugador = jugador->geometry();
        bool hayNpcCerca = false;
        npcCercano = nullptr;

        for (NPC* npc : npcs) {
            QRect rectNPC = npc->geometry();
            QRect zonaProximidad = rectNPC.adjusted(-20, -20, 20, 20); // margen de 20px

            if (rectJugador.intersects(zonaProximidad)) {
                npc->mostrarHintInteractuar();
                npcCercano = npc;
                hayNpcCerca = true;
            } else {
                npc->ocultarHintInteractuar();
            }
        }

        if (!hayNpcCerca) {
            npcCercano = nullptr;
        }
    });

    movimientoTimer->setInterval(30); // ~33 FPS

    // OBSTÁCULOS FIJOS
    obstaculos.append(QRect(3, 2, 1334, 227));     // Muro superior
    obstaculos.append(QRect(44, 557, 234, 118));   // Mesa abajo izquierda
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(900, 570, 160, 150));  // Casilleros
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void lobby::keyPressEvent(QKeyEvent* event)
{
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
    case Qt::Key_H:
        if (npcCercano && !npcCercano->estaHablando()) {
            npcCercano->mostrarDialogo(lblDialogo);
        }
        break;
    }
}

void lobby::keyReleaseEvent(QKeyEvent* event)
{
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
    }
}
