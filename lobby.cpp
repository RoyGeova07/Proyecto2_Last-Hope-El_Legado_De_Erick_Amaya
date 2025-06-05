#include "lobby.h"
#include "mapa.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include<QPainter>
#include <QPushButton>
#include"caminos.h"

lobby::lobby(personaje*jugadorExistente, QWidget* parent) : EscenaBase(jugadorExistente,parent)
{

    this->resize(1280, 720);
    this->setWindowTitle("Lobby - Last hope");

    configurarEscena();
    configurarObstaculos();

    jugador=jugadorExistente;
    jugador->setParent(this);
    jugador->show();
    jugador->raise();

    //aqui QLabel para mostrar los datos
    lblDatos = new QLabel(this);
    lblDatos->setStyleSheet("background: black; color: white; padding: 15px; border-radius: 10px;");
    lblDatos->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    lblDatos->setWordWrap(true);
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

    labelPresionarR=new QLabel("PRESIONE R PARA EXPLORAR",this);
    labelPresionarR->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 5px; border-radius: 5px;");
    labelPresionarR->setAlignment(Qt::AlignCenter);
    labelPresionarR->setWordWrap(true);
    labelPresionarR->setFixedSize(200,30);
    labelPresionarR->hide();//con este al inicio estara oculto

    // Boton para ver el mapa
    QPushButton *btnMapa = new QPushButton(this);
    QPixmap pixmap(":/imagenes/assets/items/btnMapa.png");
    btnMapa->setIcon(QIcon(pixmap));
    btnMapa->setIconSize(pixmap.size());
    btnMapa->setFixedSize(pixmap.size());
    btnMapa->setStyleSheet("QPushButton { border: white; background: transparent; }");
    btnMapa->setFocusPolicy(Qt::NoFocus); //Para que el focus este en el teclado + la ventana
    btnMapa->move(640,150);

    connect(btnMapa, &QPushButton::clicked, this, [this]() {
        Mapa* mapa = new Mapa();
        mapa->show();
        this->close();
        qDebug() << "Boton de mapa presionado";
        this->setFocus();
    });


    //aqui se crea y registrar los NPCs
    NPC* npc2 = new NPC(NPC::Tipo::NPC2, this);
    npc2->move(1100,390);
    npc2->show();
    npcs.append(npc2); //aqui se registra en el vector global

    //aqui obstaculo fisico del NPC
    obstaculos.append(QRect(1151,468,17,60));


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

    QRect zonaPuerta1(120, 280, 80, 120);

    if (rectJugador.intersects(zonaPuerta1))
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

    EscenaBase::keyPressEvent(event);

    //AQUI SE PROGRAMA LA ACCION DESPUES DE PRESIONAR UNA TECCLA EN ESPCIFICO DEL JUEGO
    if(event->key()==Qt::Key_H&&npcCercano&&!npcCercano->estaHablando())
    {

        npcCercano->mostrarDialogo(lblDialogo);

    }

    //si presiona R y esta cerca de la puerta
    if(event->key()==Qt::Key_R&&labelPresionarR->isVisible())
    {

        qDebug()<<"Explorar la puerta.";
        ResetearMovimiento();//para evitar errores

        Caminos*caminos=new Caminos(jugador);
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
