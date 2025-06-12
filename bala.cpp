#include "bala.h"
#include"zombie.h"

Bala::Bala(QWidget*parent):QLabel(parent)
{

    this->resize(10,4);//forma rectangular
    this->setStyleSheet("background-color: orange; border-radius: 2px;");
    timer=new QTimer(this);

    connect(timer,&QTimer::timeout,this,&Bala::Mover);

}

void Bala::Disparar(bool haciaDerecha)
{

    direccionDerecha=haciaDerecha;
    this->show();
    timer->start(30);

}

//EXPLICAR ESTA FUNCION DESPUESSSZSZSZ
void Bala::Mover()
{

    int dx=direccionDerecha?velocidad:-velocidad;
    this->move(x()+dx,y());

    //revisa si colisiona con algun zombie
    QList<QLabel*>posibles=parentWidget()->findChildren<QLabel*>();

    for(QLabel*obj:posibles)
    {

        if(obj==this)continue;

        if(obj->inherits("Zombie")&&this->geometry().intersects(obj->geometry()))
        {

            Zombie*z=static_cast<Zombie*>(obj);
            if(z->muerto)continue;//ignorar si esta muerto

            z->recibirDanio(danio);

            timer->stop();         // Detener el timer
            this->hide();          // Ocultar la bala visualmente
            emit impactoBala(this); // Emitir señal para que el padre la elimine

            return;//termina aqui para evitar multiples colisiones

        }

    }

    //emitir señal si se va muy lejos
    //si se sale de la pantalla
    if(x()<0||x()>parentWidget()->width())
    {

        emit impactoBala(this);

    }

}
