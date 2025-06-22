#include "supermercado.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include "caminos.h"
#include "inicio.h"
#include "Inventario.h"

supermercado::supermercado(personaje*jugadorExistente,QWidget* parent) : AtributosPersonaje(jugadorExistente,parent)
{
    this->resize(1280, 720);
    this->setWindowTitle("Supermercado - Last hope");

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
            c->cambiarRuta(9); // <---- Manda a Ruta 9
            c->posicionarJugadorEnCalleRuta9();
            c->show();
            ResetearMovimiento();
            this->close();
            deleteLater();

        });
    });

    cofreLabel = new QLabel(this);
    cofreLabel->setGeometry(1112, 508, 164, 164);
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

    // --------------- ZOMBIES ---------------
    int posX = 80; // a la izquierda
    int posY = 500;

    // 2 zombies grandotes (Z7)
    for (int i = 0; i < 2; ++i) {
        Zombie* z7 = new Zombie(Zombie::Tipo::Z7, this);
        z7->move(posX, posY + i*60);
        z7->setVelocidad(2); // más lento
        z7->show();
        zombies.append(z7);
        posX += 80; // pequeño desplazamiento para que no se encimen
    }

    // 2 zombies tipo 5 (Z5)
    posX = 250;
    for (int i = 0; i < 2; ++i) {
        Zombie* z5 = new Zombie(Zombie::Tipo::Z5, this);
        z5->move(posX, posY + 40*i);
        z5->setVelocidad(3); // velocidad media
        z5->show();
        zombies.append(z5);
        posX += 80;
    }

    // 1 zombie tipo 3 (Z3)
    Zombie* z3 = new Zombie(Zombie::Tipo::Z3, this);
    z3->move(400, posY + 30);
    z3->setVelocidad(5); // más rápido
    z3->show();
    zombies.append(z3);

    // Conectar colisiones con jugador
    for (Zombie* z : zombies) {
        z->perseguirJugador(jugador);

        connect(z, &Zombie::ColisionConJugador, this, [=]() {
            if (jugador->getVidas() <= 0) return;

            jugador->setVidas(jugador->getVidas() - 1);
            ActualizarBarraVida();
            CancelarCuracion();
            ActualizarMuniciones();

            if (jugador->getVidas() <= 0) {
                jugador->Morir();
                movimientoTimer->stop();

                QTimer::singleShot(1000, this, [=]() {
                    QMessageBox::information(this, "💀 GAME OVER", "Has muerto...");
                    Inicio* i = new Inicio();
                    i->show();
                    this->close();
                });
            }
        });
    }
}


void supermercado::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/War6.png");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/Gimnasio.jpeg";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void supermercado::configurarObstaculos() {
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void supermercado::onMovimientoUpdate() {
    verificarZombiesYMostrarMensaje();
}

bool supermercado::eventFilter(QObject* obj, QEvent* event) {
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
                    cofreLabel->setPixmap(cofreAbierto.scaled(64, 64));
                    mensajeCofre->setText("🎁 Cofre abierto");
                    mensajeCofre->show();
                    Inventario::getInstance()->insertarObjeto("chaleco",2,"armadura","protege");
                    mostrarNotificacion("¡Recibiste dos chalecos!");
                    QTimer::singleShot(3000, this, [=]() {
                        mostrarNotificacion("🛒 Nivel completado...");
                        Caminos* c = new Caminos(jugador);
                        Inventario::getInstance()->setBalas(jugador->getMuniciones());
                        c->cambiarRuta(9); // <---- Manda a Ruta 9
                        c->posicionarJugadorEnCalleRuta9();
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
void supermercado::keyPressEvent(QKeyEvent* event)
{


    if(event->key()==Qt::Key_I)
    {

        return;//se bloquea el inventario

    }

    AtributosPersonaje::keyPressEvent(event);

}

void supermercado::mostrarNotificacion(const QString& texto) {
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

void supermercado::verificarZombiesYMostrarMensaje() {
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
        mostrarNotificacion("🏆 ¡Has limpiado el Supermercado!\nPodés abrir el cofre.");
        TablaHash::getInstance().descubrir("Nivel6");
    }
}


