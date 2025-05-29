#include "personaje.h"
#include<QDebug>

personaje::personaje(QWidget*parent):QLabel(parent),frameActual(0),velocidadMovimiento(10)
{

    this->resize(128,128);//aqui tamanio del frame
    this->move(100,500);//aqui posiciion inicial

    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&personaje::AvanzarFrame);

}

void personaje::SetAnimacion(const QString &ruta, int cantidadFrames)
{

    QPixmap spriteSheet(ruta);
    if(spriteSheet.isNull())
    {

        qDebug()<<"no se pudo cargar la animacion: "<<ruta;
        return;

    }
    frames.clear();
    frameActual=0;//con esto reinicia desde el primer frame

    int frameAncho=spriteSheet.width()/cantidadFrames;

    for(int i=0;i<cantidadFrames;++i)
    {

       QPixmap frame = spriteSheet.copy(i*frameAncho,0,frameAncho,spriteSheet.height());
        frames.append(frame);

    }

    setPixmap(frames[frameActual]);//con esto muestro el primer frame

    timer->start(100);//con esto inicia para que avance la primera animacion


}

void personaje::AvanzarFrame()
{

    if(frames.isEmpty())return;

    frameActual=(frameActual+1)%frames.size();
    setPixmap(frames[frameActual]);

}

void personaje::MoverIzquierda()
{

    this->move(x()-velocidadMovimiento,y());

}

void personaje::MoverDerecha()
{

    this->move(x()+velocidadMovimiento,y());

}
