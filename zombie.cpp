#include "zombie.h"
#include<QRandomGenerator>

Zombie::Zombie(Tipo tipo, QWidget* parent)
    : QLabel(parent),
    tipo(tipo),
    frameActual(0)
{
    this->setFixedSize(128, 128);
    this->move(600, 500);

    // ASIGNA VIDA SEGÚN TIPO:
    switch(tipo) {
    case Tipo::Z1:
    case Tipo::Z2:
    case Tipo::Z3:
        vida = vidaMaxima = 10;
        break;
    case Tipo::Z4:
        vida = vidaMaxima = 8;   // Z4: rápido
        break;
    case Tipo::Z5:
        vida = vidaMaxima = 10;  // Z5: intermedio
        break;
    case Tipo::Z6:
        vida = vidaMaxima = 14;  // Z6: lento
        break;
    }

    // Timers
    animacionTimer = new QTimer(this);
    connect(animacionTimer, &QTimer::timeout, this, &Zombie::AvanzarFrame);

    // Definicion por tipo de zombie
    switch(tipo) {
    case Tipo::Z1:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z1.png", 9, true);
        break;
    case Tipo::Z2:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z2.png", 8, true);
        break;
    case Tipo::Z3:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z3.png", 5, true);
        break;
    case Tipo::Z4:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z4.png", 6, true);
        break;
    case Tipo::Z5:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z5.png", 6, true);
        break;
    case Tipo::Z6:
        SetAnimacion(":/imagenes/assets/Zombies/Idle_Z6.png", 6, true);
        break;
    }

    inicializarBarraVida();
}

