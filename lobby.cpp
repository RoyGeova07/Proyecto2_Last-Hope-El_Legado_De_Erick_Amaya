#include "lobby.h"
#include "mapa.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include<QPainter>
#include <QPushButton>
#include"caminos.h"

lobby::lobby(personaje* jugadorExistente, QWidget* parent)
    : AtributosPersonaje(jugadorExistente, parent)
{
    this->resize(1280, 720);
    this->setWindowTitle("Lobby - Last hope");

    configurarEscena();
    configurarObstaculos();

    jugador = jugadorExistente;
    jugador->setParent(this);
    jugador->show();
    jugador->raise();

    // dialogo NPC
    dialogoNPC = new DialogoNPC(this);
    dialogoNPC->hide();
    barraVidaLabel->raise();

    // Label para el hint de interaccion con puerta
    labelPresionarR = new QLabel("PRESIONE R PARA EXPLORAR", this);
    labelPresionarR->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 5px; border-radius: 5px;");
    labelPresionarR->setAlignment(Qt::AlignCenter);
    labelPresionarR->setWordWrap(true);
    labelPresionarR->setFixedSize(200,30);
    labelPresionarR->hide();

    // Boton para ver el mapa
    QPushButton *btnMapa = new QPushButton(this);
    QPixmap pixmap(":/imagenes/assets/items/btnMapa.png");
    btnMapa->setIcon(QIcon(pixmap));
    btnMapa->setIconSize(pixmap.size());
    btnMapa->setFixedSize(pixmap.size());
    btnMapa->setStyleSheet("QPushButton { border: white; background: transparent; }");
    btnMapa->setFocusPolicy(Qt::NoFocus);
    btnMapa->move(640,150);

    // registro de NPCs
    NPC* npc2 = new NPC(NPC::Tipo::NPC2, this);
    npc2->move(1100, 390);
    npc2->show();
    npcs.append(npc2);
    npc2->setInventario(Inventario::getInstance());

    // Obstaculos npc
    obstaculos.append(QRect(1151, 468, 17, 60));

    Movimientos();
}

void lobby::configurarEscena()
{

    QPixmap fondoPixmap(":/imagenes/assets/mapas/lobby.jpg");
    if(fondoPixmap.isNull())
    {

        qDebug()<<"Error al cargar imagen desde assets/mapas/lobby.jpg";

    }else{

        QLabel*fondo=new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0,0,width(),height());

    }

}

void lobby::configurarObstaculos()
{

    // OBSTACULOS FIJOS
    obstaculos.append(QRect(3,2,1334,227));     // Muro superior
    obstaculos.append(QRect(44,557,234,118));   // Mesa abajo izquierda
    obstaculos.append(QRect(5,669,1273,47));    // Piso inferior
    obstaculos.append(QRect(900,570,160,150));  // Casilleros
    obstaculos.append(QRect(1,278,3,274));      // Pared izquierda
    obstaculos.append(QRect(1312,282,3,324));   // Pared derecha

}


void lobby::onMovimientoUpdate()
{

    ActualizarBarraVida();

    QRect rectJugador = jugador->geometry();
    bool hayNpcCerca = false;
    npcCercano = nullptr;

    for(NPC* npc : npcs)
    {
        QRect rectNPC = npc->geometry();
        QRect zonaProximidad = rectNPC.adjusted(-20, -20, 20, 20);

        if(rectJugador.intersects(zonaProximidad))
        {
            npc->mostrarHintInteractuar();
            npcCercano = npc;
            hayNpcCerca = true;
        }
        else
        {
            npc->ocultarHintInteractuar();
        }
    }

    if(!hayNpcCerca)
    {
        npcCercano = nullptr;
    }

    QRect zonaPuerta1(120, 280, 80, 120);
    if(rectJugador.intersects(zonaPuerta1))
    {
        mostrarHintPuerta();
        hayPuertaCerca = true;
    }
    else
    {
        ocultarHintPuerta();
        hayPuertaCerca = false;
    }
}

void lobby::keyPressEvent(QKeyEvent* event)
{
    AtributosPersonaje::keyPressEvent(event);

    // Interaccion con NPC
    if(event->key() == Qt::Key_H && npcCercano && !npcCercano->estaHablando())
    {
        npcCercano->mostrarDialogo(dialogoNPC);

        connect(npcCercano, &NPC::dialogoTerminado, this, [this]() {
            qDebug() << "Diálogo con NPC terminado";

            // Resetear las flags de movimiento
            shiftPresionado = false;
            izquierdaPresionada = false;
            derechaPresionada = false;
            arribaPresionado = false;
            abajoPresionado = false;
            ZPresionado = false;

            // Detener animacion
            if (jugador) {
                jugador->DetenerAnimacion();
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
            }

            //reactivar movimiento del personaje despues de terminar la conversacion
            if(movimientoTimer&&!movimientoTimer->isActive())
                movimientoTimer->start();

            this->activateWindow();
            this->setFocus();

        });
    }

    // Interaccion con puerta
    if(event->key() == Qt::Key_R && labelPresionarR->isVisible())
    {
        qDebug() << "Explorar la puerta.";
        ResetearMovimiento();
        Caminos* caminos = new Caminos(jugador);
        caminos->show();
        this->close();
    }
}

void lobby::mostrarHintPuerta()
{
    if (!labelPresionarR->isVisible())
    {
        QRect zonaPuerta1(120, 280, 80, 120);
        labelPresionarR->move(zonaPuerta1.center().x() - labelPresionarR->width() / 2,
                              zonaPuerta1.top() - labelPresionarR->height() - 10);

        labelPresionarR->show();
        labelPresionarR->raise();
    }
}

void lobby::ocultarHintPuerta()
{
    labelPresionarR->hide();
}

void lobby::posicionarJugadorEnPuerta()
{
    jugador->move(120, 280); // misma zona de la puerta
}
