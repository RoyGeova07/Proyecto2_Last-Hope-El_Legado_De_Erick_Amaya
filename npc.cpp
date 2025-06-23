#include "npc.h"
#include "dialogonpc.h"
#include"QDebug"

NPC::NPC(Tipo tipo, QWidget* parent): QLabel(parent),tipo(tipo), tablaNPCs(TablaHash::getInstance())
{
    this->setFixedSize(128,128);
    this->move(600,500);

    animacionTimer=new QTimer(this);
    connect(animacionTimer, &QTimer::timeout, this, &NPC::AvanzarFrame);

    /* ---------- VISUAL (sprite idle por defecto) ---------- */
    switch (tipo)
    {
    case Tipo::NPC1: SetAnimacion(":/imagenes/assets/NPC/Idle_NPC1.png", 7); break;
    case Tipo::NPC2: SetAnimacion(":/imagenes/assets/NPC/Idle_NPC2.png", 4); break;
    case Tipo::NPC3: SetAnimacion(":/imagenes/assets/NPC/Idle_NPC3.png", 6); break;
    case Tipo::NPC4: SetAnimacion(":/imagenes/assets/NPC/Idle_NPC4.png",     6); break;
    case Tipo::NPC5: SetAnimacion(":/imagenes/assets/NPC/Idle_NPC5.png",     7); break;
    case Tipo::NPC6: SetAnimacion(":/imagenes/assets/NPC/Idle_NPC6.png",     6); break;
    }

    construirArbolDecisiones();
    nodoActual=arbolDialogos;

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
    if(!dialogoUI)return;

    // Marcar como descubierto SOLO cuando inicias diálogo
    QString npcKey = QString("NPC%1").arg(static_cast<int>(tipo) + 1);
    TablaHash::getInstance().descubrir(npcKey);
    qDebug() << "Descubierto:" << npcKey << TablaHash::getInstance().estaDescubierto(npcKey);

    // *** si la recompensa ya fue dada mostramos un SUERTE sin botones ***
    if(recompensaEntregada)
    {

        dialogoUI->mostrarDialogo("SUERTE EN TU BATALLA, DE SALVAR AL MUNDO O CONQUISTARLO!!!",obtenerImagenNPC(),QStringList{});//qstringlist me devuelve una lista vacia

        // Ocultar automaticamente luego de 3.5 segundos (3500 ms)
        QTimer::singleShot(3500,dialogoUI,[dialogoUI]()
                           {

                               dialogoUI->ocultarDialogo();

                           });

        return;

    }

    if(!nodoActual)return;
    hablando = true;
    dialogoActualUI = dialogoUI;

    dialogoUI->mostrarDialogo(nodoActual->texto,obtenerImagenNPC(),nodoActual->opciones);

    disconnect(dialogoUI,nullptr,this,nullptr);//limpiaaaaaaaaa
    connect(dialogoUI,&DialogoNPC::opcionSeleccionada,this,&NPC::manejarOpcionSeleccionada);

    //aqui si YA leyo completo, mostrar directamente el mensaje final

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

    if(!nodoActual)return;

    /* Si la opcion seleccionada existe dentro de hijos -> avanzar nodo */
    if(opcion>=0&&opcion<nodoActual->hijos.size())
    {
        nodoActual=nodoActual->hijos[opcion];
        mostrarDialogo(dialogoActualUI);
        return;
    }

    /* hoja -> ejecutar recompensa / castigo */
    ejecutarConsecuencia(nodoActual);

    dialogoActualUI->ocultarDialogo();
    hablando=false;

    if(!recompensaEntregada)
    {

        nodoActual=arbolDialogos;// solo reinicia si la misión sigue activa

    }else{

        nodoActual=nullptr;// ya no se usara el arbol

    }

    emit dialogoTerminado();
}

