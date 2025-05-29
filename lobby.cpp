#include "lobby.h"
#include<QPixmap>
#include<QLabel>
#include<QKeyEvent>

lobby::lobby(QWidget*parent):QWidget(parent)
{

    this->resize(1280,720);
    this->setWindowTitle("Lobby - Last hope");

    QPixmap fondoPixmap("assets/mapas/lobby.jpg");
    if (fondoPixmap.isNull())
    {

        qDebug()<<"Error al cargar imagen desde assets/mapas/lobby.jpg";

    }else{

        QLabel*fondo=new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0,0,width(),height());

    }

    jugador=new personaje(this);
    jugador->SetAnimacion("assets/protagonista/Idle.png",7);//aqui idle perfecto
    jugador->show();

}

void lobby::keyPressEvent(QKeyEvent *event)
{

    if(!jugador)return;

    if(event->key()==Qt::Key_Left)
    {

        jugador->MoverIzquierda();
        jugador->SetAnimacion("assets/protagonista/Walk.png",7);

    }else if(event->key()==Qt::Key_Right){

        jugador->MoverDerecha();
        jugador->SetAnimacion("assets/protagonista/Walk.png",7);

    }else if(event->key()==Qt::Key_Shift){

        jugador->SetAnimacion("assets/protagonista/Run.png",8);

    }else if(event->key()==Qt::Key_Space){

        jugador->SetAnimacion("assets/protagonista/Idle.png",7);

    }

}
