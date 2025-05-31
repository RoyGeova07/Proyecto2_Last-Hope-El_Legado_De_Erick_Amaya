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
    jugador->raise();//con esto se aseguara que este encima del fondo

    shiftPresionado=false;
    izquierdaPresionada=false;
    derechaPresionada=false;
    arribaPresionado=false;
    abajoPresionado=false;

    //aqui se crear el timer de movimiento
    movimientoTimer=new QTimer(this);
    connect(movimientoTimer,&QTimer::timeout,this,[=](){

        bool moviendo=false;


        if(izquierdaPresionada)
        {

            jugador->MoverSiNoColisiona(-jugador->getVelocidadMoviento(),0,obstaculos);
            moviendo=true;

        }else if(derechaPresionada){

            jugador->MoverSiNoColisiona(jugador->getVelocidadMoviento(),0,obstaculos);
            moviendo=true;

        }

        if(arribaPresionado)
        {

            jugador->MoverSiNoColisiona(0,-jugador->getVelocidadMoviento(),obstaculos);
            moviendo=true;

        }else if(abajoPresionado){

            jugador->MoverSiNoColisiona(0,jugador->getVelocidadMoviento(),obstaculos);
            moviendo=true;

        }

        if(moviendo)
        {
            //aqui se maneja el movimiento del personaje
            if(shiftPresionado)
            {

                jugador->SetAnimacionMovimiento(6);
                jugador->SetAnimacion("assets/protagonista/Run.png",8);

            }else{

                jugador->SetAnimacionMovimiento(2);
                jugador->SetAnimacion("assets/protagonista/Walk.png", 7);

            }

        }else{

            // Si no se mueve, detener
            movimientoTimer->stop();
            jugador->DetenerAnimacion();
            jugador->SetAnimacion("assets/protagonista/Idle.png", 7);

        }

    });

    movimientoTimer->setInterval(30);//33fps

    // Muro de arriba (cuevas + muro superior)
    //                      x,y,ancho,alto
    obstaculos.append(QRect(3,2,1334,227));

    // Mesa de abajo izquierda
    //                      x,  y, ancho,alto
    obstaculos.append(QRect(44,557,234,118));

    // Cubierta toda la parte de abajo (parte inferior)
    //                      x,  y, ancho,alto
    obstaculos.append(QRect(5,669,1273,47));

    // Casilleros (abajo derecha)
    //                      x,  y, ancho,alto
    obstaculos.append(QRect(900,570,160,150));

    //Lado izquierdo
    //                      x, y,ancho,alto
    obstaculos.append(QRect(3,278,5,388));

    //Lado derecho
    //                      x, y,ancho,alto
    obstaculos.append(QRect(1272,282,3,324));

}

void lobby::keyPressEvent(QKeyEvent *event)
{

    if(!jugador)return;

    if(event->key()==Qt::Key_Shift)
    {

        shiftPresionado=true;

    }

    if(event->key()==Qt::Key_Left)
    {

        izquierdaPresionada=true;
        if(!movimientoTimer->isActive())
            movimientoTimer->start();

    }else if(event->key()==Qt::Key_Right){

        derechaPresionada=true;

        if(!movimientoTimer->isActive())
            movimientoTimer->start();

    }else if(event->key()==Qt::Key_Up){

        arribaPresionado=true;
        if(!movimientoTimer->isActive())
            movimientoTimer->start();

    }else if(event->key()==Qt::Key_Down){

        abajoPresionado=true;
        if(!movimientoTimer->isActive())
            movimientoTimer->start();

    }

}

void lobby::keyReleaseEvent(QKeyEvent *event)
{

    if(!jugador)return;

    if(event->key()==Qt::Key_Shift)
    {

        shiftPresionado=false;

    }else if(event->key()==Qt::Key_Left){

        izquierdaPresionada=false;

    }else if(event->key()==Qt::Key_Right){

        derechaPresionada=false;

    }else if(event->key()==Qt::Key_Up){

        arribaPresionado=false;

    }else if(event->key()==Qt::Key_Down){

        abajoPresionado=false;

    }


}
