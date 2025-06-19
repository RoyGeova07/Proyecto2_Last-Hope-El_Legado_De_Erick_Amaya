#include "npc.h"
#include "dialogonpc.h"
#include <QDebug>

NPC::NPC(Tipo tipo, QWidget* parent)
    : QLabel(parent),
    tipo(tipo),
    frameActual(0),
    indiceDialogo(0),
    hablando(false),
    labelPresionaH(nullptr),
    labelNotificacion(nullptr),
    arbolDialogos(nullptr),
    nodoActual(nullptr),
    yaHabloCompleto(false),
    inventarioRef(nullptr),
    dialogoActualUI(nullptr)
{
    qDebug() << "Creando NPC tipo:" << static_cast<int>(tipo);

    this->setFixedSize(128, 128);
    this->move(600, 500);

    animacionTimer = new QTimer(this);
    connect(animacionTimer, &QTimer::timeout, this, &NPC::AvanzarFrame);

    dialogoTimer = new QTimer(this);
    dialogoTimer->setSingleShot(true);

    switch(tipo) {
    case Tipo::NPC1:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC1.png", 7);
        dialogos = {
            "Oye, perdí mis llaves en la Ciudad en Ruinas. ¿Me ayudas a buscarlas?",
            "Ten cuidado, esta llena de zombies..."
        };
        break;
    case Tipo::NPC2:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC2.png", 4);
        dialogos = {
            "¡OOH, vaya! Miren que sorpresa... si no es el mismísimo militar Erick. Te habías perdido, sé que fue difícil perder a tus amigos, pero no es hora de lamentarse.",
            "Es hora de eliminar a estos zombies para poder conseguir ese antídoto y curar a todos.",
            "Oye, escuché rumores que también quieres ese antídoto, pero en lugar de recuperar a la humanidad, quieres controlar a los zombies.",
            "Espero tomes el camino correcto mi amigo, aquí te doy 2 botiquines, úsalos bien en tu batalla."
        };
        break;
    case Tipo::NPC3:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC3.png", 6);
        dialogos = {
            "Ten cuidado con los enemigos",
            "El gimnasio tiene municiones!!"
        };
        break;
    case Tipo::NPC4:
        SetAnimacion(":/imagenes/assets/NPC/Idle_NPC4.png", 6);
        dialogos = {
            "Ayudame! Secuestraron a mi perro en el mall!!"
        };
        break;
    case Tipo::NPC5:
        SetAnimacion(":/imagenes/assets/NPC/Idle_NPC5.png", 6);
        dialogos = {
            "Ten cuidado con los enemigos",
            "Callejón C tiene municiones!!"
        };
        break;
    case Tipo::NPC6:
        SetAnimacion(":/imagenes/assets/NPC/Idle_NPC6.png", 6);
        dialogos = {
            "Ten cuidado con los enemigos",
            "Callejón C tiene municiones!!"
        };
        break;
    default:
        qDebug() << "Tipo de NPC no reconocido";
        break;
    }

    qDebug() << "Diálogos inicializados:" << dialogos.size();
}

void NPC::SetAnimacion(const QString& ruta, int cantidadFrames)
{
    qDebug() << "Cargando animación:" << ruta;

    QPixmap spriteSheet(ruta);
    if (spriteSheet.isNull()) {
        qDebug() << "Error al cargar animacion:" << ruta;
        return;
    }

    frames.clear();
    frameActual = 0;

    int frameAncho = spriteSheet.width() / cantidadFrames;

    for(int i=0; i<cantidadFrames; ++i) {
        QPixmap frame = spriteSheet.copy(i*frameAncho, 0, frameAncho, spriteSheet.height());
        frames.append(frame.scaled(this->size(), Qt::KeepAspectRatio));
    }

    if (!frames.isEmpty()) {
        setPixmap(frames.first());
        animacionTimer->start(100);
        qDebug() << "Animación iniciada con" << frames.size() << "frames";
    }
}

