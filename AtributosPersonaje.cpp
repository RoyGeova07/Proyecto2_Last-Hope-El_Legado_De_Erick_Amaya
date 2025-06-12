#include "AtributosPersonaje.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include"bala.h"
#include"zombie.h"
#include"Ciudad.h"

AtributosPersonaje::AtributosPersonaje(QWidget* parent)
    : QWidget(parent),
    jugador(nullptr), movimientoTimer(nullptr),
    shiftPresionado(false), izquierdaPresionada(false),
    derechaPresionada(false), arribaPresionado(false),
    abajoPresionado(false), ZPresionado(false)
{

    inventarioWidget=new InventarioWidget(Inventario::getInstance());

    //barra de vida contenedor
    barraVidaLabel = new QLabel(this);
    barraVidaLabel->setStyleSheet("background-color: #333333; border: 2px solid black; border-radius: 5px; padding: 0px;");
    barraVidaLabel->setGeometry(10, 90, 200, 30);
    barraVidaLabel->show();

    //Barra rellena (acortable)
    barraVidaInterna = new QWidget(barraVidaLabel);
    barraVidaInterna->setStyleSheet("background-color: green; border-radius: 3px;");
    barraVidaInterna->setGeometry(0, 0, 200, 30);
    barraVidaInterna->show();

    //Texto encima
    barraVidaTexto = new QLabel(barraVidaLabel);
    barraVidaTexto->setAlignment(Qt::AlignCenter);
    barraVidaTexto->setStyleSheet("color: white; font-weight: bold; font-size: 14px; background: transparent;");
    barraVidaTexto->setGeometry(0, 0, 200, 30);
    barraVidaTexto->show();
    ActualizarBarraVida();
    barraVidaLabel->raise();

    //etiqueta para mostrar las municiones
    labelMuniciones=new QLabel(this);
    labelMuniciones->setGeometry(10,45,300,40);
    labelMuniciones->setStyleSheet(
        "color: #FFD700;"
        "font-size: 18px;"
        "font-weight: bold;"
        "background: rgba(0,0,0,100);"
        "padding: 5px;"
        );
    labelMuniciones->show();

    if(!puedeDisparar)
    {

        labelMuniciones->hide();//ocultar el label de municiones si no esta habilitado

    }

}

AtributosPersonaje::AtributosPersonaje(personaje *jugadorExistente, QWidget *parent):QWidget(parent),

    jugador(jugadorExistente), movimientoTimer(nullptr),
    shiftPresionado(false), izquierdaPresionada(false),
    derechaPresionada(false), arribaPresionado(false),
    abajoPresionado(false), ZPresionado(false)
{

    inventarioWidget=new InventarioWidget(Inventario::getInstance());
    //barra de vida contenedor
    barraVidaLabel = new QLabel(this);
    barraVidaLabel->setStyleSheet("background-color: #333333; border: 2px solid black; border-radius: 5px; padding: 0px;");
    barraVidaLabel->setGeometry(10, 90, 200, 30);
    barraVidaLabel->show();

    //Barra rellena (acortable
    barraVidaInterna = new QWidget(barraVidaLabel);
    barraVidaInterna->setStyleSheet("background-color: green; border-radius: 3px;");
    barraVidaInterna->setGeometry(0, 0, 200, 30);
    barraVidaInterna->show();

    //Texto encima
    barraVidaTexto = new QLabel(barraVidaLabel);
    barraVidaTexto->setAlignment(Qt::AlignCenter);
    barraVidaTexto->setStyleSheet("color: white; font-weight: bold; font-size: 14px; background: transparent;");
    barraVidaTexto->setGeometry(0, 0, 200, 30);
    barraVidaTexto->show();

    //etiqueta para mostrar las municiones
    labelMuniciones=new QLabel(this);
    labelMuniciones->setGeometry(10,45,300,40);
    labelMuniciones->setStyleSheet(
        "color: #FFD700;"
        "font-size: 18px;"
        "font-weight: bold;"
        "text-shadow: 1px 1px black;"
        "background: rgba(0,0,0,100);"
        "padding: 5px;"
        );
    labelMuniciones->show();

    ActualizarBarraVida();
    barraVidaLabel->raise();


}