QPixmap NPC::obtenerImagenNPC() const
{
    QPixmap imagenCompleta;
    QRect areaCara;

    switch(tipo) {
    case Tipo::NPC1:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Idle_NPC1.png");
        areaCara = QRect(26, 40, 70, 70); // x, y, width, height
        break;
    case Tipo::NPC2:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Idle_NPC2.png");
        areaCara = QRect(25, 50, 70, 70);
        break;
    case Tipo::NPC3:
        imagenCompleta = QPixmap(":/imagenes/assets/NPC/Idle_NPC3.png");
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

void NPC::construirArbolDecisiones()
{
    switch (tipo)
    {
    /* ───────── NPC 1 ───────── */
    case Tipo::NPC1: {
        //raizzzz

        //Marcar como npc descubierto
        qDebug() << "Estado NPC1:" << TablaHash::getInstance().estaDescubierto("NPC1");

        arbolDialogos=new NodoDialogo(
            "Perdí mis llaves en la Ciudad en Ruinas. ¿Me ayudas a buscarlas?",
            {"Sí, te ayudaré", "No puedo"}
            );

        auto n1_si=new NodoDialogo("¡Gracias! Ten cuidado, está llena de zombies…",
                                     {"Entendido"});
        auto n1_no=new NodoDialogo("Oh… Entiendo. Si cambias de opinión, estaré aquí.",
                                     {"Adiós"});

        auto n1_llave=new NodoDialogo("¡Encontraste mi llave! ¿Quieres intercambiarla por municiones?",
                                        {"Sí", "No"});

        auto n2_si=new NodoDialogo("¡Aqui tienes!",
                                     {"Gracias"}, "DAR_MUNICIONES");
        auto n2_no=new NodoDialogo("Oh… Entiendo. Si cambias de opinión, estaré aquí.",
                                     {"Adiós"});

        arbolDialogos->hijos<<n1_si<<n1_no;
        n1_llave->hijos<<n2_si<<n2_no;

        if (Inventario::getInstance()->objetoExiste("llave")) {
            n1_si->hijos << n1_llave; // ruta cuando regreses con la llave
        }
        break;
    }

    /* ───────── NPC 2 ───────── */
    case Tipo::NPC2: {
        arbolDialogos=new NodoDialogo(
            "Te habías perdido… Fue duro perder a tus amigos, pero no es hora de lamentarse.\n"
            "¿Quieres 2 botiquines para tu batalla?",
            {"Sí", "No"}
            );

        auto n2_si=new NodoDialogo("Aquí tienes 2 botiquines. ¡Úsalos bien!",
                                     {"Gracias"},
                                     "DAR_CURAR1x2");
        auto n2_no=new NodoDialogo("Como quieras… ¡Suerte!",
                                     {"Adiós"});

        arbolDialogos->hijos<<n2_si<<n2_no;
        break;
    }

    /* ───────── NPC 3 ───────── */
    case Tipo::NPC3: {
        arbolDialogos=new NodoDialogo(
            "Ten cuidado con los zombies ya por estos caminos, hay mas zombies. El gimnasio tiene municiones.\n"
            "¿Quieres botiquines más potentes (x2)?",
            {"Sí", "No"}
            );

        auto g1 = new NodoDialogo("Toma, te servirán mucho.",
                                  {"Gracias"},
                                  "DAR_CURAR2x2");
        auto g2 = new NodoDialogo("De acuerdo, ¡cuídate!",
                                  {"Adiós"});

        arbolDialogos->hijos<<g1<<g2;
        break;
    }

    /* ───────── NPC 4 ───────── */
    case Tipo::NPC4: {
        arbolDialogos = new NodoDialogo(
            "¿Podrías darme una caja de municiones? Te compensaré.",
            {"Sí","No"});

        auto ok   = new NodoDialogo("¡Gracias! A cambio toma 2 curaciones pequeñas.",
                                  {"Entendido"},
                                  "QUITAR_MUNICIONES");
        auto bye  = new NodoDialogo("Bueno… volveré a intentarlo luego.",
                                   {"Adiós"});

        arbolDialogos->hijos << ok << bye;
        break;
    }

    /* ───────── NPC 5 ───────── */
    case Tipo::NPC5: {
        arbolDialogos=new NodoDialogo(
            "Creo que deje unos chalecos en el supermercado, ¿Me ayudas a recuperarlos?",
            {"Sí", "No"}
            );
        auto cSi=new NodoDialogo("¡Gracias! Necesitaras granadas para ir...",
                                   {"Entendido"}, "DAR_GRANADAS");
        auto cNo=new NodoDialogo("ta bueno pues.",
                                   {"Adiós"});
        auto chalecos = new NodoDialogo("¡Mis chalecos! Toma uno por ayudarme",
                                        {"Gracias"}, "DAR_CHALECO");

        arbolDialogos->hijos<<cSi<<cNo;


        if (Inventario::getInstance()->objetoExiste("chaleco")) {
            cSi->hijos << chalecos;
        }
        break;
    }

    /* ───────── NPC 6 ───────── */
    case Tipo::NPC6: {
        arbolDialogos=new NodoDialogo(
            "Consigueme un casco del gimnasio y te daré mis zapatos especiales!",
            {"Sí", "No"}
            );
        auto cSi=new NodoDialogo("Vé, antes que cambie de opinión",
                                   {"Entendido"});
        auto cNo=new NodoDialogo("Estaré aqui por si vuelves...",
                                   {"Adiós"});
        auto zapatos = new NodoDialogo("¡El casco! Los zapatos son tuyos",
                                       {"Gracias"}, "DAR_ZAPATOS");

        arbolDialogos->hijos<<cSi<<cNo;


        if (Inventario::getInstance()->objetoExiste("casco")) {
            cSi->hijos << zapatos;
        }
        break;


    }
    }
}

QStringList NPC::obtenerOpcionesDialogo()const
{

    return nodoActual?nodoActual->opciones:QStringList{};

}

void NPC::ejecutarConsecuencia(NodoDialogo *hoja)
{

    if(!inventarioRef||!hoja)return;

    if(recompensaEntregada)return;

    const QString& c=hoja->consecuencia;

    if(c=="DAR_CURAR1x2"){
        mostrarNotificacion("✔️ Ganaste un botiquin pequeño");
        inventarioRef->insertarObjeto("curar1", 2, "Botiquín", "Cura vida");
    }

    else if(c=="DAR_CURAR2x2"){
        mostrarNotificacion("✔️ Obtuviste un botiquin grande");
        inventarioRef->insertarObjeto("curar2", 2, "Botiquín", "Cura más vida");
    }
    else if(c=="QUITAR_LLAVE"){
        //mostrarNotificacion("✔️ ");
        inventarioRef->eliminarObjeto("llave");
    }
    else if(c=="DAR_GRANADAS"){
        mostrarNotificacion("✔️ Obtuviste 10 granadas");
        inventarioRef->insertarObjeto("granada",10,"arma","explota");
    }
    else if(c=="QUITAR_MUNICIONES"){
        //mostrarNotificacion("✔️ " + c);
        inventarioRef->setBalas(inventarioRef->getBalas()-20);
        inventarioRef->insertarObjeto("curar1", 2, "Botiquín", "Cura vida");
    }

    else if(c=="DAR_ZAPATOS"){
        mostrarNotificacion("✔️ Obtuviste los zapatos rapidos! ");
        inventarioRef->eliminarObjeto("casco");
        inventarioRef->insertarObjeto("zapato_rapido",1,"armadura","incrementa velocidad");
    }

    else if(c=="DAR_CHALECO"){
        mostrarNotificacion("✔️ Tienes un chaleco!");
        inventarioRef->eliminarObjeto("chaleco");
        inventarioRef->insertarObjeto("chaleco",1,"armadura","protege");
    }
    else if(c=="DAR_MUNICIONES"){
        mostrarNotificacion("✔️ Obtuviste 30 municiones" );
        inventarioRef->insertarObjeto("municiones", 30, "arma", "disparar");
        inventarioRef->eliminarObjeto("llave");
    }

    recompensaEntregada=true;

}
