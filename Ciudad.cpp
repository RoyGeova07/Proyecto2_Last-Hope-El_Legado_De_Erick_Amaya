#include "Ciudad.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include<QMessageBox>
#include"lobby.h"
#include"inicio.h"
#include"caminos.h"
#include "Inventario.h"
#include"personaje.h"

Ciudad::Ciudad(personaje* jugadorExistente,QWidget* parent) : AtributosPersonaje(jugadorExistente,parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Ciudad en Ruinas - Last hope");

    configurarEscena();
    inicializarJugador();
    jugador->setMuniciones(Inventario::getInstance()->getBalas());
    jugador->move(16,538);
    configurarObstaculos();

    cofreCerrado=QPixmap(":/imagenes/assets/items/cofre_cerrado.png"); // actualiza ruta
    cofreAbierto=QPixmap(":/imagenes/assets/items/cofre_abierto.png");

    btnSalir=new QPushButton("Abandonar Partida", this);
    btnSalir->setFocusPolicy(Qt::NoFocus);
    btnSalir->setStyleSheet(
        "QPushButton {"
        "  background-color: #8B0000;"        // rojo oscuro
        "  color: white;"
        "  font-weight: bold;"
        "  border: 2px solid white;"
        "  border-radius: 10px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #B22222;"        // mas claro al pasar el mouse
        "}"
        "QPushButton:pressed {"
        "  background-color: #5C0000;"        // mas oscuro al presionar
        "}"
        );

    btnSalir->setGeometry(width()-180,60,170,40);  // Ajustado dentro de pantalla

    // Funcion para verificar vida
    auto actualizarEstadoBoton = [this]() {
        int vidasActuales = jugador->cargarDatosJugador()["Vida"];
        qDebug() << "Vida actual del jugador:" << vidasActuales;

        bool debeEstarActivo = (vidasActuales >= 5);

        if (btnSalir->isEnabled() != debeEstarActivo) {
            btnSalir->setEnabled(debeEstarActivo);
            qDebug() << "Estado del botón cambiado a:" << (debeEstarActivo ? "Activado" : "Desactivado");

            // Mostrar notificación cuando se desactiva el botón (vida <= 5)
            if (!debeEstarActivo) {
                mostrarNotificacion("No puedes abandonar la partida con poca vida!");
                qDebug() << "Mostrando notificación de vida baja";
            }
        }
    };

    actualizarEstadoBoton();

    // Configurar timer para verificar vida
    QTimer *timerVida = new QTimer(this);
    connect(timerVida, &QTimer::timeout, this, actualizarEstadoBoton);
    timerVida->start(1000);

    connect(btnSalir, &QPushButton::clicked, this, [=]() {
        this->hide();
        QTimer::singleShot(300, this, [=]() {

            Caminos*c=new Caminos(jugador);
            c->cambiarRuta(2);
            c->posicionarJugadorEnCalleRuta2();
            c->show();
            ResetearMovimiento();//FUNCION SALVADORA, QUE EVITA BUGS, TE ADOROOOOOOOOOOOOOO
            this->close();
            deleteLater();

        });
    });

    cofreLabel=new QLabel(this);
    cofreLabel->setGeometry(1112, 508, 164, 164);
    cofreLabel->setPixmap(cofreCerrado.scaled(64,64)); //tamaño del cofre
    cofreLabel->show();
    cofreLabel->installEventFilter(this);
    //obstaculos.append(QRect(1142,538,64,64));

    //etiqueta de estado del estado del cofre
    mensajeCofre=new QLabel(this);
    mensajeCofre->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 150); padding: 4px; font-weight: bold;");
    mensajeCofre->setAlignment(Qt::AlignCenter);
    mensajeCofre->setGeometry(1080, 490, 180, 30);
    mensajeCofre->hide();//oculto por defecto

    Movimientos();
    puedeDisparar=true;
    labelMuniciones->show();//mostrar el contador municiones
    ActualizarMuniciones();

    //aqui se crea y registrar los zombies
    Zombie* z1 = new Zombie(Zombie::Tipo::Z1, this);
    z1->move(1100,500);
    z1->setVelocidad(4);
    z1->show();
    zombies.append(z1); //aqui se registra en el vector global

    Zombie* z2 = new Zombie(Zombie::Tipo::Z2, this);
    z2->move(800,500);
    z2->setVelocidad(3);
    z2->show();
    zombies.append(z2); //aqui se registra en el vector global

    Zombie* z3 = new Zombie(Zombie::Tipo::Z3, this);
    z3->move(900,500);
    z3->setVelocidad(2);
    z3->show();
    zombies.append(z3); //aqui se registra en el vector global

    //aqui es donde ocurre el daño hacia el personaje
    for(Zombie* z:zombies)
    {

        z->perseguirJugador(jugador);

        connect(z,&Zombie::ColisionConJugador,this,[=]()
        {

                    if(jugador->getVidas()<=0) return;

                    if(jugador->getVidas()>0)
                    {

                        if(jugador->getEscudo()>0)
                        {
                            int escudoActual=jugador->getEscudo();
                            jugador->setEscudo(escudoActual-1);
                            ActualizarBarraEscudo();

                        }else{

                            jugador->setVidas(jugador->getVidas()-1);
                            ActualizarBarraVida();

                        }
                        CancelarCuracion();
                        ActualizarMuniciones();

                        if(jugador->getVidas()<=0)
                        {

                            ResetearMovimiento();

                            jugador->Morir();
                            movimientoTimer->stop();

                            QTimer::singleShot(1000, this, [=]() {
                                QMessageBox::information(this, "💀 GAME OVER", "Has muerto...");

                                jugador->reiniciarEstadoDefensivo();

                                this->hide();
                                QTimer::singleShot(300, this, [=]()
                                {

                                    Inicio*i=new Inicio();
                                    i->show();
                                    deleteLater();  // destruye correctamente esta ventana actual
                                });

                                this->close();
                            });

                        }

                    }

                });

    }



}

