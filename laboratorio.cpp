#include "laboratorio.h"

#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <QKeyEvent>
#include "lobby.h"
#include "inicio.h"
#include "caminos.h"
#include "Inventario.h"
#include "personaje.h"
#include "npc.h"
#include "zombie.h"

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
            c->cambiarRuta(10);
            c->posicionarJugadorEnCalleRuta10();
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
    boss = new Zombie(Zombie::Tipo::BOSS, this);
    boss->move(1050, 540);
    boss->setVelocidad(3);
    boss->show();
    zombies.append(boss);

    // Los Z1 adelante y con espacio entre ellos
    int xBase = 920;
    int yBase = 530;
    int yStep = 60;
    for (int i = 0; i < 4; ++i) {
        Zombie* z = new Zombie(Zombie::Tipo::Z1, this);
        z->move(xBase, yBase + i * yStep);
        z->show();
        z->setVelocidad(4);
        zombies.append(z);
        xBase += 80;
    }

    // Sprite del antídoto (oculto al inicio, aparece en el piso al morir los zombies)
    antidotoLabel = new QLabel(this);
    antidotoLabel->setPixmap(QPixmap(":/imagenes/assets/items/antidotobueno.jpeg").scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    // Posición FIJA en el piso, elige la que quieras (ejemplo: x=1020, y=620)
    antidotoLabel->setGeometry(1020, 620, 64, 64);
    antidotoLabel->hide();

    mensajeAntidoto = new QLabel(this);
    mensajeAntidoto->setStyleSheet(
        "background: rgba(0,0,0,210);"
        "color: #63ffea;"
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 20px 24px;"
        "border-radius: 18px;"
        "border: 3px solid #37cfff;"

        );

    mensajeAntidoto->setAlignment(Qt::AlignCenter);
    mensajeAntidoto->setWordWrap(true);
    mensajeAntidoto->setFixedSize(420, 90);
    mensajeAntidoto->move(antidotoLabel->x() - 120, antidotoLabel->y() - 100); // Ajusta según prefieras
    mensajeAntidoto->hide();

    // Lógica de colisión de zombies
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
                        inv->eliminarObjeto("casco");
                        inv->eliminarObjeto("chaleco");
                        jugador->setEscudo(0);

                        this->hide();
                        QTimer::singleShot(300, this, [=]()
                                           {
                                               Inicio*i=new Inicio();
                                               i->show();
                                               deleteLater();
                                           });
                        this->close();
                    });
                }
            }
        });
    }

    antidotoRecogido = false;
    finalYaDesplegado = false;
    cofreAbiertoYa = false;
    llaveUsadaEnFinal = false;
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
    obstaculos.append(QRect(3, 2, 1334, 470));
    obstaculos.append(QRect(5, 669, 1273, 47));
    obstaculos.append(QRect(3, 278, 5, 388));
    obstaculos.append(QRect(1272, 282, 3, 324));
}

void laboratorio::keyPressEvent(QKeyEvent* event) {
    if (finalYaDesplegado) return;
    if(event->key()==Qt::Key_I) {
        return;
    }
    // Interacción con antídoto
    if(event->key()==Qt::Key_E && !antidotoRecogido && !zombiesVivos()) {
        if(estaCercaAntidoto()) {
            recogerAntidoto();
            return;
        }
    }
    // Interacción con cofre con llave final
    if(event->key()==Qt::Key_F && antidotoRecogido && !cofreAbiertoYa && tieneLlaveFinal() && jugadorEstaCercaCofre()) {
        abrirCofreFinal();
        return;
    }
    AtributosPersonaje::keyPressEvent(event);
}

void laboratorio::onMovimientoUpdate() {
    if (jugador) {
        // Antídoto aparece al morir el boss
        if (!antidotoRecogido && !zombiesVivos() && !antidotoLabel->isVisible()) {
            antidotoLabel->show();
        }
        // Mostrar mensaje para recoger antídoto
        if (!antidotoRecogido && antidotoLabel->isVisible() && estaCercaAntidoto()) {
            mensajeAntidoto->setText("Presiona E para\nrecuperar el antídoto");
            mensajeAntidoto->show();
        } else {
            mensajeAntidoto->hide();
        }
        // Quita cualquier mensaje de cofre aquí
        mensajeCofre->hide();
    }
    verificarZombiesYMostrarMensaje();
}