void NPC::AvanzarFrame()
{
    if(frames.isEmpty() || frames.size() <= 1) {
        qDebug() << "No hay suficientes frames para animar";
        return;
    }

    frameActual = (frameActual + 1) % frames.size();
    setPixmap(frames[frameActual]);
}

void NPC::mostrarDialogo(DialogoNPC* dialogoUI)
{
    qDebug() << "=== INICIANDO MOSTRAR DIÁLOGO ===";
    qDebug() << "NPC tipo:" << static_cast<int>(tipo);
    qDebug() << "Dialogo UI válido:" << (dialogoUI != nullptr);
    qDebug() << "Índice actual:" << indiceDialogo;
    qDebug() << "Total diálogos:" << dialogos.size();

    if (dialogos.isEmpty() || !dialogoUI) {
        qDebug() << "Error: Diálogos vacíos o UI inválida";
        return;
    }

    hablando = true;
    dialogoActualUI = dialogoUI;

    if (yaHabloCompleto) {
        qDebug() << "Mostrando diálogo final";
        QString textoFinal;
        QStringList opcionesFinales;

        switch(tipo) {
        case Tipo::NPC1:
            textoFinal = tieneItem("llave")
                             ? "¡Encontraste mi llave! ¿Quieres intercambiarla por municiones?"
                             : "¿Trajiste mi llave?";
            opcionesFinales = {"Sí", "No"};
            break;
        case Tipo::NPC2:
            textoFinal = "No tengo nada más que decir...";
            opcionesFinales = {"Entendido"};
            break;
        case Tipo::NPC3:
            textoFinal = "¿Necesitas más ayuda?";
            opcionesFinales = {"Sí", "No"};
            break;
        case Tipo::NPC4:
            textoFinal = "¿Encontraste a mi perro?";
            opcionesFinales = {"Sí", "No"};
            break;
        default:
            textoFinal = "No tengo nada más que decirte...";
            opcionesFinales = {"Entendido"};
            break;
        }

        qDebug() << "Texto final:" << textoFinal;
        dialogoUI->mostrarDialogo(textoFinal, obtenerImagenNPC(), opcionesFinales);
        return;
    }

    qDebug() << "Mostrando diálogo normal";
    dialogoActual = dialogos.at(indiceDialogo);
    QPixmap imagenNPC = obtenerImagenNPC();
    QStringList opciones = obtenerOpcionesDialogo();

    qDebug() << "Diálogo actual:" << dialogoActual;
    dialogoUI->mostrarDialogo(dialogoActual, imagenNPC, opciones);

    disconnect(dialogoActualUI, &DialogoNPC::opcionSeleccionada, this, nullptr);
    connect(dialogoActualUI, &DialogoNPC::opcionSeleccionada, this, &NPC::manejarOpcionSeleccionada);
}

void NPC::mostrarHintInteractuar()
{
    qDebug() << "Mostrando hint de interacción";

    if (!labelPresionaH) {
        labelPresionaH = new QLabel("Presiona H para hablar", this->parentWidget());
        labelPresionaH->setStyleSheet("background: rgba(0, 0, 0, 180); color: white; padding: 5px; border-radius: 5px;");
        labelPresionaH->setAlignment(Qt::AlignCenter);
        labelPresionaH->setWordWrap(true);
        labelPresionaH->setFixedSize(150, 30);
        qDebug() << "Label de hint creado";
    }

    QPoint posicionLabel = this->pos();
    posicionLabel.setY(posicionLabel.y() - 35);
    labelPresionaH->move(posicionLabel);
    labelPresionaH->show();
    labelPresionaH->raise();
}

void NPC::ocultarHintInteractuar()
{
    qDebug() << "Ocultando hint de interacción";

    if (labelPresionaH) {
        labelPresionaH->hide();
    }
}

