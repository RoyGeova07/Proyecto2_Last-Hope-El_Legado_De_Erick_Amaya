#include "granada.h"
#include <QTimer>
#include <QDebug>
#include"personaje.h"
#include"AtributosPersonaje.h"
#include<QMessageBox>
#include"inicio.h"

Granada::Granada(QWidget* parent) : QLabel(parent), velocidad(10), distanciaRecorrida(0), maxDistancia(300)
{
    setFixedSize(200,200);

    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Granada::mover);

    // ---------- preparar frames de explosion ----------
    QPixmap sprite(":/imagenes/assets/protagonista/Explosion.png");
    const int totalFrames = 9;
    const int frameW = sprite.width() / totalFrames;
    for (int i = 0; i < totalFrames; ++i)
    {
        framesExplosion.append(sprite.copy(i * frameW, 0, frameW, sprite.height()).scaled(200,200,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void Granada::lanzar(bool haciaDerecha)
{
    direccionDerecha = haciaDerecha;
    posYInicial = y();
    tiempoVuelo = 0;
    velocidadY = -15; // opcional: puedes ajustarla por personaje o distancia

    show();
    raise();

    timerMovimiento->start(30);

    QTimer::singleShot(600, this, [this]() {
        if (timerMovimiento->isActive())
            timerMovimiento->stop();
        explotar();
    });
}


void Granada::mover()
{
    int dx = direccionDerecha ? velocidad : -velocidad;
    int nuevoX = x() + dx;

    // Actualizamos la componente vertical
    velocidadY += gravedad;
    int nuevoY = y() + velocidadY;

    move(nuevoX, nuevoY);
    distanciaRecorrida += qAbs(dx);

    if (distanciaRecorrida >= maxDistancia || nuevoY > parentWidget()->height()) // por si se cae fuera de pantalla
    {
        timerMovimiento->stop();
        explotar();
    }
}

void Granada::explotar()
{
    timerMovimiento->stop();
    this->hide();

    frameActualExplosion=0;
    timerExplosion=new QTimer(this);
    connect(timerExplosion, &QTimer::timeout, this, &Granada::avanzarFrameExplosion);
    timerExplosion->start(60);

    // Coordenadas en sistema del parentWidget (mismo que los zombies)
    int radioExplosion = 100;
    QPoint centroGranada=mapToParent(rect().center());
    QRect areaExplosion(centroGranada.x()-radioExplosion,centroGranada.y()-radioExplosion,radioExplosion*2,radioExplosion*2);

    // (Opcional) Mostrar rectangulo visual para depurar

    //DEBUG DEL RADIOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    // QFrame* debug = new QFrame(parentWidget());
    // debug->setGeometry(areaExplosion);
    // debug->setStyleSheet("background: rgba(255,0,0,80); border: 2px solid black;");
    // debug->show();
    // QTimer::singleShot(500, debug, &QFrame::deleteLater);


    //¿Que hace esto exactamente?

    //Busca todos los objetos visuales (QLabel) hijos del mismo contenedor (normalmente AtributosPersonaje).

    // Filtra solo los que son Zombie activos en escena.

    //Entonces, no depende de una lista manual, sino que recorre directamente lo que esta presente en la escena,
    //lo mismo que hace el ataque melee o las balas.

    QList<QLabel*> posibles = parentWidget()->findChildren<QLabel*>();//Busca todos los QLabel hijos del contenedor, que incluye a los zombies.

    for(QLabel* obj : posibles)//	Recorre cada posible QLabel.
    {
        if(obj->inherits("Zombie"))//Verifica si ese objeto hereda de Zombie.
        {
            Zombie* z = static_cast<Zombie*>(obj);//Convierte el objeto a Zombie* de forma segura.
            if (z->muerto) continue;//Si el zombie ya esta muerto, lo saltaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.

            QRect hitbox = z->hitbox();//	Obtiene la hitbox del zombie.
            if (areaExplosion.intersects(hitbox)) {//	Verifica si la hitbox del zombie intersecta con el area de la explosión.
                z->recibirDanio(20);
                qDebug() << "💥 Zombie alcanzado por explosion";
            }
        }
    }

    // ----------------- DAÑO AL JUGADOR -----------------
    //Busca al personaje dentro del widget padre (normalmente es AtributosPersonaje).
    personaje*p=parentWidget()->findChild<personaje*>();//aqui verifica que el personaje exista, no esta muerto, y este dentro del area de explosion.
    QWidget* ventanaActual=parentWidget(); // AtributosPersonaje
    if(p&&!p->estaMuertoPersonaje()&&areaExplosion.intersects(p->geometry()))
    {
        int nuevaVida=p->getVidas()-5;//pierde 5 de vida por cada explosion
        p->setVidas(nuevaVida);
        qDebug() << "🔥 El jugador fue alcanzado por la explosion de la granada.";

        if (nuevaVida<=0)
        {
            p->Morir();

            // Mostrar mensaje fuera del hilo de granada
            QTimer::singleShot(800, ventanaActual, [=]()
            {

                QMessageBox::information(nullptr,"💀 GAME OVER", "Has muerto por una explosión.");

                // Cerrar todo y volver al inicio
                QTimer::singleShot(300, nullptr, [=]()
                {
                    Inicio*i=new Inicio();
                    i->show();

                    // Cerrar la ventana actual (AtributosPersonaje)
                    ventanaActual->close();
                });
            });
        }

    }

}

void Granada::avanzarFrameExplosion()
{
    ++frameActualExplosion;
    if (frameActualExplosion >= framesExplosion.size())
    {
        timerExplosion->stop();
        deleteLater();              // se elimina la granada tras la explosion
        return;
    }
    setPixmap(framesExplosion[frameActualExplosion]);
    show();
    raise();
    update();

}
