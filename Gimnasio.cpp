#include "gimnasio.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include "caminos.h"
#include "inicio.h"
#include "Inventario.h"

Gimnasio::Gimnasio(personaje* jugadorExistente,QWidget* parent) : AtributosPersonaje(jugadorExistente,parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Gimnasio - Last hope");

    configurarEscena();
    inicializarJugador();
    jugador->setMuniciones(Inventario::getInstance()->getBalas());
    jugador->move(16, 538);
    configurarObstaculos();

    cofreCerrado = QPixmap(":/imagenes/assets/items/cofre_cerrado.png");
    cofreAbierto = QPixmap(":/imagenes/assets/items/cofre_abierto.png");

    btnSalir = new QPushButton("Abandonar Partida", this);
    btnSalir->setFocusPolicy(Qt::NoFocus);  // No interfiere con las teclas
    btnSalir->setStyleSheet(
        "QPushButton {"
        "  background-color: #8B0000;"
        "  color: white;"
        "  font-weight: bold;"
        "  border: 2px solid white;"
        "  border-radius: 10px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #B22222;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #5C0000;"
        "}"
        );
    btnSalir->setGeometry(width() - 180, 60, 170, 40);

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
            c->cambiarRuta(5); // <---- Manda a Ruta 5
            c->posicionarJugadorEnCalleRuta5();
            c->show();
            ResetearMovimiento();
            this->close();
            deleteLater();

        });
    });

    cofreLabel = new QLabel(this);
    cofreLabel->setGeometry(1112, 508, 164, 164); // hitbox grande
    cofreLabel->setPixmap(cofreCerrado.scaled(64, 64));
    cofreLabel->show();
    cofreLabel->installEventFilter(this);

    mensajeCofre = new QLabel(this);
    mensajeCofre->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 150); padding: 4px; font-weight: bold;");
    mensajeCofre->setAlignment(Qt::AlignCenter);
    mensajeCofre->setGeometry(1080, 490, 180, 30);
    mensajeCofre->hide();

    Movimientos();

    puedeDisparar = true;
    labelMuniciones->show();
    ActualizarMuniciones();


    // Crear y registrar 4 zombies rápidos (Z6)
    int posX6 = 1100;
    for (int i = 0; i < 4; ++i) {
        Zombie* z6 = new Zombie(Zombie::Tipo::Z6, this);
        z6->move(posX6, 500);
        z6->setVelocidad(5); // Rápido
        z6->show();
        zombies.append(z6);
        posX6 -= 70;
    }

    // Crear y registrar 4 zombies lentos (Z4)
    int posX4 = 1000;
    for (int i = 0; i < 4; ++i) {
        Zombie* z4 = new Zombie(Zombie::Tipo::Z4, this);
        z4->move(posX4, 500);
        z4->setVelocidad(4); // Lento
        z4->show();
        zombies.append(z4);
        posX4 -= 70;
    }

    // Conectar colisiones con jugador
    for (Zombie* z : zombies) {
        z->perseguirJugador(jugador);

        connect(z, &Zombie::ColisionConJugador, this, [=]() {
            if(jugador->getVidas()<=0) return;

            if(jugador->getVidas()>0)
            {

                if(jugador->getEscudo()>0)
                {
                    int escudoActual=jugador->getEscudo();
                    jugador->setEscudo(escudoActual-1);//QUITA ESCUDO
                    ActualizarBarraEscudo();

                }else{

                    jugador->setVidas(jugador->getVidas()-1);//QUITA VIDA
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

                        Inventario* inv = Inventario::getInstance();
                        inv->eliminarObjeto("casco");      // quita el casco
                        inv->eliminarObjeto("chaleco");    // quita el chaleco (si estaba equipado)
                        jugador->setEscudo(0);             // pone la barra en 0

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

void Gimnasio::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/gym.jpeg");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/gym.jpeg";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void Gimnasio::configurarObstaculos()
{
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void Gimnasio::onMovimientoUpdate() {
    ActualizarBarraEscudo();
    verificarZombiesYMostrarMensaje();
}

bool Gimnasio::eventFilter(QObject* obj, QEvent* event) {
    if (obj == cofreLabel) {
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
        else if (event->type() == QEvent::Leave) {
            mensajeCofre->hide();
        }
        else if (event->type() == QEvent::MouseButtonPress) {
            if (!cofreAbiertoYa) {
                bool zombiesVivos = false;
                for (Zombie* z : zombies) {
                    if (!z->muerto) {
                        zombiesVivos = true;
                        break;
                    }
                }
                if (!zombiesVivos) {
                    cofreAbiertoYa = true;
                    cofreLabel->setPixmap(cofreAbierto.scaled(104, 104));
                    mensajeCofre->setText("🎁 Cofre abierto");
                    mensajeCofre->show();
                    if (TablaHash::getInstance().estaDescubierto("Nivel3")){
                        Inventario::getInstance()->setBalas(Inventario::getInstance()->getBalas()+30);
                        mostrarNotificacion("Recibiste 30 municiones!");
                    }else {
                        Inventario::getInstance()->insertarObjeto("casco",1,"armadura","protege");
                        TablaHash::getInstance().descubrir("Nivel3");
                        mostrarNotificacion("¡Recuperaste el casco!");
                    }
                    QTimer::singleShot(3000, this, [=]() {
                        ActualizarBarraEscudo();
                        mostrarNotificacion("🏋️ Nivel completado...");
                        Caminos* c=new Caminos(jugador);
                        Inventario::getInstance()->setBalas(jugador->getMuniciones());
                        c->cambiarRuta(5); // <---- Manda a Ruta 5
                        c->posicionarJugadorEnCalleRuta5();
                        c->show();
                        ResetearMovimiento();
                        this->close();
                    });
                    return true;
                }
                return true;
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

//SE HEREDA LOS BOTONES A LOS MAPAS
void Gimnasio::keyPressEvent(QKeyEvent* event)
{


    if(event->key()==Qt::Key_I)
    {

        return;//se bloquea el inventario

    }

    AtributosPersonaje::keyPressEvent(event);
}

void Gimnasio::mostrarNotificacion(const QString& texto) {
    if (!labelNotificacion) {
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

void Gimnasio::verificarZombiesYMostrarMensaje() {
    static bool mensajeMostrado = false;
    if (mensajeMostrado) return;

    bool zombiesVivos = false;
    for (Zombie* z : zombies) {
        if (!z->muerto) {
            zombiesVivos = true;
            break;
        }
    }

    if (!zombiesVivos) {
        mensajeMostrado = true;
        mostrarNotificacion("🏆 ¡Has limpiado el Gimnasio!\nPodés abrir el cofre.");
    }
}