AtributosPersonaje::~AtributosPersonaje()
{

    delete movimientoTimer;
    delete inventarioWidget;

}

void AtributosPersonaje::inicializarJugador() {
    if(!jugador)
    {

        jugador = new personaje(this);//Solo si NO hay jugador
        jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png",7);

    }else{

        jugador->setParent(this);//si ya se actualizo solo se actualiza el parent

    }
    jugador->show();
    jugador->raise();

}

void AtributosPersonaje::Movimientos() {
    movimientoTimer = new QTimer(this);
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {
        bool moviendo = false;

        if(ZPresionado&&puedeDisparar&&!disparandoAhora)
        {
            if(jugador->getMuniciones()>0)
            {
                disparandoAhora=true;
                jugador->setMuniciones(jugador->getMuniciones()-1);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Shot_1.png",4);
                ActualizarMuniciones();

                //crea la bala
                Bala*bala=new Bala(this);

                int offsetX=jugador->miradoDerecha?213:(128-213-10);// derecha o izquierda del arma
                int offsetY=90;// altura a la mitad

                bala->move(jugador->x()+offsetX,jugador->y()+offsetY);
                bala->Disparar(jugador->miradoDerecha);
                balasActivas.append(bala);
                connect(bala, &Bala::impactoBala, this, [=](Bala* b) {
                    balasActivas.removeOne(b);
                    b->deleteLater();  // esto elimina la bala una vez que termine de ejecutarse
                });

                //Cancelar cualquier timer de disparo anterior
                if(disparoTimer)
                {

                    disparoTimer->stop();
                    delete disparoTimer;
                    disparoTimer=nullptr;

                }

                //aqui se crea un nuevo timer y guardarlo para poder cancelarlo
                disparoTimer=new QTimer(this);
                disparoTimer->setSingleShot(true);
                connect(disparoTimer,&QTimer::timeout,this,[=](){
                    disparandoAhora=false;

                    //si no hay mas municiones, forzar detener el disparo y desactivar el Zpresinodado
                    if(jugador->getMuniciones()==0)
                    {

                        ZPresionado=false;

                    }

                    if(!ZPresionado&&!izquierdaPresionada&&!derechaPresionada&&!arribaPresionado&&!abajoPresionado)
                    {

                        jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png",7);

                    }
                    delete disparoTimer;
                    disparoTimer=nullptr;

                });
                disparoTimer->start(400);//duracion del disparo

            }else{

                //si no hay balas, el bool se asegura q no siga intentando disparar
                ZPresionado=false;

            }
            return;//no se permite otras acciones mientras el personaje dispara
        }

        if (izquierdaPresionada) {
            jugador->MoverSiNoColisiona(-jugador->getVelocidadMoviento(), 0, obstaculos);
            moviendo = true;
        } else if (derechaPresionada) {
            jugador->MoverSiNoColisiona(jugador->getVelocidadMoviento(), 0, obstaculos);
            moviendo = true;
        }

        if (arribaPresionado) {
            jugador->MoverSiNoColisiona(0, -jugador->getVelocidadMoviento(), obstaculos);
            moviendo = true;
        } else if (abajoPresionado) {
            jugador->MoverSiNoColisiona(0, jugador->getVelocidadMoviento(), obstaculos);
            moviendo = true;
        }

        if (moviendo) {
            if (shiftPresionado) {
                jugador->SetAnimacionMovimiento(6);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Run.png", 8);
            } else {
                jugador->SetAnimacionMovimiento(2);
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Walk.png", 7);
            }
        } else {

            movimientoTimer->stop();

            if(!disparandoAhora)
            {

                jugador->DetenerAnimacion();
                jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);

            }

        }
        ActualizarBarraVida();
        onMovimientoUpdate();
    });

    movimientoTimer->setInterval(30);
}