void NPC::manejarOpcionSeleccionada(int opcion)
{
    qDebug() << "=== MANEJANDO OPCIÓN ===";
    qDebug() << "Opción seleccionada:" << opcion;
    qDebug() << "NPC tipo:" << static_cast<int>(tipo);
    qDebug() << "Ya habló completo:" << yaHabloCompleto;

    QStringList opciones = obtenerOpcionesDialogo();
    qDebug() << "Opciones disponibles:" << opciones;

    if (opcion < 0 || opcion >= opciones.size()) {
        qDebug() << "Opción inválida";
        return;
    }

    QString opcionTexto = opciones[opcion];
    qDebug() << "Texto de opción:" << opcionTexto;

    switch(tipo) {
    case Tipo::NPC1:
        if (!yaHabloCompleto) {
            if (opcionTexto == "Entendido") {
                indiceDialogo++;
                if (indiceDialogo < dialogos.size()) {
                    mostrarDialogo(dialogoActualUI);
                } else {
                    yaHabloCompleto = true;
                    emit dialogoTerminado();
                }
            } else if (opcionTexto == "Adios") {
                dialogoActualUI->ocultarDialogo();
                hablando = false;
                indiceDialogo = 0;
                emit dialogoTerminado();
            }
        } else {
            if (opcionTexto == "Sí" && tieneItem("llave") && inventarioRef) {
                inventarioRef->eliminarObjeto("llave");
                inventarioRef->insertarObjeto("municiones", 1, "Munición", "Balas 9mm");
                mostrarNotificacion("¡Intercambio realizado! Recibiste municiones.");
            }
            dialogoActualUI->ocultarDialogo();
            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        }
        break;

    case Tipo::NPC2:
        if (!yaHabloCompleto) {
            if (opcionTexto == "Entendido") {
                indiceDialogo++;
                if (indiceDialogo < dialogos.size()) {
                    mostrarDialogo(dialogoActualUI);
                } else {
                    yaHabloCompleto = true;
                    if (inventarioRef) {
                        inventarioRef->insertarObjeto("curar1", 2, "Botiquín", "Cura vida");
                        mostrarNotificacion("¡Obtuviste 2 botiquines! Presiona L para ver tu inventario.");
                    }
                    emit dialogoTerminado();
                }
            } else if (opcionTexto == "Adios") {
                dialogoActualUI->ocultarDialogo();
                hablando = false;
                indiceDialogo = 0;
                emit dialogoTerminado();
            }
        } else {
            dialogoActualUI->ocultarDialogo();
            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        }
        break;

    case Tipo::NPC3:
        if (yaHabloCompleto) {
            if (opcionTexto == "Sí") {
                mostrarNotificacion("Busca las municiones en el gimnasio");
            }
            dialogoActualUI->ocultarDialogo();
            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        } else {
            if (opcionTexto == "Entendido") {
                indiceDialogo++;
                if (indiceDialogo < dialogos.size()) {
                    mostrarDialogo(dialogoActualUI);
                } else {
                    yaHabloCompleto = true;
                    emit dialogoTerminado();
                }
            } else if (opcionTexto == "Adios") {
                dialogoActualUI->ocultarDialogo();
                hablando = false;
                indiceDialogo = 0;
                emit dialogoTerminado();
            }
        }
        break;

    case Tipo::NPC4:
        if (yaHabloCompleto) {
            if (opcionTexto == "Sí") {
                mostrarNotificacion("Busca el perro en el mall");
            }
            dialogoActualUI->ocultarDialogo();
            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        } else {
            if (opcionTexto == "Entendido") {
                indiceDialogo++;
                if (indiceDialogo < dialogos.size()) {
                    mostrarDialogo(dialogoActualUI);
                } else {
                    yaHabloCompleto = true;
                    emit dialogoTerminado();
                }
            } else if (opcionTexto == "Adios") {
                dialogoActualUI->ocultarDialogo();
                hablando = false;
                indiceDialogo = 0;
                emit dialogoTerminado();
            }
        }
        break;

    default:
        if (opcionTexto == "Entendido") {
            indiceDialogo++;
            if (indiceDialogo < dialogos.size()) {
                mostrarDialogo(dialogoActualUI);
            } else {
                yaHabloCompleto = true;
                emit dialogoTerminado();
            }
        } else if (opcionTexto == "Adios") {
            dialogoActualUI->ocultarDialogo();
            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        }
        break;
    }
}

