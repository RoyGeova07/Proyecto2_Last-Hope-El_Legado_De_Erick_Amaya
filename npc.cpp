#include "npc.h"
#include "dialogonpc.h"

NPC::NPC(Tipo tipo, QWidget* parent)
    : QLabel(parent),
    tipo(tipo),
    frameActual(0),
    indiceDialogo(0),
    hablando(false),
    labelPresionaH(nullptr)
{
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
    }
}

void NPC::SetAnimacion(const QString& ruta, int cantidadFrames)
{
    QPixmap spriteSheet(ruta);
    if (spriteSheet.isNull()) {
        qDebug() << "Error al cargar animacion:" << ruta;
        return;
    }

    frames.clear();
    frameActual = 0;

    int frameAncho = spriteSheet.width() / cantidadFrames;

    for(int i=0;i<cantidadFrames;++i)
    {

        QPixmap frame = spriteSheet.copy(i*frameAncho,0,frameAncho,spriteSheet.height());
        frames.append(frame.scaled(this->size(), Qt::KeepAspectRatio));

    }

    if (!frames.isEmpty()) {
        setPixmap(frames.first());
        animacionTimer->start(100);
    }
}

void NPC::AvanzarFrame()
{
    if(frames.isEmpty()||frames.size()<=1) return;

    frameActual=(frameActual+1)%frames.size();
    setPixmap(frames[frameActual]);
}

void NPC::mostrarDialogo(DialogoNPC* dialogoUI)
{
    if (dialogos.isEmpty() || !dialogoUI) return;

    hablando = true;
    dialogoActualUI = dialogoUI;

    //aqui si YA leyo completo, mostrar directamente el mensaje final
    if (yaHabloCompleto)
    {
        QString textoFinal;
        switch(tipo) {
        case Tipo::NPC1:
            textoFinal = "...La encontraste?";
            break;
        case Tipo::NPC2:
            textoFinal = "No tengo más botiquines, sigue el camino para encontrar más.";
            break;
        case Tipo::NPC3:
            textoFinal = "No tengo más botiquines, sigue el camino para encontrar más.";
            break;
        case Tipo::NPC4:
            textoFinal = "¿Recuperaste a mi perro?";
            break;
        default:
            textoFinal = "No tengo nada más que decirte...";
            break;
        }

        dialogoActualUI->mostrarDialogo(textoFinal, obtenerImagenNPC(), {});

        QTimer::singleShot(2000, dialogoActualUI, [this]() {
            if (dialogoActualUI)
                dialogoActualUI->ocultarDialogo();

            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        });

        return; // salir
    }


    //aqui si es la primera vez, mostrar los dialogos normales
    dialogoActual = dialogos.at(indiceDialogo);

    QPixmap imagenNPC = obtenerImagenNPC();
    QStringList opciones = obtenerOpcionesDialogo();

    dialogoActualUI->mostrarDialogo(dialogoActual, imagenNPC, opciones);

    disconnect(dialogoActualUI, &DialogoNPC::opcionSeleccionada, this, nullptr);
    connect(dialogoActualUI, &DialogoNPC::opcionSeleccionada, this, &NPC::manejarOpcionSeleccionada);
}

void NPC::mostrarHintInteractuar()
{
    if (!labelPresionaH) {
        labelPresionaH = new QLabel("Presiona H para hablar", this->parentWidget());
        labelPresionaH->setStyleSheet("background: rgba(0, 0, 0, 180); color: white; padding: 5px; border-radius: 5px;");
        labelPresionaH->setAlignment(Qt::AlignCenter);
        labelPresionaH->setWordWrap(true);
        labelPresionaH->setFixedSize(150, 30);
    }

    QPoint posicionLabel=this->pos();
    posicionLabel.setY(posicionLabel.y()-35);// Posicion sobre el NPC
    labelPresionaH->move(posicionLabel);
    labelPresionaH->show();
    labelPresionaH->raise();
}

void NPC::ocultarHintInteractuar()
{

    if (labelPresionaH)
        labelPresionaH->hide();

}

void NPC::manejarOpcionSeleccionada(int opcion)
{
    qDebug() << "Opcion seleccionada en NPC:" << opcion;

    QStringList opcionesActuales = obtenerOpcionesDialogo();

    if (opcion < 0 || opcion >= opcionesActuales.size())
        return;

    QString opcionTexto = opcionesActuales[opcion].toLower();

    if (opcionTexto.contains("entendido"))
    {
        indiceDialogo++;

        if (indiceDialogo < dialogos.size())
        {
            mostrarDialogo(dialogoActualUI);
        }
        else
        {
            //aqui MARCAMOS que ya habla completo
            yaHabloCompleto = true;

            //si el npc2 se le agrega el botiquin
            if(tipo==Tipo::NPC2&&inventarioRef)
            {

                inventarioRef->insertarObjeto("curar1",2,"Botiquin","Restaura vidaaa");
                qDebug() << "Botiquines agregados al inventario";

                mostrarNotificacion("¡Has obtenido un botiquín pequeño! Revisa tu inventario presionando I.");

            }

            if(tipo==Tipo::NPC3&&inventarioRef)
            {

                inventarioRef->insertarObjeto("curar2",1,"Botiquin","Restaura vidaaa");
                qDebug() << "Botiquines agregados al inventario";

                mostrarNotificacion("¡Has obtenido un botiquín grande! Revisa tu inventario presionando I.");

            }

            // NO mostramos el mensaje final esta primera vez
            if(dialogoActualUI)
            {
                dialogoActualUI->ocultarDialogo();
            }

            hablando = false;
            indiceDialogo = 0;
            emit dialogoTerminado();
        }
    }

    else if (opcionTexto.contains("adios"))
    {
        if (dialogoActualUI)
            dialogoActualUI->ocultarDialogo();

        hablando = false;
        indiceDialogo = 0;
        emit dialogoTerminado();

    }else{
        qDebug() << "Opcion secundaria seleccionada:" << opcionTexto;
    }
}
QPixmap NPC::obtenerImagenNPC() const
{
    QPixmap imagenCompleta;
    QRect areaCara;

    switch(tipo) {
    case Tipo::NPC1:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC1.png");
        areaCara = QRect(26, 40, 70, 70); // x, y, width, height
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
        return QPixmap();
    }

    if(imagenCompleta.isNull()) return QPixmap();
    return imagenCompleta.copy(areaCara).scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QStringList NPC::obtenerOpcionesDialogo() const
{
    switch(tipo) {
    case Tipo::NPC1:
        return {"Entendido", "Adios"};
    case Tipo::NPC2:
        return {"Entendido!", "Adios"};
    default:
        return {"Entendido", "Adios"};
    }
}

void NPC::mostrarNotificacion(const QString& texto)
{
    if (!labelNotificacion)
    {
        labelNotificacion = new QLabel(texto, this->parentWidget());
        labelNotificacion->setStyleSheet("background: rgba(0, 0, 0, 200); color: lime; font-size: 16px; padding: 10px; border-radius: 8px; border: 2px solid lime;");
        labelNotificacion->setAlignment(Qt::AlignCenter);
        labelNotificacion->setFixedSize(550,100);
    }

    labelNotificacion->setText(texto);

    // centrar arriba
    labelNotificacion->move((parentWidget()->width() - labelNotificacion->width()) / 2, 50);
    labelNotificacion->show();
    labelNotificacion->raise();

    //aqui ocultar automaticamente después de 2.5 segundos
    QTimer::singleShot(2500, labelNotificacion, [this]() {
        labelNotificacion->hide();
    });
}
