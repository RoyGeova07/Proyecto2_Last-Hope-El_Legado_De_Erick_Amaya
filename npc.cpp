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
            "¡Bienvenido, Erick!",
            "Ten 3 de Vida"
        };
        break;
    case Tipo::NPC2:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC2.png", 4);
        dialogos = {
            "OOH vaya miren que sorpresa si es el mismisimo Militar Erick. Te habias perdido, se que fue dificil perder a tus amigos, pero no es hora de lamentarse.",
            "Es hora de eliminar a estos zombies para poder conseguir ese antitodoto y curar a todos.",
            "Oye escuche rumores que tambien quieres con ese antitodoto en vez de recuperar la humanidad es controlar a los zombies.",
            "Espero tomes el camino correcto mi amigo."
        };
        break;
    case Tipo::NPC3:
        SetAnimacion(":/imagenes/assets/NPC/Hablar1_NPC3.png", 6);
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
    if(dialogos.isEmpty() || !dialogoUI || hablando) return;

    hablando = true;
    dialogoActual = dialogos.at(indiceDialogo);

    // Configurar el dialogo con imagen y opciones
    QPixmap imagenNPC = obtenerImagenNPC();
    QStringList opciones = obtenerOpcionesDialogo();

    dialogoUI->mostrarDialogo(dialogoActual, imagenNPC, opciones);

    connect(dialogoUI, &DialogoNPC::opcionSeleccionada,
            this, &NPC::manejarOpcionSeleccionada);

    indiceDialogo = (indiceDialogo + 1) % dialogos.size();
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

    switch(tipo) {
    case Tipo::NPC1:
        if(opcion == 0) {
        }
        break;
    case Tipo::NPC2:
        if(opcion == 1) {
        }
        break;
    case Tipo::NPC3:
        break;
    }

    hablando = false;
    emit dialogoTerminado();
}

QPixmap NPC::obtenerImagenNPC() const
{
    QPixmap imagenCompleta;
    QRect areaCara;

    switch(tipo) {
    case Tipo::NPC1:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC1.png");
        areaCara = QRect(30, 10, 70, 70); // x, y, width, height
        break;
    case Tipo::NPC2:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC2.png");
        areaCara = QRect(25, 50, 70, 70);
        break;
    case Tipo::NPC3:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Hablar1_NPC3.png");
        areaCara = QRect(35, 15, 60, 60);
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
        return {"Entendido", "¿Donde estan las armas?", "Gracias", "Adios"};
    case Tipo::NPC2:
        return {"Entendido!", "Adios"};
    case Tipo::NPC3:
        return {"Gracias", "¿Donde hay municiones?", "Adios"};
    default:
        return {"Entendido", "Adios"};
    }
}
