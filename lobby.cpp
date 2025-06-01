#include "lobby.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>

lobby::lobby(QWidget* parent) : EscenaBase(parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Lobby - Last hope");

    configurarEscena();
    inicializarJugador();
    configurarObstaculos();
    configurarNPCs();

    // QLabel para mostrar el diálogo
    lblDialogo = new QLabel(this);
    lblDialogo->setStyleSheet("background: black; color: white; padding: 15px; border-radius: 10px;");
    lblDialogo->setAlignment(Qt::AlignCenter);
    lblDialogo->setGeometry(100, 100, 400, 100);
    lblDialogo->hide();

    Movimientos();
}

void lobby::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/lobby.jpg");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/lobby.jpg";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void lobby::configurarObstaculos() {
    // OBSTÁCULOS FIJOS
    obstaculos.append(QRect(3, 2, 1334, 227));     // Muro superior
    obstaculos.append(QRect(44, 557, 234, 118));   // Mesa abajo izquierda
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(900, 570, 160, 150));  // Casilleros
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void lobby::configurarNPCs() {
    NPC* npc2 = new NPC(NPC::Tipo::NPC2, this);
    npc2->move(700, 250);
    npc2->show();
    npcs.append(npc2);
    obstaculos.append(QRect(750, 250, 40, 70)); // Obstáculo físico del NPC
}

void lobby::onMovimientoUpdate() {
    QRect rectJugador = jugador->geometry();
    bool hayNpcCerca = false;
    npcCercano = nullptr;

    for (NPC* npc : npcs) {
        QRect rectNPC = npc->geometry();
        QRect zonaProximidad = rectNPC.adjusted(-20, -20, 20, 20);

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
}

void lobby::keyPressEvent(QKeyEvent* event) {
    EscenaBase::keyPressEvent(event);

    if (event->key() == Qt::Key_H && npcCercano && !npcCercano->estaHablando()) {
        npcCercano->mostrarDialogo(lblDialogo);
    }
}
