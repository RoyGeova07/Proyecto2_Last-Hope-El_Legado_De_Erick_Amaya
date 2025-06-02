#include "lobby.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include<QPainter>

lobby::lobby(QWidget* parent) : EscenaBase(parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Lobby - Last hope");

    configurarEscena();
    inicializarJugador();
    configurarObstaculos();

    //aqui QLabel para mostrar el dialogo
    lblDatos = new QLabel(this);
    lblDatos->setStyleSheet("background: black; color: white; padding: 15px; border-radius: 10px;");
    lblDatos->setAlignment(Qt::AlignLeft|Qt::AlignTop); //con este mejora alineacion mejor para textos largo
    lblDatos->setWordWrap(true); // Para que haga salto de linea automatico
    lblDatos->setGeometry(10, 10, 120, 70);
    lblDatos->raise();
    lblDatos->show();

    //Actualizar los datos del jugador
    auto actualizarUI = [=]() {
        QMap<QString, int> datos = jugador->cargarDatosJugador();
        QString texto = QString("Energia: %1\nMuniciones: %2")
                            .arg(datos["Energia"])
                            .arg(datos["Municiones"]);
        lblDatos->setText(texto);
    };
    actualizarUI();

    //aqui QLabel para mostrar el dialogo
    lblDialogo = new QLabel(this);
    lblDialogo->setStyleSheet("background: black; color: white; padding: 15px; border-radius: 10px;");
    lblDialogo->setAlignment(Qt::AlignLeft|Qt::AlignTop); //con este mejora alineacion mejor para textos largo
    lblDialogo->setWordWrap(true); // Para que haga salto de linea automatico
    lblDialogo->setGeometry(100, 100, 600, 150);
    lblDialogo->hide();

    //aqui se crea y registrar los NPCs
    NPC* npc2 = new NPC(NPC::Tipo::NPC2, this);
    npc2->move(1100,390);
    npc2->show();
    npcs.append(npc2); //aqui se registra en el vector global

    //aqui obstaculo fisico del NPC
    obstaculos.append(QRect(1151,468,17,60));

    //aqui se creaa timer de movimiento
    movimientoTimer = new QTimer(this);
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {
        bool moviendo = false;

        if (izquierdaPresionada)
        {

            jugador->MoverSiNoColisiona(-jugador->getVelocidadMoviento(),0,obstaculos);
            moviendo=true;

        }else if (derechaPresionada)
        {

            jugador->MoverSiNoColisiona(jugador->getVelocidadMoviento(),0,obstaculos);
            moviendo=true;

        }

        if(arribaPresionado)
        {

            jugador->MoverSiNoColisiona(0,-jugador->getVelocidadMoviento(),obstaculos);
            moviendo=true;

        } else if (abajoPresionado) {
            jugador->MoverSiNoColisiona(0, jugador->getVelocidadMoviento(), obstaculos);
            moviendo = true;
        }

        if(moviendo)
        {

            if(shiftPresionado)
            {

                jugador->SetAnimacionMovimiento(6);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Run.png", 8);

            }else{

                jugador->SetAnimacionMovimiento(2);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Walk.png", 7);

            }
        }else{

            movimientoTimer->stop();
            jugador->DetenerAnimacion();
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);

        }

        //aqui deteccion de proximidad con NPCs
        QRect rectJugador=jugador->geometry();
        bool hayNpcCerca=false;
        npcCercano=nullptr;

        for (NPC*npc:npcs)
        {

            QRect rectNPC=npc->geometry();
            QRect zonaProximidad=rectNPC.adjusted(-20,-20,20,20); // margen de 20px

            if(rectJugador.intersects(zonaProximidad))
            {

                npc->mostrarHintInteractuar();
                npcCercano=npc;
                hayNpcCerca=true;

            }else{

                npc->ocultarHintInteractuar();

            }
        }

        if(!hayNpcCerca)
        {

            npcCercano=nullptr;

        }
    });

    movimientoTimer->setInterval(30); // ~33 FPS


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

    QRect rectJugador=jugador->geometry();
    bool hayNpcCerca=false;
    npcCercano=nullptr;

    for(NPC* npc : npcs)
    {

        QRect rectNPC=npc->geometry();
        QRect zonaProximidad=rectNPC.adjusted(-20,-20,20,20);

        if(rectJugador.intersects(zonaProximidad))
        {

            npc->mostrarHintInteractuar();
            npcCercano=npc;
            hayNpcCerca=true;

        }else{

            npc->ocultarHintInteractuar();

        }
    }

    if(!hayNpcCerca)
    {

        npcCercano=nullptr;

    }
}

void lobby::keyPressEvent(QKeyEvent* event)
{

    EscenaBase::keyPressEvent(event);

    if(event->key()==Qt::Key_H&&npcCercano&&!npcCercano->estaHablando())
    {

        npcCercano->mostrarDialogo(lblDialogo);

    }

}
