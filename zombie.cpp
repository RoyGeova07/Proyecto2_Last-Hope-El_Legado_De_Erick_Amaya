#include "zombie.h"

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
        frame=frame.transformed(QTransform().scale(-1,1));//volteo horizontallll
        frames.append(frame.scaled(this->size(), Qt::KeepAspectRatio));

    }

    if (!frames.isEmpty()) {
        setPixmap(frames.first());
        animacionTimer->start(100);
    }
}

void Zombie::AvanzarFrame()
{
    if(frames.isEmpty()||frames.size()<=1) return;

    frameActual=(frameActual+1)%frames.size();
    setPixmap(frames[frameActual]);
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

