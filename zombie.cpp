#include "zombie.h"
#include<QRandomGenerator>

Zombie::Zombie(Tipo tipo, QWidget* parent)
    : QLabel(parent),
    tipo(tipo),
    frameActual(0)
{
    this->setFixedSize(128, 128);
    this->move(600, 500);

    // Timers
    animacionTimer = new QTimer(this);
    connect(animacionTimer, &QTimer::timeout, this, &Zombie::AvanzarFrame);

    // Definición por tipo de zombie
    switch(tipo) {
    case Tipo::Z1:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z1.png", 9);
        break;
    case Tipo::Z2:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z2.png", 8);
        break;
    case Tipo::Z3:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z3.png", 5);
        break;
    }
}

void Zombie::SetAnimacion(const QString& ruta, int cantidadFrames)
{
    QPixmap spriteSheet(ruta);
    if (spriteSheet.isNull()) {
        qDebug() << "Error al cargar animacion:" << ruta;
        return;
    }

    frames.clear();
    frameActual = 0;

    int frameAncho = spriteSheet.width() / cantidadFrames;

    for(int i=0;i<cantidadFrames;++i)
    {

        QPixmap frame = spriteSheet.copy(i*frameAncho,0,frameAncho,spriteSheet.height());

        //si no esta volteando a la derecha
        if(!mirandoALaDerecha)
        {

            frame=frame.transformed(QTransform().scale(-1,1));//volteo horizontallll

        }
        //el smoth me ayuda a mejorar la calidad visual (mas nitido, menos pixelado).
        frames.append(frame.scaled(this->size(), Qt::KeepAspectRatio,Qt::SmoothTransformation));

    }

    setPixmap(frames.first());
    animacionTimer->start(100);

    //detectar si es animacion de ataque por su ruta
    estaAtacando=ruta.contains("Attack");
    danioEmitido=false;
    frameAtaqueFinal=cantidadFrames-1;

}

void Zombie::AvanzarFrame()
{
    if(frames.isEmpty()||frames.size()<=1) return;

    frameActual=(frameActual+1)%frames.size();
    setPixmap(frames[frameActual]);

    //si es animacion de ataque y estamos en el ultimo frame
    if(estaAtacando&&frameActual==frameAtaqueFinal&&!danioEmitido)
    {

        if(this->geometry().intersects(objetivo->geometry()))
        {

            emit ColisionConJugador();

        }
        danioEmitido=true;//evita q lo haga mas de una vez por ataque

    }

}


void Zombie::mostrarHintInteractuar()
{
    if (!labelPresionaA) {
        labelPresionaA = new QLabel("Presiona A para atacar", this->parentWidget());
        labelPresionaA->setStyleSheet("background: rgba(0, 0, 0, 180); color: white; padding: 5px; border-radius: 5px;");
        labelPresionaA->setAlignment(Qt::AlignCenter);
        labelPresionaA->setWordWrap(true);
        labelPresionaA->setFixedSize(150, 30);
    }

    QPoint posicionLabel=this->pos();
    posicionLabel.setY(posicionLabel.y()-35);// Posicion sobre el zombie
    labelPresionaA->move(posicionLabel);
    labelPresionaA->show();
    labelPresionaA->raise();
}

void Zombie::ocultarHintInteractuar()
{

    if (labelPresionaA)
        labelPresionaA->hide();

}

void Zombie::perseguirJugador(QWidget *objetivo)
{

    this->objetivo=objetivo;

    SetAnimacionMovimientoZombie();//con esta funcion cambiara el momiviento a la hora de perseguir el jugador

    if(!movimientoTimer)
    {

        movimientoTimer=new QTimer(this);
        connect(movimientoTimer,&QTimer::timeout,this,&Zombie::moverHaciaJugador);
        movimientoTimer->start(50);

    }

}

void Zombie::moverHaciaJugador()
{

    if(!objetivo)return;

    //aqui calculamos la distancia
    int distanciaX=std::abs(objetivo->x()-this->x());
    int distanciaY=std::abs(objetivo->y()-this->y());

    //si esta muy cerca, atcarrrr
    if(distanciaX<20&&distanciaY<20)
    {

        realizarAtaque();
        return;

    }

    //perseguir al jugador
    int dx=0,dy=0;
    if(objetivo->x()>this->x()) dx=velocidad;
    if(objetivo->x()<this->x())dx=-velocidad;
    if(objetivo->y()>this->y())dy=velocidad;
    if(objetivo->y()<this->y())dy=-velocidad;

    //con este booleano determina la direccion del jugador respecto al zombie
    bool nuevaDireccionDerecha=objetivo->x()>this->x();

    //si cambia de direccion, actualizar el sprite
    if(nuevaDireccionDerecha!=mirandoALaDerecha&&!atacando)
    {

        mirandoALaDerecha=nuevaDireccionDerecha;
        SetAnimacionMovimientoZombie();//recarga animacion con flip adecuado

    }

    this->move(x()+dx,y()+dy);

}

void Zombie::SetAnimacionMovimientoZombie()
{

    QString ruta;
    int frames=0;

    switch (tipo)
    {

    case Tipo::Z1:

        ruta=":/imagenes/assets/Zombies/Run_Z1.png";
        frames=8;
        break;

    case Tipo::Z2:

        ruta=":/imagenes/assets/Zombies/Run_Z2.png";
        frames=7;
        break;

    case Tipo::Z3:

        ruta=":/imagenes/assets/Zombies/Run_Z3.png";
        frames=7;
        break;

    }

    SetAnimacion(ruta,frames);

}

void Zombie::realizarAtaque()
{

    if(atacando||!objetivo)return;

    atacando=true;

    //con esto elige una animacion de ataque aleatoria, entre el ataque 1 o 2
    int ataqueRandom=QRandomGenerator::global()->bounded(1,3);//1 o 2
    QString rutaAtaque;
    int cantidadFrames=4;

    switch (tipo)
    {
    case Tipo::Z1:

        rutaAtaque=ataqueRandom==1?":/imagenes/assets/Zombies/Attack_1_Z1.png":":/imagenes/assets/Zombies/Attack_2_Z1.png";
        break;

    case Tipo::Z2:
        if(ataqueRandom==1)
        {

            rutaAtaque=":/imagenes/assets/Zombies/Attack_1_Z2.png";
            cantidadFrames=5; // ESTE tiene 5 sprites
        }else{

            rutaAtaque=":/imagenes/assets/Zombies/Attack_2_Z2.png";
            cantidadFrames=4;

        }
        break;

    case Tipo::Z3:

        rutaAtaque=ataqueRandom==1?":/imagenes/assets/Zombies/Attack_1_Z3.png":":/imagenes/assets/Zombies/Attack_2_Z3.png";
        break;

    }

    SetAnimacion(rutaAtaque,cantidadFrames);

    //emitir danio solo una vez por ataque
    if(this->geometry().intersects(objetivo->geometry()))
    {

        emit ColisionConJugador();

    }

    //que vuelva a correr despues de 600ms
    QTimer::singleShot(600,this,[this](){

        atacando=false;
        SetAnimacionMovimientoZombie();

    });

}
