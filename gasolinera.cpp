#include "gasolinera.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include "caminos.h"
#include "inicio.h"
#include "Inventario.h"

Gasolinera::Gasolinera(QWidget* parent) : AtributosPersonaje(parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Gasolinera - Last hope");

    configurarEscena();
    inicializarJugador();
    jugador->setMuniciones(Inventario::getInstance()->getBalas());
    jugador->move(16, 538);
    configurarObstaculos();

    cofreCerrado = QPixmap(":/imagenes/assets/items/cofre_cerrado.png");
    cofreAbierto = QPixmap(":/imagenes/assets/items/cofre_abierto.png");

    cofreLabel = new QLabel(this);
    cofreLabel->setPixmap(cofreCerrado.scaled(64, 64));
    cofreLabel->setGeometry(1142, 538, 104, 104);
    cofreLabel->show();

    mensajeCofre = new QLabel(this);
    mensajeCofre->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 150); padding: 4px; font-weight: bold;");
    mensajeCofre->setAlignment(Qt::AlignCenter);
    mensajeCofre->setGeometry(1080, 490, 180, 30);
    mensajeCofre->hide();

    Movimientos();
    puedeDisparar = true;
    labelMuniciones->show();
    ActualizarMuniciones();

    // Crear y registrar 3 zombies de cada tipo
    int posX = 1100;
    for (int i = 0; i < 3; ++i) {
        Zombie* z1 = new Zombie(Zombie::Tipo::Z1, this);
        z1->move(posX, 500);
        z1->setVelocidad(4);
        z1->show();
        zombies.append(z1);

        Zombie* z2 = new Zombie(Zombie::Tipo::Z2, this);
        z2->move(posX - 100, 500);
        z2->setVelocidad(3);
        z2->show();
        zombies.append(z2);

        Zombie* z3 = new Zombie(Zombie::Tipo::Z3, this);
        z3->move(posX - 200, 500);
        z3->setVelocidad(2);
        z3->show();
        zombies.append(z3);

        posX -= 300;
    }

    // Conectar colisiones con jugador
    for (Zombie* z : zombies) {
        z->perseguirJugador(jugador);

        connect(z, &Zombie::ColisionConJugador, this, [=]() {
            if (jugador->getVidas() <= 0) return;

            jugador->setVidas(jugador->getVidas() - 1);
            ActualizarBarraVida();
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

void Gasolinera::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/War5.png");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/War5.png";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void Gasolinera::configurarObstaculos() {
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void Gasolinera::onMovimientoUpdate() {
    if (jugador)
    {

        //aqui verifica si el jugador esta cerca del cofre distancia<80
        int distancia=std::abs(jugador->x()-1142);
        if (jugador->x() >= 1104 && jugador->x() <= 1144 && jugador->y() == 538 && !cofreAbiertoYa)

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

                mensajeCofre->setText("✅ Presiona T para abrir");

            }
            mensajeCofre->show();

        }else{

            mensajeCofre->hide();

        }

    }

    verificarZombiesYMostrarMensaje();
}

void Gasolinera::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_T && !cofreAbiertoYa) {
        if (jugador->x() >= 1104 && jugador->x() <= 1144 && jugador->y() == 538) {
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
                jugador->setMuniciones(jugador->getMuniciones() + 30);
                ActualizarMuniciones();
                mostrarNotificacion("🎯 Recibiste 30 municiones");

                QTimer::singleShot(3000, this, [=]() {
                    mostrarNotificacion("⛽ Nivel completado...");
                    Caminos* c = new Caminos(jugador);
                    Inventario::getInstance()->setBalas(jugador->getMuniciones());
                    c->cambiarRuta(3); // ← Gasolinera termina en Ruta 3
                    c->posicionarJugadorEnCalleRuta3();
                    c->show();
                    this->close();
                });
            }
        }
    }

    AtributosPersonaje::keyPressEvent(event);
}

void Gasolinera::mostrarNotificacion(const QString& texto) {
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

void Gasolinera::verificarZombiesYMostrarMensaje() {
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
        mostrarNotificacion("🏆 ¡Has limpiado la Gasolinera!\nPodés abrir el cofre.");
    }
}
