#include "caminos.h"
#include<QPixmap>
#include<QLabel>
#include<QDebug>
#include<QTimer>
#include"lobby.h"
#include"Ciudad.h"

Caminos::Caminos(QWidget* parent) : EscenaBase(parent), rutaActual(1)
{
    this->resize(1078, 714);
    this->setWindowTitle("Caminos - Last hope");

    configurarEscena();
    inicializarJugador();
    configurarObstaculos();
    Movimientos();

    jugador->move(22,214);

    labelPresionarT = new QLabel("PRESIONE T PARA ENTRAR", this);
    labelPresionarT->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 5px; border-radius: 5px;");
    labelPresionarT->setAlignment(Qt::AlignCenter);
    labelPresionarT->setWordWrap(true);
    labelPresionarT->setFixedSize(200, 30);
    labelPresionarT->hide(); // Al inicio oculto

}

void Caminos::configurarEscena()
{
    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    cambiarRuta(1);
}


//AGREGAR OBSTACULOS DESPUESSSS
void Caminos::configurarObstaculos()
{
    obstaculos.clear();

    if (rutaActual == 1)
    {
        //obstaculos.append(QRect(18, 351, 344, 686));

    }else if (rutaActual == 2){
       // obstaculos.append(QRect(30, 75, 215, 420));

    }
}

void Caminos::cambiarRuta(int nuevaRuta)
{
    rutaActual = nuevaRuta;

    QString rutaImagen;

    if (rutaActual == 1)
    {
        rutaImagen = ":/imagenes/assets/mapas/RUTA_1.jpg";
    }
    else if (rutaActual == 2)
    {
        rutaImagen = ":/imagenes/assets/mapas/RUTA_2.jpg";
    }

    QPixmap fondoPixmap(rutaImagen);
    if (fondoPixmap.isNull())
    {
        qDebug() << "Error al cargar la imagen de la ruta:" << rutaImagen;
    }
    else
    {
        fondoLabel->setPixmap(fondoPixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));
    }

    if(jugador)
    {

        jugador->DetenerAnimacion();
        jugador->SetAnimacion(":/imagenes/assets/protagonista/Idle.png",7);

    }

    //aqui llamaar funcion de configurar otro obstaculo
}

void Caminos::onMovimientoUpdate()
{
    if (!jugador) return;

    QRect rectJugador = jugador->geometry();

    // aqui Cambiar a RUTA_2 cuando el jugador llegue al final de la carretera de arriba (borde derecho)
    if (rutaActual == 1 && rectJugador.right() >= width() - 50 && rectJugador.top() <= 200)
    {
        cambiarRuta(2);

        //aqui se coloca al jugador en el borde IZQUIERDO de la carretera inferior de RUTA_2
        jugador->move(100, height() - jugador->height() - 120);
        return;
    }

    //aqui vuelve a RUTA_1 si el jugador regresa por la izquierda en RUTA_2 (carretera inferior)
    if (rutaActual == 2 && rectJugador.left() <= 100 && rectJugador.bottom() >= height() - 120)
    {
        cambiarRuta(1);

        //aqui vuelve a colocar al jugador en borde derecho de carretera de ARRIBA en RUTA_1
        jugador->move(width() - jugador->width() - 50, 200);
        return;
    }

    //CON ESTO REGRESA AL LOBBY (zona de la puerta en RUTA_1)
    if (rutaActual == 1 && !regresandoAlLobby)
    {
        QRect zonaPuertaLobby(0, 120, 30, 100);

        if (rectJugador.intersects(zonaPuertaLobby))
        {
            regresandoAlLobby = true; //aqui se ACTIVA la bandera pa no creear un pjaso de widgets repetidos

            qDebug() << "Regresando al lobby...";

            lobby* lobbyWindow = new lobby();
            lobbyWindow->posicionarJugadorEnPuerta();
            lobbyWindow->show();
            this->close();

            return;
        }
    }

    if (rutaActual == 2)
    {
        if (!labelPresionarT->isVisible())
        {
            // Lo coloco fijo debajo del texto
            labelPresionarT->move(width() / 2 - labelPresionarT->width() / 2, 690); // aprox debajo del texto
            labelPresionarT->show();
            labelPresionarT->raise();
        }
    }else{
        labelPresionarT->hide();
    }

}

void Caminos::keyPressEvent(QKeyEvent *event)
{

     // Primero siempre llamar a la base
    EscenaBase::keyPressEvent(event);

    //aqui accion con T para entrar a Ciudad (solo si estamos en ruta 2)
    if (rutaActual == 2 && event->key() == Qt::Key_T && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando a la Ciudad...";

        // Crear la ciudad y mostrarla
        Ciudad* ciudadWindow = new Ciudad();
        ciudadWindow->show();

        // Cerrar esta ventana
        this->close();

        return; // salir
    }

}