void Ciudad::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/War.png");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/War.png";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void Ciudad::configurarObstaculos()
{
    // OBSTACULOS FIJOS
    obstaculos.append(QRect(3,2,1334,470));     // Muro superior
    obstaculos.append(QRect(5,669,1273,47));    // Piso inferior
    obstaculos.append(QRect(3,278,5,388));      // Pared izquierda
    obstaculos.append(QRect(1272,282,3,324));   // Pared derecha
}

void Ciudad::onMovimientoUpdate() {
    ActualizarBarraEscudo();
    if (jugador)
    {

        //aqui verifica si el jugador esta cerca del cofre distancia<80
        int distancia=std::abs(jugador->x()-1142);
        if (std::abs(jugador->x() - 1142) <= 50 && std::abs(jugador->y() - 538) <= 50 && !cofreAbiertoYa)

        {

            //aqui verifica si hay zombies vivos
            bool zombiesVivos=false;
            for(Zombie*z:zombies)
            {


                if (!z->muerto)
                {
                    zombiesVivos = true;
                    break;
                }

            }

            if(zombiesVivos)
            {

                mensajeCofre->setText("🔒 Cofre bloqueado");

            }else{

                mensajeCofre->setText("✅ Haz click para abrir");

            }
            mensajeCofre->show();

        }else{

            mensajeCofre->hide();

        }

    }

    verificarZombiesYMostrarMensaje();
}

//SE HEREDA LOS BOTONES A LOS MAPAS
void Ciudad::keyPressEvent(QKeyEvent* event)
{


    if(event->key()==Qt::Key_I)
    {

        return;//se bloquea el inventario

    }

    AtributosPersonaje::keyPressEvent(event);
}

