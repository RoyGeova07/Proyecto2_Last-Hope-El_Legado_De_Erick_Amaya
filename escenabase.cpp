#include "escenabase.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>

EscenaBase::EscenaBase(QWidget* parent)
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

    // 2️⃣ Barra rellena (acortable)
    barraVidaInterna = new QWidget(barraVidaLabel);
    barraVidaInterna->setStyleSheet("background-color: green; border-radius: 3px;");
    barraVidaInterna->setGeometry(0, 0, 200, 30);
    barraVidaInterna->show();

    // 3️⃣ Texto encima
    barraVidaTexto = new QLabel(barraVidaLabel);
    barraVidaTexto->setAlignment(Qt::AlignCenter);
    barraVidaTexto->setStyleSheet("color: white; font-weight: bold; font-size: 14px; background: transparent;");
    barraVidaTexto->setGeometry(0, 0, 200, 30);
    barraVidaTexto->show();
    ActualizarBarraVida();
    barraVidaLabel->raise();

}

EscenaBase::EscenaBase(personaje *jugadorExistente, QWidget *parent):QWidget(parent),

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

    // 2️⃣ Barra rellena (acortable)
    barraVidaInterna = new QWidget(barraVidaLabel);
    barraVidaInterna->setStyleSheet("background-color: green; border-radius: 3px;");
    barraVidaInterna->setGeometry(0, 0, 200, 30);
    barraVidaInterna->show();

    // 3️⃣ Texto encima
    barraVidaTexto = new QLabel(barraVidaLabel);
    barraVidaTexto->setAlignment(Qt::AlignCenter);
    barraVidaTexto->setStyleSheet("color: white; font-weight: bold; font-size: 14px; background: transparent;");
    barraVidaTexto->setGeometry(0, 0, 200, 30);
    barraVidaTexto->show();

    ActualizarBarraVida();
    barraVidaLabel->raise();


}

EscenaBase::~EscenaBase()
{

    delete movimientoTimer;
    delete inventarioWidget;

}

void EscenaBase::inicializarJugador() {
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

void EscenaBase::Movimientos() {
    movimientoTimer = new QTimer(this);
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {
        bool moviendo = false;

        if (ZPresionado) {
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Shot_1.png", 4);
            return;
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
            jugador->DetenerAnimacion();
            jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png", 7);
        }
        ActualizarBarraVida();
        onMovimientoUpdate();
    });

    movimientoTimer->setInterval(30);
}

void EscenaBase::keyPressEvent(QKeyEvent* event) {
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

void EscenaBase::keyReleaseEvent(QKeyEvent* event) {
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

void EscenaBase::ResetearMovimiento()
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

void EscenaBase::ActualizarBarraVida()
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


    // // Estilo de la barra
    // barraVidaLabel->setStyleSheet(QString(
    //                                   "background-color: %1; "
    //                                   "color: white; "
    //                                   "font-weight: bold; "
    //                                   "font-size: 14px; "
    //                                   "padding: 5px; "
    //                                   "border: 2px solid black; "
    //                                   "border-radius: 5px; "
    //                                   "text-align: center;")
    //                                   .arg(colorBarra));


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


    //LE RESTAN 7 DE VIDA
    // int nuevaVida = jugador->getVidas() - 7;
    // if (nuevaVida < 0) nuevaVida = 0;
    // jugador->setVidas(nuevaVida);
    // actualizarBarraVida();

}
