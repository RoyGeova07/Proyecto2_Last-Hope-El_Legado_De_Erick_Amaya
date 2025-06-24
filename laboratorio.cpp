#include "laboratorio.h"

#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include "lobby.h"
#include "inicio.h"
#include "caminos.h"
#include "Inventario.h"
#include "personaje.h"

laboratorio::laboratorio(personaje* jugadorExistente, QWidget* parent) : AtributosPersonaje(jugadorExistente, parent) {
    this->resize(1280, 720);
    this->setWindowTitle("Laboratorio - Last hope");

    configurarEscena();
    inicializarJugador();
    jugador->setMuniciones(Inventario::getInstance()->getBalas());
    jugador->move(16,538);
    configurarObstaculos();

    cofreCerrado = QPixmap(":/imagenes/assets/items/cofre_cerrado.png");
    cofreAbierto = QPixmap(":/imagenes/assets/items/cofre_abierto.png");

    btnSalir = new QPushButton("Abandonar Partida", this);
    btnSalir->setFocusPolicy(Qt::NoFocus);
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
    btnSalir->setGeometry(width()-180, 60, 170, 40);

    connect(btnSalir, &QPushButton::clicked, this, [=]() {
        this->hide();
        QTimer::singleShot(300, this, [=]() {
            Caminos* c = new Caminos(jugador);
            c->cambiarRuta(3); // Ajusta la ruta si necesitas
            c->posicionarJugadorEnCalleRuta3(); // O ajusta el método si es otro
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

    // --- AGREGA BOSS ---
    Zombie* boss = new Zombie(Zombie::Tipo::BOSS, this);
    boss->move(1050, 540); // Un poco atrás (más a la derecha)
    boss->setVelocidad(2); // Opcional, más lento por ser jefe
    boss->show();
    zombies.append(boss);

    // Los Z1 adelante y con espacio entre ellos
    int xBase = 920; // Más a la izquierda del boss
    int yBase = 530;  // Nivel inferior
    int yStep = 60;   // Espaciado vertical
    for (int i = 0; i < 3; ++i) {
        Zombie* z = new Zombie(Zombie::Tipo::Z1, this);
        z->move(xBase, yBase + i * yStep);
        z->show();
        zombies.append(z);
        xBase += 80;
    }

    // Conectar colisiones con jugador (BOSS y todos los Z1)
    for (Zombie* z : zombies) {
        z->perseguirJugador(jugador);

        connect(z, &Zombie::ColisionConJugador, this, [=]() {
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

void laboratorio::configurarEscena() {
    QPixmap fondoPixmap(":/imagenes/assets/lab.jpeg");
    if (fondoPixmap.isNull()) {
        qDebug() << "Error al cargar imagen desde assets/mapas/lab.jpeg";
    } else {
        QLabel* fondo = new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0, 0, width(), height());
    }
}

void laboratorio::configurarObstaculos() {
    obstaculos.append(QRect(3, 2, 1334, 470));     // Muro superior
    obstaculos.append(QRect(5, 669, 1273, 47));    // Piso inferior
    obstaculos.append(QRect(3, 278, 5, 388));      // Pared izquierda
    obstaculos.append(QRect(1272, 282, 3, 324));   // Pared derecha
}

void laboratorio::keyPressEvent(QKeyEvent* event) {
    // Si quieres bloquear el inventario:
    if(event->key()==Qt::Key_I) {
        return;
    }
    // Llama a la función base para el resto de teclas:
    AtributosPersonaje::keyPressEvent(event);
}

void laboratorio::onMovimientoUpdate() {
    if (jugador)
    {
        int distancia = std::abs(jugador->x() - 1142);
        if (std::abs(jugador->x() - 1142) <= 50 && std::abs(jugador->y() - 538) <= 50 && !cofreAbiertoYa)
        {
            bool zombiesVivos = false;
            for (Zombie* z : zombies)
                if (!z->muerto) {
                    zombiesVivos = true;
                    break;
                }
            if (zombiesVivos)
                mensajeCofre->setText("🔒 Cofre bloqueado");
            else
                mensajeCofre->setText("✅ Haz click para abrir");
            mensajeCofre->show();
        } else {
            mensajeCofre->hide();
        }
    }

    verificarZombiesYMostrarMensaje();
}

void laboratorio::mostrarNotificacion(const QString& texto)
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

bool laboratorio::eventFilter(QObject* obj, QEvent* event) {
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

                    jugador->setMuniciones(jugador->getMuniciones() + 50); // ¡Más premio por derrotar al jefe!
                    ActualizarMuniciones();

                    mostrarNotificacion("🏆 ¡Has derrotado al jefe! Recibes 50 municiones");

                    QTimer::singleShot(3000, this, [=]() {
                        mostrarNotificacion("🔬 ¡Laboratorio completado!");
                        Inventario::getInstance()->setBalas(jugador->getMuniciones());
                        Caminos* c = new Caminos(jugador);
                        c->cambiarRuta(3);
                        c->posicionarJugadorEnCalleRuta3();
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

void laboratorio::verificarZombiesYMostrarMensaje()
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
        mostrarNotificacion("🏆 ¡Has derrotado al jefe! Ahora puedes abrir el cofre.");
    }
}