void AtributosPersonaje::keyPressEvent(QKeyEvent* event) {
    if (!jugador) return;

    //aqui tecla l para abrir y cerrar el inventario
    if (event->key() == Qt::Key_L)
    {
        if (inventarioWidget)
        {
            if (inventarioWidget->isVisible())
                inventarioWidget->hide();
            else
            {
                inventarioWidget->actualizarVista();
                inventarioWidget->show();
                inventarioWidget->raise();
                inventarioWidget->activateWindow();
            }
        }
    }

    switch (event->key()) {
    case Qt::Key_Shift:
        shiftPresionado = true;
        break;
    case Qt::Key_Left:
        izquierdaPresionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Right:
        derechaPresionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Up:
        arribaPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Down:
        abajoPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Z:
        ZPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    }
}

void AtributosPersonaje::keyReleaseEvent(QKeyEvent* event) {
    if (!jugador) return;

    switch (event->key()) {
    case Qt::Key_Shift:
        shiftPresionado = false;
        break;
    case Qt::Key_Left:
        izquierdaPresionada = false;
        break;
    case Qt::Key_Right:
        derechaPresionada = false;
        break;
    case Qt::Key_Up:
        arribaPresionado = false;
        break;
    case Qt::Key_Down:
        abajoPresionado = false;
        break;
    case Qt::Key_Z:
        ZPresionado = false;
        break;
    }
}

void AtributosPersonaje::ResetearMovimiento()
{

    if (movimientoTimer && movimientoTimer->isActive())
        movimientoTimer->stop();

    shiftPresionado = false;
    izquierdaPresionada = false;
    derechaPresionada = false;
    arribaPresionado = false;
    abajoPresionado = false;
    ZPresionado = false;

    if (jugador)
    {
        jugador->DetenerAnimacion();
        jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png",7);
    }

    ActualizarBarraVida();
}

void AtributosPersonaje::ActualizarBarraVida()
{

    if(!jugador)return;

    int vidaActual=jugador->getVidas();
    int vidaMaxima=30;//valor maximo de vida

    //con estas condicionales se asegura los limites
    if(vidaActual>vidaMaxima)vidaActual=vidaMaxima;
    if(vidaActual<0)vidaActual=0;

    //aqui se calcula el porcentaje
    float porcentaje=static_cast<float>(vidaActual)/vidaMaxima;
    int anchoTotal=barraVidaLabel->contentsRect().width();
    int altoTotal=barraVidaLabel->contentsRect().height();

    int anchoBarra=static_cast<int>(porcentaje*anchoTotal);

    QString colorBarra;
    if(vidaActual>=18)
    {

        colorBarra="green";

    }else if(vidaActual>=13){

        colorBarra="orange";

    }else{

        colorBarra="red";

    }

    barraVidaInterna->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(colorBarra));
    barraVidaInterna->setGeometry(0,0,anchoBarra,altoTotal);

    //aqui se hace el texto de la barra
    QString texto = QString("VIDA: %1 / %2").arg(vidaActual).arg(vidaMaxima);
    barraVidaTexto->setText(texto);

    barraVidaLabel->raise();

    //CON ESTO SE CURA 5 DE VIDA
    // int nuevaVida = jugador->getVidas() + 5;
    // if (nuevaVida > 30) nuevaVida = 30;
    // jugador->setVidas(nuevaVida);
    // actualizarBarraVida();


    //SE CURA 10 DE VIDA
    // int nuevaVida = jugador->getVidas() + 10;
    // if (nuevaVida > 30) nuevaVida = 30;
    // jugador->setVidas(nuevaVida);
    // actualizarBarraVida();

}

void AtributosPersonaje::ActualizarMuniciones()
{

    if(!jugador)return;

    int mun=jugador->getMuniciones();
    QString texto=QString("MUNICIONES: %1").arg(mun);
    labelMuniciones->setText(texto);
    labelMuniciones->raise();

}