void laboratorio::mostrarNotificacion(const QString& texto) {
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

bool laboratorio::eventFilter(QObject* obj, QEvent* event) {
    if (obj == cofreLabel) {
        if (event->type() == QEvent::Enter) {
            // Mouse entra en el cofre
            if (cofreAbiertoYa) {
                mensajeCofre->setText("🎁 Cofre abierto");
            } else if (zombiesVivos()) {
                mensajeCofre->setText("🔒 Cofre bloqueado");
            } else if (antidotoRecogido && tieneLlaveFinal()) {
                mensajeCofre->setText("✅ Haz click para abrir");
            } else {
                mensajeCofre->setText("🔒 Cofre bloqueado");
            }
            mensajeCofre->show();
        }
        else if (event->type() == QEvent::Leave) {
            mensajeCofre->hide();
        }
        else if (event->type() == QEvent::MouseButtonPress) {
            // Solo puedes abrirlo con click, si cumples requisitos
            if (!cofreAbiertoYa && antidotoRecogido && tieneLlaveFinal() && !zombiesVivos()) {
                abrirCofreFinal();
                return true;
            }
            // Si no cumple requisitos, no hace nada especial
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}



void laboratorio::verificarZombiesYMostrarMensaje() {
    static bool mensajeMostrado = false;
    if (mensajeMostrado) return;
    if (!zombiesVivos()) {
        mensajeMostrado = true;
        mostrarNotificacion("🏆 ¡Has derrotado al jefe! Busca el antídoto.");
    }
}

// === Funciones auxiliares para lógica de finales ===

bool laboratorio::zombiesVivos() {
    for (Zombie* z : zombies)
        if (!z->muerto)
            return true;
    return false;
}

bool laboratorio::estaCercaAntidoto() {
    QRect areaAntidoto = antidotoLabel->geometry().adjusted(-35,-35,35,35);
    return areaAntidoto.intersects(jugador->geometry());
}

bool laboratorio::jugadorEstaCercaCofre() {
    QRect areaCofre = cofreLabel->geometry().adjusted(-25,-25,25,25);
    return areaCofre.intersects(jugador->geometry());
}

bool laboratorio::tieneLlaveFinal() {
    return Inventario::getInstance()->objetoExiste("llave");
}

void laboratorio::recogerAntidoto() {
    antidotoRecogido = true;
    antidotoLabel->hide();
    mensajeAntidoto->hide();
    mostrarNotificacion("✔️ Has recuperado el antídoto del jefe.");
    // Si tiene llave en inventario, habilitar cofre especial
    if (tieneLlaveFinal()) {
        mostrarNotificacion("¡Posees una llave especial! Busca el cofre. ");
    } else {
        // Popup de decisión: Solo salvar el mundo
        QTimer::singleShot(1200, this, [=]() {
            QMessageBox::information(this, "Decisión final", "¿Usar el antídoto para salvar el mundo?");
            mostrarFinalStandard();
        });
    }
}

void laboratorio::abrirCofreFinal() {
    cofreAbiertoYa = true;
    cofreLabel->setPixmap(cofreAbierto.scaled(64, 64));
    mensajeCofre->hide();
    mostrarDialogoFinalBonito(); // Llama al nuevo diálogo visual
}

void laboratorio::mostrarDialogoFinalBonito() {
    QWidget* dialogo = new QWidget(this, Qt::Dialog | Qt::FramelessWindowHint);
    dialogo->setAttribute(Qt::WA_DeleteOnClose);
    dialogo->setFixedSize(540, 330);
    dialogo->move((width() - dialogo->width()) / 2, (height() - dialogo->height()) / 2);
    dialogo->setStyleSheet(
        "background: qlineargradient(y1:0, y2:1, stop:0 #21244d, stop:1 #42617c);"
        "border-radius: 32px;"
        "border: 4px solid #44ffe5;"
        );

    QLabel* titulo = new QLabel("💀 El destino está en tus manos", dialogo);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setGeometry(0, 32, dialogo->width(), 40);
    titulo->setStyleSheet("color: #42ffdf; font-size: 26px; font-weight: bold;");

    QLabel* pregunta = new QLabel(
        "¿Qué deseas hacer con el antídoto?<br><br>"
        "<b>Salvar el mundo</b> o <b>Controlar a los zombies</b>",
        dialogo);
    pregunta->setAlignment(Qt::AlignCenter);
    pregunta->setWordWrap(true);
    pregunta->setGeometry(30, 82, dialogo->width()-60, 60);
    pregunta->setStyleSheet("color: #e0fff5; font-size: 20px; font-weight: 500;");

    QPushButton* btnSalvar = new QPushButton("Salvar el mundo", dialogo);
    QPushButton* btnControl = new QPushButton("Controlar a los zombies", dialogo);

    btnSalvar->setGeometry(70, 190, 180, 64);
    btnControl->setGeometry(290, 190, 180, 64);

    btnSalvar->setStyleSheet(
        "QPushButton { background-color: #43ef9d; color: #222; font-weight: bold; font-size: 18px; border-radius: 18px; border: 2px solid #69ffd8; }"
        "QPushButton:hover { background-color: #78ffd7; }"
        );
    btnControl->setStyleSheet(
        "QPushButton { background-color: #fd6767; color: #fff; font-weight: bold; font-size: 18px; border-radius: 18px; border: 2px solid #ffbbbb; }"
        "QPushButton:hover { background-color: #ff8585; }"
        );

    connect(btnSalvar, &QPushButton::clicked, this, [=]() {
        dialogo->close();
        mostrarFinalStandard();
    });
    connect(btnControl, &QPushButton::clicked, this, [=]() {
        Inventario::getInstance()->eliminarObjeto("llave");
        dialogo->close();
        mostrarFinalZombies();
    });

    dialogo->show();
    dialogo->raise();
    dialogo->activateWindow();
}

// Final estándar: aparecen NPCs y mensaje de victoria
void laboratorio::mostrarFinalStandard() {
    if (finalYaDesplegado) return;
    finalYaDesplegado = true;
    deshabilitarControlesFinal();
    // Genera NPCs
    QVector<NPC*> npcsFinales;
    int npcsMostrar = 10;
    int startX = 180;
    int ySuelo = 480; // Ajusta este valor si tu piso es otro
    for (int i=0; i<npcsMostrar; ++i) {
        NPC* npc = new NPC(static_cast<NPC::Tipo>((i%6)), this);
        int x = startX + (i*100); // Espaciado horizontal
        npc->move(x, ySuelo);
        npc->show();
        npcsFinales.append(npc);
    }
    // Mensaje arriba
    QLabel* textoFinal = new QLabel("🎉 Felicidades por salvar el mundo 🎉", this);
    textoFinal->setStyleSheet("background: rgba(0,0,0,180); color: #8fff85; font-size: 32px; font-weight: bold; padding: 14px; border-radius: 20px;");
    textoFinal->setAlignment(Qt::AlignCenter);
    textoFinal->setGeometry((width()-600)/2, 40, 600, 64);
    textoFinal->show();

    QTimer::singleShot(6000, this, [=](){
        QMessageBox::information(this, "THE END", "Has completado el juego. ¡Gracias por jugar!");
        QApplication::quit();
    });
}

// Final alternativo: aparecen zombies y mensaje de control
void laboratorio::mostrarFinalZombies() {
    if (finalYaDesplegado) return;
    finalYaDesplegado = true;
    deshabilitarControlesFinal();

    // Zombies alineados en el piso (misma altura que el personaje principal)
    QVector<Zombie*> zombiesFinales;
    int zombiesMostrar = 16;
    int startX = 150;
    int ySuelo = 480; // O el valor Y de tu personaje
    for (int i=0; i<zombiesMostrar; ++i) {
        Zombie* z = new Zombie(Zombie::Tipo::Z1, this);
        int x = startX + (i*80); // Espaciado horizontal
        z->move(x, ySuelo);
        z->setVelocidad(0);
        z->show();
        zombiesFinales.append(z);
    }

    QLabel* textoFinal = new QLabel("🧟‍♂️ Ahora eres el amo de los zombies... Final alternativo alcanzado 🧟‍♂️", this);
    textoFinal->setStyleSheet("background: rgba(0,0,0,200); color: #ff6666; font-size: 32px; font-weight: bold; padding: 14px; border-radius: 20px;");
    textoFinal->setAlignment(Qt::AlignCenter);
    textoFinal->setGeometry((width()-800)/2, 40, 800, 64);
    textoFinal->show();

    QTimer::singleShot(6000, this, [=](){
        QMessageBox::information(this, "THE END", "Has completado el juego... a tu manera.\n¡Gracias por jugar!");
        QApplication::quit();
    });
}

// Bloquea todos los controles del jugador e impide interacción tras el final
void laboratorio::deshabilitarControlesFinal() {
    puedeDisparar = false;
    if (movimientoTimer) movimientoTimer->stop();
    jugador->setEnabled(false);
    btnSalir->setEnabled(false);
    this->setFocusPolicy(Qt::NoFocus);
    // Puedes ocultar los HUD si quieres aquí también.
}
