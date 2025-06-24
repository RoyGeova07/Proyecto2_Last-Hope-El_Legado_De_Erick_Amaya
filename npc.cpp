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

        dialogoUI->mostrarDialogo(".......",obtenerImagenNPC(),QStringList{});//qstringlist me devuelve una lista vacia

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
        TablaHash::EstadoNPC estado = TablaHash::getInstance().getEstadoNPC("NPC1");

        if (estado == TablaHash::EstadoNPC::AceptoAyuda) {
            // Si YA ACEPTÓ ayudar antes
            if (Inventario::getInstance()->objetoExiste("llave")) {
                // Si tiene la llave: intercambio
                arbolDialogos = new NodoDialogo(
                    "¡Encontraste mi llave! ¿Quieres intercambiarla por municiones?",
                    {"Sí", "No"}
                    );
                auto n1_si = new NodoDialogo("¡Aquí tienes!", {"Gracias"}, "DAR_MUNICIONES");
                auto n1_no = new NodoDialogo("Bueno...", {"Adiós"});
                arbolDialogos->hijos << n1_si << n1_no;
            }
        }
        else if (estado == TablaHash::EstadoNPC::RechazoAyuda) {
            // Si RECHAZÓ antes: le vuelve a preguntar
            arbolDialogos = new NodoDialogo(
                "¿Seguro que no me ayudas a buscar mis llaves?",
                {"Sí, ahora sí", "No puedo"}
                );
            auto n1_si = new NodoDialogo("¡Gracias! Ten cuidado con los zombies...", {"Entendido"}, "MARCAR_ACEPTO_NPC1");
            auto n1_no = new NodoDialogo("Oh...", {"Adiós"});
            arbolDialogos->hijos << n1_si << n1_no;
        }
        else {
            // Primera interacción
            arbolDialogos = new NodoDialogo(
                "Perdí mis llaves en la Ciudad en Ruinas. ¿Me ayudas a buscarlas?",
                {"Sí, te ayudaré", "No puedo"}
                );
            auto n1_si = new NodoDialogo("¡Gracias! Ten cuidado...", {"Entendido"}, "MARCAR_ACEPTO_NPC1");
            auto n1_no = new NodoDialogo("Oh...", {"Adiós"}, "MARCAR_RECHAZO_NPC1");
            arbolDialogos->hijos << n1_si << n1_no;
        }
        break;
    }

    /* ───────── NPC 2 ───────── */
    case Tipo::NPC2: {
        TablaHash::EstadoNPC estado = TablaHash::getInstance().getEstadoNPC("NPC2");

        if (estado == TablaHash::EstadoNPC::AceptoAyuda) {
            // Si YA ACEPTÓ antes: mensaje de agradecimiento
            arbolDialogos = new NodoDialogo(
                "¡Usa bien esos botiquines! Cuídate ahí fuera.",
                {"Gracias"}
                );
        }
        else if (estado == TablaHash::EstadoNPC::RechazoAyuda) {
            // Si RECHAZÓ antes: le ofrece de nuevo
            arbolDialogos = new NodoDialogo(
                "¿Seguro que no quieres los botiquines? Te serán útiles.",
                {"Vale, los acepto", "No, gracias"}
                );
            auto n2_si = new NodoDialogo("Aquí tienes. ¡Úsalos bien!", {"Gracias"}, "MARCAR_ACEPTO_NPC2");
            auto n2_no = new NodoDialogo("Como quieras...", {"Adiós"});
            arbolDialogos->hijos << n2_si << n2_no;
        }
        else {
            // Primera interacción
            arbolDialogos = new NodoDialogo(
                "¿Quieres 2 botiquines para tu batalla?",
                {"Sí", "No"}
                );
            auto n2_si = new NodoDialogo("Aquí tienes. ¡Úsalos bien!", {"Gracias"}, "MARCAR_ACEPTO_NPC2");
            auto n2_no = new NodoDialogo("Como quieras...", {"Adiós"}, "MARCAR_RECHAZO_NPC2");
            arbolDialogos->hijos << n2_si << n2_no;
        }
        break;
    }

    /* ───────── NPC 3 ───────── */
    case Tipo::NPC3: {
        TablaHash::EstadoNPC estado = TablaHash::getInstance().getEstadoNPC("NPC3");

        if (estado == TablaHash::EstadoNPC::AceptoAyuda) {
            // Si YA ACEPTÓ antes: Diálogo cortés SIN ofrecer recompensa
            arbolDialogos = new NodoDialogo(
                "¡Gracias por tu ayuda antes! Ten cuidado con los zombies.",
                {"Adiós"}
                );
        }
        else if (estado == TablaHash::EstadoNPC::RechazoAyuda) {
            // Si RECHAZÓ antes: Le vuelve a preguntar
            arbolDialogos = new NodoDialogo(
                "¿Seguro que no quieres esos botiquines potentes?",
                {"Sí, ahora sí", "No, gracias"}
                );

            auto g1 = new NodoDialogo("Toma, te servirán mucho.", {"Gracias"}, "MARCAR_ACEPTO_NPC3");
            auto g2 = new NodoDialogo("Bueno...", {"Adiós"});
            arbolDialogos->hijos << g1 << g2;
        }
        else {
            // Primera vez que habla con él
            arbolDialogos = new NodoDialogo(
                "¿Quieres botiquines potentes (x2)?",
                {"Sí", "No"}
                );

            auto g1 = new NodoDialogo("Toma, te servirán mucho.", {"Gracias"}, "MARCAR_ACEPTO_NPC3");
            auto g2 = new NodoDialogo("De acuerdo, ¡cuídate!", {"Adiós"}, "MARCAR_RECHAZO_NPC3");
            arbolDialogos->hijos << g1 << g2;
        }
        break;
    }

    /* ───────── NPC 4 ───────── */
    case Tipo::NPC4: {
        // 1. Primero verifica si el inventario existe y tiene menos de 20 balas
        if (Inventario::getInstance()->getBalas() < 20) {
            arbolDialogos = new NodoDialogo(
                "¡No tienes suficientes balas para mi trueque! (Necesitas 20+)",
                {"Entendido"}
                );
            break; // ¡Importante! Sal del switch-case aquí
        }

        // 2. Si tiene suficientes balas, continúa con la lógica normal
        TablaHash::EstadoNPC estado = TablaHash::getInstance().getEstadoNPC("NPC4");

        if (estado == TablaHash::EstadoNPC::AceptoAyuda) {
            arbolDialogos = new NodoDialogo(
                "¡Gracias de nuevo por las municiones! ¿Necesitas algo más?",
                {"No, gracias", "Adiós"}
                );
        }
        else if (estado == TablaHash::EstadoNPC::RechazoAyuda) {
            arbolDialogos = new NodoDialogo(
                "¿Seguro que no quieres intercambiar 20 balas por 2 curaciones?",
                {"Sí, acepto", "No, gracias"}
                );

            auto n4_si = new NodoDialogo(
                "¡Gracias! Toma tus curaciones.",
                {"Entendido"},
                "MARCAR_ACEPTO_NPC4" // Acciones combinadas
                );

            auto n4_no = new NodoDialogo("Bueno...", {"Adiós"});
            arbolDialogos->hijos << n4_si << n4_no;
        }
        else {
            // Primera interacción
            arbolDialogos = new NodoDialogo(
                "¿Me das 20 balas a cambio de 2 curaciones pequeñas?",
                {"Sí", "No"}
                );

            auto n4_si = new NodoDialogo(
                "¡Trato hecho! Aquí tienes.",
                {"Gracias"},
                "MARCAR_ACEPTO_NPC4"
                );

            auto n4_no = new NodoDialogo(
                "Quizá después...",
                {"Adiós"},
                "MARCAR_RECHAZO_NPC4"
                );

            arbolDialogos->hijos << n4_si << n4_no;
        }
        break;
    }
    /* ───────── NPC 5 ───────── */
    case Tipo::NPC5: {
        TablaHash::EstadoNPC estado = TablaHash::getInstance().getEstadoNPC("NPC5");

        if (estado == TablaHash::EstadoNPC::AceptoAyuda) {
            if (Inventario::getInstance()->objetoExiste("chaleco")) {
                // Si YA ACEPTÓ y TIENE el chaleco: Diálogo final
                arbolDialogos = new NodoDialogo(
                    "¡Gracias por mi chaleco! Toma 70 municiones.",
                    {"Gracias"},
                    "DAR_MUNICIONES2"
                    );
            } else {
                // Si YA ACEPTÓ pero NO tiene chaleco: Recordatorio
                arbolDialogos = new NodoDialogo(
                    "¿Ya encontraste mi chaleco? Necesitas granadas...",
                    {"Todavía no", "Adiós"}
                    );
            }
        }
        else if (estado == TablaHash::EstadoNPC::RechazoAyuda) {
            // Si RECHAZÓ antes: Le vuelve a preguntar
            arbolDialogos = new NodoDialogo(
                "¿Seguro que no me ayudas con el chaleco? Te daré granadas...",
                {"Vaya, te ayudo", "No, gracias"}
                );

            auto cSi = new NodoDialogo("¡Gracias! Toma estas granadas.", {"Entendido"}, "MARCAR_ACEPTO_NPC5");
            auto cNo = new NodoDialogo("Bueno...", {"Adiós"});
            arbolDialogos->hijos << cSi << cNo;
        }
        else {
            // Primera vez que habla con él
            arbolDialogos = new NodoDialogo(
                "¿Me ayudas a recuperar mi chaleco del supermercado?",
                {"Sí", "No"}
                );

            auto cSi = new NodoDialogo("¡Gracias! Toma estas granadas.", {"Entendido"}, "MARCAR_ACEPTO_NPC5");
            auto cNo = new NodoDialogo("Ta bueno pues.", {"Adiós"},"MARCAR_RECHAZO_NPC5");
            arbolDialogos->hijos << cSi << cNo;
        }
        break;
    }

    /* ───────── NPC 6 ───────── */
    case Tipo::NPC6: {
        if (Inventario::getInstance()->objetoExiste("casco")) {
            // Si el jugador TIENE el casco: ofrecer intercambio
            arbolDialogos = new NodoDialogo(
                "¡Hey! ¿Quieres cambiar ese casco por granadas?",
                {"Sí, intercambiar", "No, gracias"}
                );

            auto n6_si = new NodoDialogo(
                "¡Buen trato! Toma estas granadas.",
                {"Gracias"},
                "MARCAR_ACEPTO_NPC6"  // Acción combinada
                );

            auto n6_no = new NodoDialogo(
                "Vuelve más tarde si cambias de opinión.",
                {"Adiós"}
                );

            arbolDialogos->hijos << n6_si << n6_no;
        } else {
            // Si NO tiene el casco: mensaje simple
            arbolDialogos = new NodoDialogo(
                "Si consigues un casco, puedo cambiártelo por granadas.",
                {"Entendido"}
                );
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
        mostrarNotificacion("✔️ Obtuviste 5 granadas");
        inventarioRef->insertarObjeto("granada",5,"arma","explota");
    }
    else if(c=="QUITAR_MUNICIONES"){
        mostrarNotificacion("¡Ganaste dos botiquines a cambio de 20 municiones! ");
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
        mostrarNotificacion("✔️ Obtuviste 60 municiones" );
        inventarioRef->insertarObjeto("municiones", 60, "arma", "disparar");
        inventarioRef->eliminarObjeto("llave");
    }

    else if(c=="DAR_MUNICIONES2"){
        mostrarNotificacion("✔️ Obtuviste 70 municiones" );
        inventarioRef->insertarObjeto("municiones", 70, "arma", "disparar");
        inventarioRef->eliminarObjeto("chaleco");
    }

    //Verificar estados de loos dialogos
    else if (c=="MARCAR_ACEPTO_NPC1"){
        TablaHash::getInstance().setEstadoNPC("NPC1", TablaHash::EstadoNPC::AceptoAyuda);
    }
    else if (c=="MARCAR_RECHAZO_NPC1"){
        TablaHash::getInstance().setEstadoNPC("NPC1", TablaHash::EstadoNPC::RechazoAyuda);
    }
    else if (c=="MARCAR_ACEPTO_NPC2"){
        TablaHash::getInstance().setEstadoNPC("NPC2", TablaHash::EstadoNPC::AceptoAyuda);
        mostrarNotificacion("✔️ Ganaste un botiquin pequeño");
        inventarioRef->insertarObjeto("curar1", 2, "Botiquín", "Cura vida");
    }
    else if (c=="MARCAR_RECHAZO_NPC2"){
        TablaHash::getInstance().setEstadoNPC("NPC2", TablaHash::EstadoNPC::RechazoAyuda);
    }
    else if (c=="MARCAR_ACEPTO_NPC3"){
        TablaHash::getInstance().setEstadoNPC("NPC3", TablaHash::EstadoNPC::AceptoAyuda);
        mostrarNotificacion("✔️ Obtuviste un botiquin grande");
        inventarioRef->insertarObjeto("curar2", 2, "Botiquín", "Cura más vida");
    }
    else if (c=="MARCAR_RECHAZO_NPC3"){
        TablaHash::getInstance().setEstadoNPC("NPC3", TablaHash::EstadoNPC::RechazoAyuda);
    }
    else if (c=="MARCAR_ACEPTO_NPC4"){
        TablaHash::getInstance().setEstadoNPC("NPC4", TablaHash::EstadoNPC::AceptoAyuda);
        if (inventarioRef->getBalas()<20){
        inventarioRef->setBalas(inventarioRef->getBalas()-20);
        inventarioRef->insertarObjeto("curar1", 2, "Botiquín", "Cura vida");
        } else {
            return;
        }
    }

    else if (c=="MARCAR_RECHAZO_NPC4"){
        TablaHash::getInstance().setEstadoNPC("NPC4", TablaHash::EstadoNPC::RechazoAyuda);
    }
    else if (c=="MARCAR_ACEPTO_NPC5"){
        TablaHash::getInstance().setEstadoNPC("NPC5", TablaHash::EstadoNPC::AceptoAyuda);
        mostrarNotificacion("✔️ Obtuviste 5 granadas");
        inventarioRef->insertarObjeto("granada",5,"arma","explota");
    }
    else if (c=="MARCAR_RECHAZO_NPC5"){
        TablaHash::getInstance().setEstadoNPC("NPC5", TablaHash::EstadoNPC::RechazoAyuda);
    }
    else if (c=="MARCAR_ACEPTO_NPC6"){
        TablaHash::getInstance().setEstadoNPC("NPC6", TablaHash::EstadoNPC::AceptoAyuda);
        mostrarNotificacion("✔️ Obtuviste 5 granadas a cambio de un casco!");
        inventarioRef->insertarObjeto("granada",5,"arma","explota");
        inventarioRef->eliminarObjeto("casco");
        //DAR_GRANADAS;QUITAR_CASCO
    }
    else if (c=="MARCAR_RECHAZO_NPC6"){
        TablaHash::getInstance().setEstadoNPC("NPC6", TablaHash::EstadoNPC::RechazoAyuda);
    }

    recompensaEntregada=true;

}