//AGREGO OTRO PARAMETRO PARA PODER CONTROLAR SI UNA ANIMACION DEBE REPETIRSE O NO
void Zombie::SetAnimacion(const QString& ruta, int cantidadFrames,bool loop)
{
    QPixmap spriteSheet(ruta);
    if(spriteSheet.isNull())
    {
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
    //Esto previene que animaciones como la de muerte (no loop) sean sobrescritas accidentalmente por otra
    //si el timer esta detenido.
    if(!loop)
    {

        animacionTimer->start(100);

    }else if(!animacionTimer->isActive()){

        animacionTimer->start(100);

    }

    //detectar si es animacion de ataque por su ruta
    estaAtacando=ruta.contains("Attack");
    danioEmitido=false;
    frameAtaqueFinal=cantidadFrames-1;

    //guardar si esta animacion debe repetirse
    animacionLoop=loop;

}

void Zombie::AvanzarFrame()
{
    if(frames.isEmpty()||frames.size()<=1) return;

    frameActual++;

    //si se llega al final de una animacion NO loop, mostrar el ultimo frame y detener el timer
    if(frameActual>=frames.size())
    {

        if(!animacionLoop)
        {

            frameActual=frames.size()-1;
            animacionTimer->stop();//detener la animacion de muerte
            setPixmap(frames[frameActual]);
            return;

        }

        frameActual=0;//reiniciar si es loop
    }

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
//Como funciona esta funcion?
//la clase personaje hereda de un Qlabel y QLabel hereda de QWidget
//Entonces se hace esto: zombie->perseguirJugador(jugador);
//es valido porque jugador (de tipo personaje*) tambien es un QWidget* gracias a la herencia.
//Qt permite tratar cualquier subclase como su clase base.
void Zombie::moverHaciaJugador()
{

    if(!objetivo||muerto)return;

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
    actualizarBarraVida();

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

    case Tipo::Z4:

        ruta=":/imagenes/assets/Zombies/Walk_Z4.png";
        frames=10;
        break;

    case Tipo::Z5:

        ruta=":/imagenes/assets/Zombies/Walk_Z5.png";
        frames=10;
        break;

    case Tipo::Z6:

        ruta=":/imagenes/assets/Zombies/Walk_Z6.png";
        frames=10;
        break;


    }

    SetAnimacion(ruta,frames,true);

}

void Zombie::realizarAtaque()
{

    if(atacando||muerto||!objetivo)return;

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

    case Tipo::Z4:
        rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z4.png";
        cantidadFrames = 5;
        break;

    case Tipo::Z5:
        rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z5.png";
        cantidadFrames = 5;
        break;

    case Tipo::Z6:
        rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z6.png";
        cantidadFrames = 4;
        break;


    }
    SetAnimacion(rutaAtaque,cantidadFrames,true);

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

void Zombie::inicializarBarraVida()
{

    barraFondo=new QLabel(this->parentWidget());
    barraFondo->setStyleSheet("background-color: gray; border: 1px solid black");
    barraFondo->setFixedSize(50,6);

    barraVida=new QLabel(barraFondo);
    barraVida->setStyleSheet("background-color: red");
    barraVida->setGeometry(0,0,50,6);

    barraFondo->show();
    barraVida->show();

}

void Zombie::actualizarBarraVida()
{

    int ancho=(static_cast<float>(vida)/vidaMaxima)*barraFondo->width();
    barraVida->setFixedWidth(ancho);

    //posiciona la barra de encima del zombie
    QPoint posZombie=this->pos();
    barraFondo->move(posZombie.x()+(width()-barraFondo->width())/2,posZombie.y()-15);
    barraFondo->raise();

}

void Zombie::recibirDanio(int cantidad)
{

    if(muerto)return;//si ya esta muerto, ignorar el daño

    vida-=cantidad;
    if(vida<0)vida=0;

    actualizarBarraVida();

    if(vida==0&&!muerto)
    {

        muerto=true;//se marca como muerto
        if(movimientoTimer)
        {

            movimientoTimer->stop();//detener el movimiento
            movimientoTimer->disconnect();

        }

        //aqui se muestra la animacion de muerte segun el tipo de zombie
        QString rutaMuerte;
        int framesMuerte=5;

        switch(tipo)
        {

            case Tipo::Z1: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z1.png";break;
            case Tipo::Z2: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z2.png";break;
            case Tipo::Z3: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z3.png";break;
            case Tipo::Z4: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z4.png";break;
            case Tipo::Z5: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z5.png";break;
            case Tipo::Z6: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z6.png";break;

        }

        //aqui reproduce animacion de muerte una unica vez sin interrupciones
        SetAnimacion(rutaMuerte,framesMuerte,false);

        //aqui ocultar barra de vida despues de animacion de muerte
        QTimer::singleShot(800,this,[=](){

           barraFondo->hide();
           this->raise();// asegura que el zombie muerto se muestre encima

        });

        return;//no hacer animacion de herido

    }

    //si sigue vivo, mostrar animacion de herido
    setAnimacionHerido();

}

void Zombie::setAnimacionHerido()
{

    QString ruta;
    int frames=0;

    switch (tipo) {
    case Tipo::Z1:

        ruta=":/imagenes/assets/Zombies/Hurt_Z1.png";
        frames=5;
        break;

    case Tipo::Z2:

        ruta=":/imagenes/assets/Zombies/Hurt_Z2.png";
        frames=3;
        break;

    case Tipo::Z3:

        ruta=":/imagenes/assets/Zombies/Hurt_Z3.png";
        frames=3;
        break;

    case Tipo::Z4:

        ruta=":/imagenes/assets/Zombies/Hurt_Z4.png";
        frames=4;
        break;

    case Tipo::Z5:

        ruta=":/imagenes/assets/Zombies/Hurt_Z5.png";
        frames=4;
        break;

    case Tipo::Z6:

        ruta=":/imagenes/assets/Zombies/Hurt_Z6.png";
        frames=4;
        break;


    }

    //aqui se muestra la animacion de herido
    SetAnimacion(ruta,frames,true);

    //despues de 300ms, vuelve al movimiento
    QTimer::singleShot(300,this,[this](){

        if(vida>0)SetAnimacionMovimientoZombie();

    });

}
