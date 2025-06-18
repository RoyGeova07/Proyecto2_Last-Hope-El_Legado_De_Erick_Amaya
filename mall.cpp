#include "mall.h"
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include "caminos.h"
#include "inicio.h"
#include "Inventario.h"

Mall::Mall(personaje* jugadorExistente,QWidget* parent) : AtributosPersonaje(jugadorExistente,parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Mall - Last hope");

    configurarEscena();
    inicializarJugador();
    jugador->setMuniciones(Inventario::getInstance()->getBalas());
    jugador->move(16, 538);
    configurarObstaculos();

    cofreCerrado = QPixmap(":/imagenes/assets/items/cofre_cerrado.png");
    cofreAbierto = QPixmap(":/imagenes/assets/items/cofre_abierto.png");

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

    // Crear y registrar 3 zombies de cada tipo (Z4, Z5, Z6)
    int posX = 1100;
    for (int i = 0; i < 3; ++i) {
        Zombie* z4 = new Zombie(Zombie::Tipo::Z4, this);
        z4->move(posX, 500);
        z4->setVelocidad(4);
        z4->show();
        zombies.append(z4);

        Zombie* z5 = new Zombie(Zombie::Tipo::Z5, this);
        z5->move(posX - 100, 500);
        z5->setVelocidad(3);
        z5->show();
        zombies.append(z5);

        Zombie* z6 = new Zombie(Zombie::Tipo::Z6, this);
        z6->move(posX - 200, 500);
        z6->setVelocidad(2);
        z6->show();
        zombies.append(z6);

        posX -= 300;
    }

    // Conectar colisiones con jugador
    for (Zombie* z : zombies) {
        z->perseguirJugador(jugador);

        connect(z, &Zombie::ColisionConJugador, this, [=]() {
            if (jugador->getVidas() <= 0) return;

            jugador->setVidas(jugador->getVidas() - 1);
            CancelarCuracion();
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

void Mall::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mall.jpeg");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/Mall.jpeg";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void Mall::configurarObstaculos() {
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void Mall::onMovimientoUpdate() {
    verificarZombiesYMostrarMensaje();
}

bool Mall::eventFilter(QObject* obj, QEvent* event) {
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
                    Inventario::getInstance()->desbloquearPersonajeP3();
                    mostrarNotificacion("🎯 Felicidades, has conseguido el Francotirador\n🧍 Personaje P3 desbloqueado");
                    QTimer::singleShot(3000, this, [=]() {
                        mostrarNotificacion("🏬 Nivel completado...");
                        Caminos* c = new Caminos(jugador);
                        Inventario::getInstance()->setBalas(jugador->getMuniciones());
                        c->cambiarRuta(6); // <---- Manda a Ruta 6
                        c->posicionarJugadroEnCalleRuta6(); // <---- Debe existir este método
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

void Mall::keyPressEvent(QKeyEvent* event)
{
    AtributosPersonaje::keyPressEvent(event);
}

void Mall::mostrarNotificacion(const QString& texto) {
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

void Mall::verificarZombiesYMostrarMensaje() {
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
        mostrarNotificacion("🏆 ¡Has limpiado el Mall!\nPodés abrir el cofre.");
    }
}
