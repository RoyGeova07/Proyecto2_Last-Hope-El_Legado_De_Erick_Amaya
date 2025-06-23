#include "zombie.h"
#include <QRandomGenerator>
#include <QDebug>

// Devuelve el centro de la hitbox real del zombie
QPoint Zombie::centroHitbox() const {
    QRect rect = hitbox();
    return QPoint(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
}

Zombie::Zombie(Tipo tipo, QWidget* parent)
    : QLabel(parent),
    tipo(tipo),
    frameActual(0)
{
    this->setFixedSize(128, 128);
    this->move(600, 500);

    switch(tipo) {
    case Tipo::Z1: vida=vidaMaxima=24; break;
    case Tipo::Z2: vida=vidaMaxima=25; break;
    case Tipo::Z3: vida = vidaMaxima = 13; break;
    case Tipo::Z4: vida = vidaMaxima = 15; break;
    case Tipo::Z5: vida = vidaMaxima = 17; break;
    case Tipo::Z6: vida = vidaMaxima = 25; break;
    case Tipo::Z7: vida = vidaMaxima = 40; break;
    case Tipo::BOSS:
        vida = vidaMaxima = 250;
        this->setFixedSize(380, 380);
        break;
    }

    animacionTimer = new QTimer(this);
    connect(animacionTimer, &QTimer::timeout, this, &Zombie::AvanzarFrame);

    switch(tipo) {
    case Tipo::Z1: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z1.png", 9, true); break;
    case Tipo::Z2: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z2.png", 8, true); break;
    case Tipo::Z3: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z3.png", 5, true); break;
    case Tipo::Z4: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z4.png", 6, true); break;
    case Tipo::Z5: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z5.png", 6, true); break;
    case Tipo::Z6: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z6.png", 6, true); break;
    case Tipo::Z7: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z6.png", 6, true); break;
    case Tipo::BOSS: SetAnimacion(":/imagenes/assets/Zombies/Idle_Z4.png", 6, true); break;
    }

    inicializarBarraVida();
}

void Zombie::SetAnimacion(const QString& ruta, int cantidadFrames, bool loop)
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
    QSize tamFinal = this->size();
    if (tipo == Tipo::Z7) { tamFinal = QSize(200, 200); this->setFixedSize(tamFinal);}
    if (tipo == Tipo::BOSS) { tamFinal = QSize(380, 380); this->setFixedSize(tamFinal);}
    for (int i = 0; i < cantidadFrames; ++i) {
        QPixmap frame = spriteSheet.copy(i*frameAncho,0,frameAncho,spriteSheet.height());
        if(!mirandoALaDerecha) frame = frame.transformed(QTransform().scale(-1,1));
        frames.append(frame.scaled(tamFinal, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    setPixmap(frames.first());
    animacionTimer->start(100);
    if(!loop) { animacionTimer->start(100); }
    else if(!animacionTimer->isActive()) { animacionTimer->start(100); }
    estaAtacando = ruta.contains("Attack");
    danioEmitido = false;
    frameAtaqueFinal = cantidadFrames-1;
    animacionLoop = loop;
}

void Zombie::AvanzarFrame()
{
    if(frames.isEmpty()||frames.size()<=1) return;
    frameActual++;
    if(frameActual>=frames.size()) {
        if(!animacionLoop) {
            frameActual=frames.size()-1;
            animacionTimer->stop();
            if (tipo == Tipo::Z7) this->setFixedSize(200, 200);
            if (tipo == Tipo::BOSS) this->setFixedSize(380, 380);
            setPixmap(frames[frameActual]);
            return;
        }
        frameActual=0;
    }
    if (tipo == Tipo::Z7) this->setFixedSize(200, 200);
    if (tipo == Tipo::BOSS) this->setFixedSize(380, 380);
    setPixmap(frames[frameActual]);

    if(estaAtacando && frameActual==frameAtaqueFinal && !danioEmitido)
    {
        if(this->hitbox().intersects(objetivo->geometry()))
        {
            emit ColisionConJugador();
        }
        danioEmitido=true;
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
    posicionLabel.setY(posicionLabel.y()-35);
    labelPresionaA->move(posicionLabel);
    labelPresionaA->show();
    labelPresionaA->raise();
}

void Zombie::ocultarHintInteractuar()
{
    if (labelPresionaA) labelPresionaA->hide();
}

void Zombie::perseguirJugador(QWidget *objetivo)
{
    this->objetivo=objetivo;
    SetAnimacionMovimientoZombie();
    if(!movimientoTimer)
    {
        movimientoTimer=new QTimer(this);
        connect(movimientoTimer,&QTimer::timeout,this,&Zombie::moverHaciaJugador);
        movimientoTimer->start(50);
    }
}

void Zombie::moverHaciaJugador()
{
    if (!objetivo || muerto) return;

    // Usar el centro del hitbox para la persecución:
    QPoint centroZombie = this->centroHitbox();
    QPoint centroJugador = objetivo->geometry().center();

    int distanciaX = std::abs(centroJugador.x() - centroZombie.x());
    int distanciaY = std::abs(centroJugador.y() - centroZombie.y());

    if (distanciaX < 30 && distanciaY < 30) {
        realizarAtaque();
        return;
    }

    int dx = 0, dy = 0;
    if (centroJugador.x() > centroZombie.x()) dx = velocidad;
    if (centroJugador.x() < centroZombie.x()) dx = -velocidad;
    if (centroJugador.y() > centroZombie.y()) dy = velocidad;
    if (centroJugador.y() < centroZombie.y()) dy = -velocidad;

    bool nuevaDireccionDerecha = centroJugador.x() > centroZombie.x();
    if (nuevaDireccionDerecha != mirandoALaDerecha && !atacando) {
        mirandoALaDerecha = nuevaDireccionDerecha;
        SetAnimacionMovimientoZombie();
    }

    // Limitar el zombie a no salirse del mapa
    int nuevaX = x() + dx;
    int nuevaY = y() + dy;
    int mapaAlto = 720;
    int margenSuelo = 10;
    int maxY = mapaAlto - this->height() - margenSuelo;
    if (nuevaY > maxY) nuevaY = maxY;
    if (nuevaY < 0) nuevaY = 0;
    this->move(nuevaX, nuevaY);
    actualizarBarraVida();
}

void Zombie::SetAnimacionMovimientoZombie()
{
    if(muerto)return;
    QString ruta;
    int frames=0;
    switch (tipo)
    {
    case Tipo::Z1: ruta=":/imagenes/assets/Zombies/Run_Z1.png"; frames=8; break;
    case Tipo::Z2: ruta=":/imagenes/assets/Zombies/Run_Z2.png"; frames=7; break;
    case Tipo::Z3: ruta=":/imagenes/assets/Zombies/Run_Z3.png"; frames=7; break;
    case Tipo::Z4: ruta=":/imagenes/assets/Zombies/Walk_Z4.png"; frames=10; break;
    case Tipo::Z5: ruta=":/imagenes/assets/Zombies/Walk_Z5.png"; frames=10; break;
    case Tipo::Z6: ruta=":/imagenes/assets/Zombies/Walk_Z6.png"; frames=10; break;
    case Tipo::Z7: ruta=":/imagenes/assets/Zombies/Walk_Z6.png"; frames=10; break;
    case Tipo::BOSS: ruta=":/imagenes/assets/Zombies/Walk_Z4.png"; frames=10; break;
    }
    SetAnimacion(ruta,frames,true);
}

void Zombie::realizarAtaque()
{
    if(atacando||muerto||!objetivo)return;

    atacando=true;
    int ataqueRandom=QRandomGenerator::global()->bounded(1,3);
    QString rutaAtaque;
    int cantidadFrames=4;
    switch (tipo)
    {
    case Tipo::Z1: rutaAtaque=ataqueRandom==1?":/imagenes/assets/Zombies/Attack_1_Z1.png":":/imagenes/assets/Zombies/Attack_2_Z1.png"; break;
    case Tipo::Z2:
        if(ataqueRandom==1) { rutaAtaque=":/imagenes/assets/Zombies/Attack_1_Z2.png"; cantidadFrames=5; }
        else { rutaAtaque=":/imagenes/assets/Zombies/Attack_2_Z2.png"; cantidadFrames=4; }
        break;
    case Tipo::Z3: rutaAtaque=ataqueRandom==1?":/imagenes/assets/Zombies/Attack_1_Z3.png":":/imagenes/assets/Zombies/Attack_2_Z3.png"; break;
    case Tipo::Z4: rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z4.png"; cantidadFrames = 5; break;
    case Tipo::Z5: rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z5.png"; cantidadFrames = 5; break;
    case Tipo::Z6: rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z6.png"; cantidadFrames = 4; break;
    case Tipo::Z7: rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z6.png"; cantidadFrames = 4; break;
    case Tipo::BOSS: rutaAtaque = ":/imagenes/assets/Zombies/Attack_Z4.png"; cantidadFrames = 5; break;
    }
    SetAnimacion(rutaAtaque,cantidadFrames,true);
    if(this->hitbox().intersects(objetivo->geometry())) { emit ColisionConJugador(); }
    QTimer::singleShot(600,this,[this](){ atacando=false; SetAnimacionMovimientoZombie(); });
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
    int ancho = (static_cast<float>(vida) / vidaMaxima) * barraFondo->width();
    barraVida->setFixedWidth(ancho);
    int offset = 0;
    QPoint posZombie = this->pos();
    barraFondo->move(
        posZombie.x() + (width() - barraFondo->width()) / 2,
        posZombie.y() + offset
        );
    barraFondo->raise();
}

QRect Zombie::hitbox() const {
    if (tipo == Tipo::BOSS) {
        int w = width() * 0.65;
        int h = height() * 0.75;
        int xOffset = x() + (width() - w) / 2;
        int yOffset = y() + height() * 0.22;
        return QRect(xOffset, yOffset, w, h);
    }
    return geometry();
}

void Zombie::recibirDanio(int cantidad)
{
    if(muerto)return;
    vida-=cantidad;
    if(vida<0)vida=0;
    actualizarBarraVida();
    if(vida==0&&!muerto)
    {
        muerto=true;
        if(movimientoTimer)
        {
            movimientoTimer->stop();
            movimientoTimer->disconnect();
            movimientoTimer->deleteLater();
            movimientoTimer=nullptr;
        }
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
        case Tipo::Z7: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z6.png";break;
        case Tipo::BOSS: rutaMuerte=":/imagenes/assets/Zombies/Dead_Z4.png";break;
        }
        SetAnimacion(rutaMuerte,framesMuerte,false);
        QTimer::singleShot(800,this,[=](){ barraFondo->hide(); this->raise(); });
        return;
    }
    setAnimacionHerido();
}

void Zombie::setAnimacionHerido()
{
    QString ruta;
    int frames=0;
    switch (tipo) {
    case Tipo::Z1: ruta=":/imagenes/assets/Zombies/Hurt_Z1.png"; frames=5; break;
    case Tipo::Z2: ruta=":/imagenes/assets/Zombies/Hurt_Z2.png"; frames=3; break;
    case Tipo::Z3: ruta=":/imagenes/assets/Zombies/Hurt_Z3.png"; frames=3; break;
    case Tipo::Z4: ruta=":/imagenes/assets/Zombies/Hurt_Z4.png"; frames=4; break;
    case Tipo::Z5: ruta=":/imagenes/assets/Zombies/Hurt_Z5.png"; frames=4; break;
    case Tipo::Z6: ruta=":/imagenes/assets/Zombies/Hurt_Z6.png"; frames=4; break;
    case Tipo::Z7: ruta=":/imagenes/assets/Zombies/Hurt_Z6.png"; frames=4; break;
    case Tipo::BOSS: ruta=":/imagenes/assets/Zombies/Hurt_Z4.png"; frames=4; break;
    }
    SetAnimacion(ruta,frames,true);
    QTimer::singleShot(300,this,[this](){ if(vida>0)SetAnimacionMovimientoZombie(); });
}

void Zombie::pausar()
{
    if(animacionTimer->isActive())animacionTimer->stop();
    if(movimientoTimer&&movimientoTimer->isActive()) movimientoTimer->stop();
}

void Zombie::reanudar()
{
    if(muerto)return;
    if(!animacionTimer->isActive())animacionTimer->start(100);
    if(movimientoTimer&&!movimientoTimer->isActive()) movimientoTimer->start(50);
}