void Ciudad::mostrarNotificacion(const QString& texto)
{
    if (!labelNotificacion)
    {
        labelNotificacion = new QLabel(texto, this);
        labelNotificacion->setStyleSheet("background: rgba(0, 0, 0, 200); color: lime; font-size: 16px; padding: 10px; border-radius: 8px; border: 2px solid lime;");
        labelNotificacion->setAlignment(Qt::AlignCenter);
        labelNotificacion->setFixedSize(400, 100);
    }

    labelNotificacion->setText(texto);
    labelNotificacion->move((width() - labelNotificacion->width()) / 2, 50);
    labelNotificacion->show();
    labelNotificacion->raise();

    QTimer::singleShot(2500, labelNotificacion, [=]() {
        labelNotificacion->hide();
    });
}

bool Ciudad::eventFilter(QObject* obj, QEvent* event) {
    if (obj == cofreLabel) {
        // Mostrar mensaje al entrar
        if (event->type() == QEvent::Enter) {
            if (cofreAbiertoYa) {
                mensajeCofre->setText("🎁 Cofre abierto");
            } else {
                bool zombiesVivos = false;
                for (Zombie* z : zombies) {
                    if (!z->muerto) {
                        zombiesVivos = true;
                        break;
                    }
                }
                if (zombiesVivos) {
                    mensajeCofre->setText("🔒 Cofre bloqueado");
                } else {
                    mensajeCofre->setText("✅ Haz click para abrir");
                }
            }
            mensajeCofre->show();
        }
        // Ocultar mensaje al salir
        else if (event->type() == QEvent::Leave) {
            mensajeCofre->hide();
        }
        // Clic en el cofre
        else if (event->type() == QEvent::MouseButtonPress) {
            //Solo abrir si NO esta abierto y NO hay zombies vivos
            if (!cofreAbiertoYa) {
                bool zombiesVivos = false;
                for (Zombie* z : zombies) {
                    if (!z->muerto) {
                        zombiesVivos = true;
                        break;
                    }
                }
                if (!zombiesVivos) {
                    //Logica exacta de abrir el cofre (igual que con la T)
                    cofreAbiertoYa = true;
                    cofreLabel->setPixmap(cofreAbierto.scaled(64, 64));
                    mensajeCofre->setText("🎁 Cofre abierto");
                    mensajeCofre->show();

                    //agregar llave al inventario si es primera vez
                    if (TablaHash::getInstance().estaDescubierto("Nivel2")){
                        Inventario::getInstance()->insertarObjeto("municiones",20,"arma","disparar");
                        mostrarNotificacion("Recibiste municiones!");
                    } else {
                        Inventario::getInstance()->insertarObjeto("llave",1,"objeto","desconocido");
                        mostrarNotificacion("Recibiste la llave!");
                    }

                    TablaHash::getInstance().descubrir("Nivel2");

                    QTimer::singleShot(3000, this, [=]() {
                        mostrarNotificacion("🏃 Volviendo al camino...");

                        Inventario::getInstance()->setBalas(jugador->getMuniciones());

                        Caminos* c = new Caminos(jugador);
                        c->cambiarRuta(2);
                        c->posicionarJugadorEnCalleRuta2();
                        c->show();
                        ResetearMovimiento();//FUNCION SALVADORA, QUE EVITA BUGS, TE ADOROOOOOOOOOOOOOO
                        this->close();
                    });
                    return true; // Consumimos el evento
                }
                // Si hay zombies vivos, el mensaje ya esta visible
                return true;
            }
            //Si ya esta abierto, no hacemos nada extra (mensaje de abierto ya visible)
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);

}


void Ciudad::verificarZombiesYMostrarMensaje()
{
    static bool mensajeMostrado = false;

    if (mensajeMostrado) return;

    bool zombiesVivos = false;
    for (Zombie* z : zombies)
    {
        if (!z->muerto)
        {
            zombiesVivos = true;
            break;
        }
    }

    if (!zombiesVivos)
    {
        mensajeMostrado = true;
        mostrarNotificacion("🏆 ¡Felicidades! Has pasado el Nivel 1.\nPuedes reclamar el cofre.");
    }
}