QPixmap NPC::obtenerImagenNPC() const
{
    qDebug() << "Obteniendo imagen para NPC tipo:" << static_cast<int>(tipo);

    QPixmap imagenCompleta;
    QRect areaCara;

    switch(tipo) {
    case Tipo::NPC1:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC1.png");
        areaCara = QRect(26, 40, 70, 70);
        break;
    case Tipo::NPC2:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC2.png");
        areaCara = QRect(25, 50, 70, 70);
        break;
    case Tipo::NPC3:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC3.png");
        areaCara = QRect(26, 40, 60, 60);
        break;
    case Tipo::NPC4:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Idle_NPC4.png");
        areaCara = QRect(26, 40, 60, 60);
        break;
    case Tipo::NPC5:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Idle_NPC5.png");
        areaCara = QRect(26, 40, 60, 60);
        break;
    case Tipo::NPC6:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Idle_NPC6.png");
        areaCara = QRect(26, 40, 60, 60);
        break;
    default:
        qDebug() << "Tipo de NPC no reconocido para imagen";
        return QPixmap();
    }

    if(imagenCompleta.isNull()) {
        qDebug() << "Error al cargar imagen para NPC";
        return QPixmap();
    }

    qDebug() << "Imagen cargada correctamente";
    return imagenCompleta.copy(areaCara).scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QStringList NPC::obtenerOpcionesDialogo() const
{
    qDebug() << "Obteniendo opciones para NPC tipo:" << static_cast<int>(tipo);

    switch(tipo) {
    case Tipo::NPC1:
        return yaHabloCompleto ? QStringList{"Sí", "No"} : QStringList{"Entendido", "Adios"};
    case Tipo::NPC2:
        return yaHabloCompleto ? QStringList{"Entendido"} : QStringList{"Entendido", "Adios"};
    case Tipo::NPC3:
        return yaHabloCompleto ? QStringList{"Sí", "No"} : QStringList{"Entendido", "Adios"};
    case Tipo::NPC4:
        return {"Entendido", "Adios"};
    case Tipo::NPC5:
        return {"Entendido", "Adios"};
    case Tipo::NPC6:
        return {"Entendido", "Adios"};
    default:
        qDebug() << "Tipo de NPC no reconocido para opciones";
        return {"Entendido", "Adios"};
    }
}

void NPC::mostrarNotificacion(const QString& texto)
{
    qDebug() << "Mostrando notificación:" << texto;

    if (!labelNotificacion) {
        labelNotificacion = new QLabel(texto, this->parentWidget());
        labelNotificacion->setStyleSheet("background: rgba(0, 0, 0, 200); color: lime; font-size: 16px; padding: 10px; border-radius: 8px; border: 2px solid lime;");
        labelNotificacion->setAlignment(Qt::AlignCenter);
        labelNotificacion->setFixedSize(550, 100);
        qDebug() << "Label de notificación creado";
    }

    labelNotificacion->setText(texto);
    labelNotificacion->move((parentWidget()->width() - labelNotificacion->width()) / 2, 50);
    labelNotificacion->show();
    labelNotificacion->raise();

    QTimer::singleShot(2500, this, [this]() {
        qDebug() << "Ocultando notificación";
        if (labelNotificacion) {
            labelNotificacion->hide();
        }
    });
}

bool NPC::tieneItem(const QString& nombreItem)
{
    qDebug() << "Verificando si tiene item:" << nombreItem;

    if (!inventarioRef) {
        qDebug() << "Error: inventarioRef es nullptr";
        return false;
    }

    NodoInventario* nodoEncontrado = inventarioRef->buscar(inventarioRef->obtenerRaiz(), nombreItem);

    if (nodoEncontrado) {
        qDebug() << "Item encontrado en inventario. Cantidad:" << nodoEncontrado->cantidad;
        return true;
    } else {
        qDebug() << "Item NO encontrado en inventario";
        return false;
    }
}
