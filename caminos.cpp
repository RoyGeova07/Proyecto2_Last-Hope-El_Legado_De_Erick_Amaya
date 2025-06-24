#include "caminos.h"
#include<QPixmap>
#include<QLabel>
#include<QDebug>
#include<QTimer>
#include"lobby.h"
#include"Ciudad.h"
#include "gasolinera.h"
#include "mall.h"
#include "gimnasio.h"
#include<QPainter>
#include"supermercado.h"
#include"laboratorio.h"


Caminos::Caminos(personaje*jugadorExistente, QWidget* parent) : AtributosPersonaje(jugadorExistente,parent), rutaActual(1)
{
    this->resize(1078, 714);
    this->setWindowTitle("Caminos - Last hope");

    configurarEscena();
    setPermitirLanzarGranada(false);
    configurarObstaculos();

    jugador->setParent(this);
    jugador->show();
    jugador->raise();

    Movimientos();

    jugador->move(42,190);

    labelPresionarT = new QLabel(this);
    labelPresionarT->setText("¿Quieres entrar al nivel?\nA. Sí    B. Info");

    QPixmap letreroPixmap(":/imagenes/assets/mapas/LetreroEntrar.png");
    if (!letreroPixmap.isNull()) {
        QPixmap letreroEscalado = letreroPixmap.scaled(300, 80, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPainter painter(&letreroEscalado);
        painter.setPen(QPen(Qt::white));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(letreroEscalado.rect(), Qt::AlignCenter | Qt::TextWordWrap,
                         "¿Quieres entrar al nivel?\nA. Sí    B. Info");

        labelPresionarT->setPixmap(letreroEscalado);
        labelPresionarT->setText("");
        labelPresionarT->hide();
    }
    // dialogo NPC
    dialogoNPC = new DialogoNPC(this);
    dialogoNPC->hide();

    // Crear NPC en Ruta 1
    QList<NPC*> npcsRuta1;
    NPC* npcRuta1 = new NPC(NPC::Tipo::NPC1, this);
    npcRuta1->move(400, 300);  // Posición en Ruta 1
    npcsRuta1.append(npcRuta1);
    npcRuta1->setInventario(Inventario::getInstance());
    npcsPorRuta.insert(1, npcsRuta1);

    // Crear NPC en Ruta 2
    QList<NPC*> npcsRuta2;
    NPC* npcRuta2 = new NPC(NPC::Tipo::NPC4, this);
    npcRuta2->move(210, 45);  // Posición en Ruta 1
    npcRuta2->hide();
    npcsRuta2.append(npcRuta2);
    npcRuta2->setInventario(Inventario::getInstance());
    npcsPorRuta.insert(2, npcsRuta2);

    // Crear NPC en Ruta 4
    QList<NPC*> npcsRuta4;
    NPC* npcRuta4 = new NPC(NPC::Tipo::NPC3, this);
    npcRuta4->move(400, 300);
    npcRuta4->hide();  // Inicialmente oculto
    npcsRuta4.append(npcRuta4);
    npcRuta4->setInventario(Inventario::getInstance());
    npcsPorRuta.insert(4, npcsRuta4);

    // Crear NPC en Ruta 7
    QList<NPC*> npcsRuta7;
    NPC* npcRuta7 = new NPC(NPC::Tipo::NPC5, this);
    npcRuta7->move(400, 180);
    npcRuta7->hide();  // Inicialmente oculto
    npcsRuta7.append(npcRuta7);
    npcRuta7->setInventario(Inventario::getInstance());
    npcsPorRuta.insert(7, npcsRuta7);

    // Crear NPC en Ruta 8
    QList<NPC*> npcsRuta8;
    NPC* npcRuta8 = new NPC(NPC::Tipo::NPC6, this);
    npcRuta8->move(400, 300);
    npcRuta8->hide();  // Inicialmente oculto
    npcsRuta8.append(npcRuta8);
    npcRuta8->setInventario(Inventario::getInstance());
    npcsPorRuta.insert(8, npcsRuta8);

}
//NO BORRAR ESTO ===========================================================
//son las coordenadas exactas para cambiar ENTRE LAS RUTAASSSSSZZZZ
QRect zonaCambioRuta3(940, 40, 200, 300);
QRect zonaCambioRuta2(0, 40, 100, 300);
QRect zonaCambioRuta4(644,574,127,127);
QRect zonaCambioRuta5(982,486,127,127);
QRect zonaCambioRuta6(986,422,127,127);
QRect zonaCambioRuta1_DesdeRuta4(-12,306,127,127);
QRect zonaCambioRuta5_a_Ruta4(-24,398,127,127);
QRect zonaCambioRuta6_a_Ruta2(-32,356,127,127);
QRect zonaCambioRuta3_a_Desde_Ruta6(446,-72,127,127);
QRect zonaCambioRuta6_desde_Ruta3(451,576,127,127);
QRect zonaCambioRuta5_a_Desde_Ruta6(456,580,127,127);
QRect zonaCambioRuta6_a_Desde_Ruta5(454,-66,127,127);
// =========================================================================
QRect zonaEntradaCiudad(236,422,131,127);
QRect zonaEntradaGasolinera(450,350,131,127);
QRect zonaEntradaMall(450,400,131,40);
QRect zonaEntradaGym(450,500,131,40);
QRect zonaEntradaSuper(700,350,131,50);
QRect zonaEntradaLab(600,400,131,40);

void Caminos::configurarEscena()
{
    fondoLabel = new QLabel(this);
    fondoLabel->setGeometry(0, 0, width(), height());
    cambiarRuta(1);
}


//AGREGAR OBSTACULOS DESPUESSSS
void Caminos::configurarObstaculos()
{
    obstaculos.clear();

    if (rutaActual == 1)
    {
        obstaculos.clear();
        obstaculos.append(QRect(18, 20, 270, 100)); //PARTE SUPERIOR IZQUIERDA
        obstaculos.append(QRect(288, 20, 700, 20)); //PARTE SUPERIOR DERECHA
        obstaculos.append(QRect(18, 335, 270, 400)); //PARTE INFERIOR IZQUIERDA
        obstaculos.append(QRect(288, 365, 60, 300)); //PARTE INFERIOR IZQUIERDA 2
        obstaculos.append(QRect(550, 220, 270, 40)); //PARTE INFERIOR DERECHA
        obstaculos.append(QRect(580, 260, 270, 40)); //PARTE INFERIOR DERECHA 2
        obstaculos.append(QRect(610, 300, 270, 60)); //PARTE INFERIOR DERECHA 3
        obstaculos.append(QRect(680, 360, 270, 80)); //PARTE INFERIOR DERECHA 3
        obstaculos.append(QRect(720, 440, 270, 80)); //PARTE INFERIOR DERECHA 4


    }else if (rutaActual == 2){
       obstaculos.clear();
       obstaculos.append(QRect(18, 20, 205, 330)); //PARTE SUPERIOR IZQUIERDA
       obstaculos.append(QRect(223, 20, 800, 20)); //PARTE SUPERIOR DERECHA
       obstaculos.append(QRect(430, 220, 400, 170)); //PARTE MEDIA
       obstaculos.append(QRect(18, 600, 1000, 30)); //PARTE INFERIOR

    }else if(rutaActual==3){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 3
        obstaculos.append(QRect(20,20,1000,190)); //PARTE SUPERIOR
        obstaculos.append(QRect(20,410,400,190)); //PARTE INFERIOR IZQUIERDA
        obstaculos.append(QRect(620,410,400,190)); //PARTE INFERIOR DERECHA

    }else if(rutaActual==4){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 4
        obstaculos.append(QRect(20,20,150,190)); //PARTE SUPERIOR
        obstaculos.append(QRect(170,20,150,300));
        obstaculos.append(QRect(320,20,150,360));
        obstaculos.append(QRect(470,20,150,370));
        obstaculos.append(QRect(620,20,400,390));
        obstaculos.append(QRect(20,600,200,200)); //PARTE INFERIOR
        obstaculos.append(QRect(220,630,800,200));

    }else if(rutaActual==5){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 5
        obstaculos.append(QRect(20,20,400,310)); //PARTE SUPERIOR IZQUIERDA
        obstaculos.append(QRect(600,20,400,310)); //PARTE SUPERIOR DERECHA
        obstaculos.append(QRect(20,555,400,300)); //PARTE INFERIOR IZQUIERDA
        obstaculos.append(QRect(600,555,400,300)); //PARTE INFERIOR DERECHA
        obstaculos.append(QRect(456,542,583,669));

    }else if(rutaActual==6){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 6
        obstaculos.append(QRect(20,20,400,300)); //PARTE SUPERIOR IZQUIERDA
        obstaculos.append(QRect(600,20,400,300)); //PARTE SUPERIOR DERECHA
        obstaculos.append(QRect(20,520,400,300)); //PARTE INFERIOR IZQUIERDA
        obstaculos.append(QRect(600,500,400,300)); //PARTE INFERIOR DERECHA

    }else if(rutaActual==7){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 7
        obstaculos.append(QRect(20,20,1200,100)); //PARTE SUPERIOR
        obstaculos.append(QRect(20,300,700,600)); //PARTE  DERECHA
        obstaculos.append(QRect(940,20,400,600)); //PARTE  IZQUIERDA

    }else if(rutaActual==8){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 8
        obstaculos.append(QRect(20,20,720,330)); //PARTE SUPERIOR
        obstaculos.append(QRect(1000,20,300,700)); //PARTE  DERECHA
        obstaculos.append(QRect(20,580,1100,600)); //PARTE  IZQUIERDA

    }else if(rutaActual==9){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 9
        obstaculos.append(QRect(20,20,610,280)); //PARTE SUPERIOR IZQUIERDA
        obstaculos.append(QRect(880,20,500,280)); //PARTE SUPERIOR DERECHA
        obstaculos.append(QRect(20,490,610,700)); //PARTE INFERIOR IZQUIERDA
        obstaculos.append(QRect(880,490,600,600)); //PARTE INFERIOR DERECHA

    }
    else if(rutaActual==10){

        obstaculos.clear();//AQUI AGREGAR LOS OBSTACULOS DE LA RUTA 10
        obstaculos.append(QRect(20,20,1100,300)); //PARTE SUPERIOR
        obstaculos.append(QRect(20,480,1100,700)); //PARTE INFERIOR
        obstaculos.append(QRect(700,20,1100,800)); //PARTE  DERECHA

    }
}
void Caminos::mostrarInfoDelNivel(int nivel) {
    QString rutaImagen;
    switch (nivel) {
    case 2:
        rutaImagen = ":/imagenes/assets/Info/ciudad_info.png";
        break;
    case 3:
        rutaImagen = ":/imagenes/assets/Info/gasolinera_info.png";
        break;
    case 5:
        rutaImagen = ":/imagenes/assets/Info/gimnasio_info.png";
        break;
    case 6:
        rutaImagen = ":/imagenes/assets/Info/mall_info.png";
        break;
    case 9:
        rutaImagen = ":/imagenes/assets/Info/super_info.png";
        break;
    case 10:
        rutaImagen = ":/imagenes/assets/Info/lab_info.png";
        break;
    default:
        rutaImagen = ":/imagenes/assets/Info/info_default.png";
        break;
    }

    QDialog* dlg = new QDialog(this);
    dlg->setWindowTitle("Información del nivel");
    dlg->setFixedSize(560, 350); // Ajusta el tamaño a tu gusto
    dlg->setStyleSheet("background: #222; border-radius: 14px;");

    QLabel* imgLabel = new QLabel(dlg);
    QPixmap infoImg(rutaImagen);
    if (infoImg.isNull()) {
        imgLabel->setText("No se pudo cargar la imagen.");
        imgLabel->setStyleSheet("color: white;");
    } else {
        imgLabel->setPixmap(infoImg.scaled(dlg->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLabel->setAlignment(Qt::AlignCenter);
    }
    imgLabel->setGeometry(0, 0, dlg->width(), dlg->height());

    // Opcional: cerrar al hacer clic en cualquier parte del dialog
    dlg->installEventFilter(this);
    infoLabelInfoActual = imgLabel; // Para eventFilter si quieres usarlo para cerrar con clic

    dlg->exec();
    dlg->deleteLater();
}



bool Caminos::eventFilter(QObject* obj, QEvent* event) {
    if (infoLabelInfoActual && obj == infoLabelInfoActual) {
        if (event->type() == QEvent::MouseButtonPress) {
            infoLabelInfoActual->close();
            infoLabelInfoActual = nullptr;
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Caminos::posicionarJugadorEnCalleRuta1()
{

    // Posicion en la calle superior de RUTA 1
    //jugador->move(width()-jugador->width()-50,180);
    int posX=width()-jugador->width()-50;
    int posY=115;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorEnCalleRuta2()
{

    // Posicion en la calle inferior de RUTA 2
    jugador->move(100,height()-jugador->height()-120);

}

void Caminos::posicionarJugadorEnCalleRuta3()
{

    int posX=200;
    int posY=260;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorEnCalleRuta4()
{

    int posX=132;
    int posY=354;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorEnCalleRuta5()
{

    int posX=144;
    int posY=398;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadroEnCalleRuta6()
{

    int posX=158;
    int posY=358;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorEnCalleRuta9()
{

    int posX=158;
    int posY=358;

    jugador->move(posX,posY);

}


void Caminos::posicionarJugadorDeVueltaARuta2_Desde_Ruta3()
{

    int posX=808;
    int posY=40;

    jugador->move(posX,posY);

}
void Caminos::posicionarJugadorDeVueltaA_Ruta1_Desde_Ruta4()
{

    int posX=552;
    int posY=438;


    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorDeVueltaA_Ruta4_Desde_Ruta5()
{

    int posX=818;
    int posY=480;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorDeVueltaA_Ruta2_Desde_Ruta6()
{

    int posX=826;
    int posY=434;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta3()
{

    int posX=458;
    int posY=96;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorDeVueltaA_Ruta3_Desde_Ruta6()
{

    int posX=447;
    int posY=446;


    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorDeVueltaA_Ruta5_Desde_Ruta6()
{

    int posX=452;
    int posY=76;

    jugador->move(posX,posY);

}

void Caminos::posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta5()
{

    int posX=458;
    int posY=442;

    jugador->move(posX,posY);

}

void Caminos::cambiarRuta(int nuevaRuta)
{
    rutaActual = nuevaRuta;

    // Ocultar todos los NPCs primero
    for (auto it = npcsPorRuta.begin(); it != npcsPorRuta.end(); ++it) {
        for (NPC* npc : it.value()) {
            npc->hide();
            npc->ocultarHintInteractuar();
        }
    }

    // Mostrar solo los NPCs de la ruta actual
    if (npcsPorRuta.contains(rutaActual)) {
        for (NPC* npc : npcsPorRuta[rutaActual]) {
            npc->show();
            npc->raise();
        }
    }

    QString rutaImagen;

    if(rutaActual==1)
    {
        rutaImagen=":/imagenes/assets/mapas/RUTA_1.jpg";

    }else if(rutaActual==2){

        rutaImagen=":/imagenes/assets/mapas/RUTA_2.jpg";

    }else if(rutaActual==3){

        rutaImagen=":/imagenes/assets/mapas/RUTA_3.jpg";

    }else if(rutaActual==4){

        rutaImagen=":/imagenes/assets/mapas/RUTA_4.jpg";

    }else if(rutaActual==5){

        rutaImagen=":/imagenes/assets/mapas/RUTA_5.jpg";

    }else if(rutaActual==6){

        rutaImagen=":/imagenes/assets/mapas/RUTA_6.jpg";

    }else if(rutaActual==7){

        rutaImagen=":/imagenes/assets/mapas/RUTA_7.png";
    }else if(rutaActual==8){

        rutaImagen=":/imagenes/assets/mapas/RUTA_8.png";

    }else if(rutaActual==9){

        rutaImagen=":/imagenes/assets/mapas/RUTA_9.png";

    }else if(rutaActual==10){

        rutaImagen=":/imagenes/assets/mapas/RUTA_10.png";

    }


    configurarObstaculos();

    QPixmap fondoPixmap(rutaImagen);
    if(fondoPixmap.isNull())
    {

        qDebug() << "Error al cargar la imagen de la ruta:" << rutaImagen;

    }else{

        fondoLabel->setPixmap(fondoPixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));

    }

    if(jugador)
    {

        jugador->DetenerAnimacion();
        auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
        jugador->SetAnimacion(anim.ruta,anim.frames);

    }

}

void Caminos::onMovimientoUpdate()
{

    ActualizarBarraEscudo();
    if (!jugador) return;
    QRect rectJugador = jugador->geometry();

    // Verificar NPCs solo de la ruta actual
    npcCercano = nullptr;
    if (npcsPorRuta.contains(rutaActual)) {
        bool hayNpcCerca = false;

        for(NPC* npc : npcsPorRuta[rutaActual]) {
            QRect rectNPC = npc->geometry();
            QRect zonaProximidad = rectNPC.adjusted(-20, -20, 20, 20);

            if(rectJugador.intersects(zonaProximidad)) {
                npc->mostrarHintInteractuar();
                npcCercano = npc;
                hayNpcCerca = true;
            } else {
                npc->ocultarHintInteractuar();
            }
        }
    }

    // aqui Cambiar a RUTA_2 cuando el jugador llegue al final de la carretera de arriba (borde derecho)
    if (rutaActual == 1 && rectJugador.right() >= width() - 50 && rectJugador.top() <= 200)
    {
        obstaculos.clear();
        cambiarRuta(2);
        configurarObstaculos();

        posicionarJugadorEnCalleRuta2();
        return;
    }

    //aqui vuelve a RUTA_1 si el jugador regresa por la izquierda en RUTA_2 (carretera inferior)
    if (rutaActual == 2 && rectJugador.left() <= 70 && rectJugador.bottom() >= height() - 200)
    {
        obstaculos.clear();
        cambiarRuta(1);
        configurarObstaculos();

        posicionarJugadorEnCalleRuta1();

        return;
    }

    if (rutaActual == 2 && zonaCambioRuta3.intersects(rectJugador))
    {
        obstaculos.clear();
        cambiarRuta(3);
        configurarObstaculos();

        posicionarJugadorEnCalleRuta3();

        qDebug() << "Cambiando a RUTA_3...";

        return;
    }

    if (rutaActual == 3 && zonaCambioRuta2.intersects(rectJugador))
    {
        obstaculos.clear();
        cambiarRuta(2);
        configurarObstaculos();

        posicionarJugadorDeVueltaARuta2_Desde_Ruta3();

        qDebug() << "Regresando a RUTA_2...";

        return;
    }



    //CON ESTO REGRESA AL LOBBY (zona de la puerta en RUTA_1)
    if (rutaActual == 1 && !regresandoAlLobby)
    {
        QRect zonaPuertaLobby(0, 120, 30, 100);

        if (rectJugador.intersects(zonaPuertaLobby))
        {
            regresandoAlLobby = true; //aqui se ACTIVA la bandera pa no creear un pjaso de widgets repetidos

            qDebug() << "Regresando al lobby...";

            // Parar el timer
            if (movimientoTimer && movimientoTimer->isActive())
                movimientoTimer->stop();

            // Resetear las teclas
            shiftPresionado=false;
            izquierdaPresionada=false;
            derechaPresionada=false;
            arribaPresionado=false;
            abajoPresionado=false;
            ZPresionado=false;

            // Dejar el personaje en Idle
            jugador->DetenerAnimacion();
            auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
            jugador->SetAnimacion(anim.ruta,anim.frames);


            lobby* lobbyWindow=new lobby(jugador);
            lobbyWindow->posicionarJugadorEnPuerta();
            lobbyWindow->show();
            this->close();

            return;
        }
    }

    //AQUI CAMBIA DESDE LA RUTA_1 HACIA LA RUTA_4
    if (rutaActual == 1 && zonaCambioRuta4.intersects(rectJugador))
    {
        obstaculos.clear();

        cambiarRuta(4); // Cuando implementes RUTA_4
        configurarObstaculos();

        posicionarJugadorEnCalleRuta4();
        qDebug() << "Cambiando a RUTA_4...";

        return;
    }

    if (rutaActual == 3)
    {
        qDebug() << "Jugador en RUTA_3: LEFT =" << rectJugador.left()
        << " TOP =" << rectJugador.top()
        << " RIGHT =" << rectJugador.right()
        << " BOTTOM =" << rectJugador.bottom();

        if(zonaEntradaGasolinera.intersects(rectJugador))
        {
            if(!labelPresionarT->isVisible())
            {
                labelPresionarT->move(450, 350);
                labelPresionarT->show();
                labelPresionarT->raise();
            }
        }else{

            //si sale de la zona ocultar el label
            labelPresionarT->hide();

        }

        // Cambio a RUTA_6 desde RUTA_3
        if (zonaCambioRuta6_desde_Ruta3.intersects(rectJugador))
        {
            obstaculos.clear();

            cambiarRuta(6); // RUTA_6
            configurarObstaculos();

            posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta3();

            qDebug() << "Cambiando a RUTA_6 desde RUTA_3...";

            return;
        }

    }

    if (rutaActual == 5)
    {
        qDebug() << "Jugador en RUTA_5: LEFT =" << rectJugador.left()
        << " TOP =" << rectJugador.top()
        << " RIGHT =" << rectJugador.right()
        << " BOTTOM =" << rectJugador.bottom();

        if(zonaEntradaGym.intersects(rectJugador))
        {
            if(!labelPresionarT->isVisible())
            {
                labelPresionarT->move(450, 422);
                labelPresionarT->show();
                labelPresionarT->raise();
            }
        }else{

            //si sale de la zona ocultar el label
            labelPresionarT->hide();

        }

        //Cambio a RUTA_6 desde RUTA_5
        if(zonaCambioRuta6_a_Desde_Ruta5.intersects(rectJugador))
        {
            obstaculos.clear();

            cambiarRuta(6); // RUTA_6
            configurarObstaculos();

            posicionarJugadorDeVueltaA_Ruta6_Desde_Ruta5();

            qDebug() << "Regresando a RUTA_6 desde RUTA_5...";

            return;
        }

    }

    if(rutaActual==6)
    {
        qDebug() << "Jugador en RUTA_6: LEFT =" << rectJugador.left()
        <<" TOP ="<<rectJugador.top()
        <<" RIGHT ="<<rectJugador.right()
        <<" BOTTOM ="<<rectJugador.bottom();

        if(zonaEntradaMall.intersects(rectJugador))
        {
            if(!labelPresionarT->isVisible())
            {
                labelPresionarT->move(450, 422);
                labelPresionarT->show();
                labelPresionarT->raise();
            }
        }else{

            //si sale de la zona ocultar el label
            labelPresionarT->hide();

        }

        //  Cambio a RUTA_3 desde RUTA_6
        if(zonaCambioRuta3_a_Desde_Ruta6.intersects(rectJugador))
        {
            obstaculos.clear();

            cambiarRuta(3); // RUTA_3
            configurarObstaculos();

            posicionarJugadorDeVueltaA_Ruta3_Desde_Ruta6();

            qDebug()<<"Regresando a RUTA_3 desde RUTA_6...";

            return;
        }

        //  Cambio a RUTA_5 desde RUTA_6
        if(zonaCambioRuta5_a_Desde_Ruta6.intersects(rectJugador))
        {
            obstaculos.clear();

            cambiarRuta(5); // RUTA_5
            configurarObstaculos();

            posicionarJugadorDeVueltaA_Ruta5_Desde_Ruta6();

            qDebug()<<"Regresando a RUTA_5 desde RUTA_6...";

            return;
        }


    }

    if (rutaActual==9){
        if(zonaEntradaSuper.intersects(rectJugador))
        {
            if(!labelPresionarT->isVisible())
            {
                labelPresionarT->move(450, 360);
                labelPresionarT->show();
                labelPresionarT->raise();
            }
        }else{

            //si sale de la zona ocultar el label
            labelPresionarT->hide();

        }
    }

    if (rutaActual==10){
        if(zonaEntradaLab.intersects(rectJugador))
        {
            if(!labelPresionarT->isVisible())
            {
                labelPresionarT->move(450, 422);
                labelPresionarT->show();
                labelPresionarT->raise();
            }
        }else{

            //si sale de la zona ocultar el label
            labelPresionarT->hide();

        }
    }


    // Volver a RUTA_2 desde RUTA_6
    if(rutaActual==6&&zonaCambioRuta6_a_Ruta2.intersects(rectJugador))
    {
        obstaculos.clear();

        cambiarRuta(2); // Volvemos a RUTA_2
        configurarObstaculos();

        posicionarJugadorDeVueltaA_Ruta2_Desde_Ruta6();

        qDebug()<<"Regresando a RUTA_2 desde RUTA_6...";

        return;
    }

    // VOLVER A LA RUTA_4 DESDE LA RUTA_5
    if(rutaActual==5&&zonaCambioRuta5_a_Ruta4.intersects(rectJugador))
    {
        obstaculos.clear();

        cambiarRuta(4);
        configurarObstaculos();

        posicionarJugadorDeVueltaA_Ruta4_Desde_Ruta5();

        qDebug()<<"Regresando a RUTA_4 desde RUTA_5...";

        return;
    }

    //VOLVER A LA RUTA_1 DESDE LA RUTA_4
    if(rutaActual==4&&zonaCambioRuta1_DesdeRuta4.intersects(rectJugador))
    {
        obstaculos.clear();

        cambiarRuta(1);
        configurarObstaculos();

        posicionarJugadorDeVueltaA_Ruta1_Desde_Ruta4();

        qDebug()<<"Regresando a RUTA_1 desde RUTA_4...";

        return;
    }

    //AQUI CAMBIA A RUTA_5 desde RUTA_4
    if(rutaActual==4&&zonaCambioRuta5.intersects(rectJugador))
    {
        obstaculos.clear();

        cambiarRuta(5); // RUTA_5
        configurarObstaculos();

        posicionarJugadorEnCalleRuta5();

        qDebug()<<"Cambiando a RUTA_5...";

        return;
    }

    if (rutaActual == 2)
    {

        // Intento de cambio a RUTA_3 (ajusta esta zona cuando veas las coords)
        if (rectJugador.left() <= 30 && rectJugador.top() >= 50 && rectJugador.bottom() <= 150)
        {
            obstaculos.clear();
            cambiarRuta(3);
            configurarObstaculos();

            posicionarJugadorEnCalleRuta3();

            qDebug() << "Cambiando a RUTA_3...";

            return;
        }

        // Cambio a RUTA_6 desde RUTA_2
        if(zonaCambioRuta6.intersects(rectJugador))
        {
            obstaculos.clear();

            cambiarRuta(6); // RUTA_6
            configurarObstaculos();

            posicionarJugadroEnCalleRuta6();

            qDebug()<<"Cambiando a RUTA_6...";

            return;
        }

        //aqui se mustra menu entrada ciudad en la zona
        if(zonaEntradaCiudad.intersects(rectJugador))
        {
            if(!labelPresionarT->isVisible())
            {
                labelPresionarT->move(width()/2-labelPresionarT->width()/2,height()-200);
                labelPresionarT->show();
                labelPresionarT->raise();
            }
        }else{

            //si sale de la zona ocultar el label
            labelPresionarT->hide();

        }
    }

    // Cambio a RUTA_7 desde RUTA_3
    if(rutaActual == 3 && rectJugador.intersects(QRect(830, 250, 100, 500)))
    {
        obstaculos.clear();

        cambiarRuta(7);
        configurarObstaculos();

        jugador->move(230,150);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }

    // Cambio a RUTA_3 desde RUTA_7
    if(rutaActual == 7 && rectJugador.intersects(QRect(20, 250, 70, 300)))
    {
        obstaculos.clear();

        cambiarRuta(3);
        configurarObstaculos();

        jugador->move(700,250);

        qDebug()<<"Cambiando a RUTA_3...";

        return;
    }
    // Cambio a RUTA_9 desde RUTA_7
    if(rutaActual == 7 && rectJugador.intersects(QRect(800, 550, 200, 60)))
    {
        obstaculos.clear();

        cambiarRuta(9);
        configurarObstaculos();

        jugador->move(700,230);

        qDebug()<<"Cambiando a RUTA_3...";

        return;
    }

    // Cambio a RUTA_9 desde RUTA_6
    if(rutaActual == 6 && rectJugador.intersects(QRect(830, 400, 60, 200)))
    {
        obstaculos.clear();

        cambiarRuta(9);
        configurarObstaculos();

        jugador->move(200,300);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }

    // Cambio a RUTA_9 desde RUTA_8
    if(rutaActual == 8 && rectJugador.intersects(QRect(830, 60, 200, 60)))
    {
        obstaculos.clear();

        cambiarRuta(9);
        configurarObstaculos();

        jugador->move(640,350);

        qDebug()<<"Cambiando a RUTA_3...";

        return;
    }

    // Cambio a RUTA_5 desde RUTA_8
    if(rutaActual == 8 && rectJugador.intersects(QRect(50, 400, 60, 200)))
    {
        obstaculos.clear();

        cambiarRuta(5);
        configurarObstaculos();

        jugador->move(650,400);

        qDebug()<<"Cambiando a RUTA_3...";

        return;
    }

    // Cambio a RUTA_8 desde RUTA_5
    if(rutaActual == 5 && rectJugador.intersects(QRect(800, 400, 60, 200)))
    {
        obstaculos.clear();

        cambiarRuta(8);
        configurarObstaculos();

        jugador->move(200,400);

        qDebug()<<"Cambiando a RUTA_3...";

        return;
    }

    // Cambio a RUTA_7 desde RUTA_9
    if(rutaActual == 9 && rectJugador.intersects(QRect(730, 40, 200, 80)))
    {
        obstaculos.clear();

        cambiarRuta(7);
        configurarObstaculos();

        jugador->move(770,340);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }

    // Cambio a RUTA_8 desde RUTA_9
    if(rutaActual == 9 && rectJugador.intersects(QRect(800, 550, 200, 60)))
    {
        obstaculos.clear();

        cambiarRuta(8);
        configurarObstaculos();

        jugador->move(780,220);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }

    // Cambio a RUTA_6 desde RUTA_9
    if(rutaActual == 9 && rectJugador.intersects(QRect(80, 400, 60, 200)))
    {
        obstaculos.clear();

        cambiarRuta(6);
        configurarObstaculos();

        jugador->move(700,350);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }

    // Cambio a RUTA_10 desde RUTA_9
    if(rutaActual == 9 && rectJugador.intersects(QRect(920, 400, 60, 200)))
    {
        obstaculos.clear();

        cambiarRuta(10);
        configurarObstaculos();

        jugador->move(240,320);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }

    // Cambio a RUTA_9 desde RUTA_10
    if(rutaActual == 10 && rectJugador.intersects(QRect(50, 400, 60, 200)))
    {
        obstaculos.clear();

        cambiarRuta(9);
        configurarObstaculos();

        jugador->move(750,320);

        qDebug()<<"Cambiando a RUTA_7...";

        return;
    }


}

void Caminos::keyPressEvent(QKeyEvent *event)
{
    // Primero siempre llamar a la base
    AtributosPersonaje::keyPressEvent(event);

    // Interacción con NPC
    if(event->key() == Qt::Key_H && npcCercano && !npcCercano->estaHablando()) {
        npcCercano->mostrarDialogo(dialogoNPC);

        connect(npcCercano, &NPC::dialogoTerminado, this, [this]() {
            qDebug() << "Diálogo con NPC terminado";

            // Resetear flags de movimiento
            shiftPresionado = false;
            izquierdaPresionada = false;
            derechaPresionada = false;
            arribaPresionado = false;
            abajoPresionado = false;
            ZPresionado = false;

            // Detener animacion
            if(jugador) {
                jugador->DetenerAnimacion();
                auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
                jugador->SetAnimacion(anim.ruta,anim.frames);
            }

            // Reactivar movimiento
            if(movimientoTimer && !movimientoTimer->isActive())
                movimientoTimer->start();

            this->activateWindow();
            this->setFocus();
        });
    }

    // Entrar a la Ciudad (Ruta 2)
    if (rutaActual == 2 && event->key() == Qt::Key_A && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando a la Ciudad...";
        Ciudad* ciudadWindow = new Ciudad(jugador);
        ResetearMovimiento();
        ciudadWindow->show();
        this->close();
        return;
    }

    // Entrar a la Gasolinera (Ruta 3)
    if (rutaActual == 3 && event->key() == Qt::Key_A && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando a la Gasolinera...";
        Gasolinera* w = new Gasolinera(jugador);
        ResetearMovimiento();
        w->show();
        this->close();
        return;
    }

    // Entrar al Gimnasio (Ruta 5)
    if (rutaActual == 5 && event->key() == Qt::Key_A && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando al gimnasio...";
        Gimnasio* w = new Gimnasio(jugador);
        ResetearMovimiento();
        w->show();
        this->close();
        return;
    }

    // Entrar al Mall (Ruta 6)
    if (rutaActual == 6 && event->key() == Qt::Key_A && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando al Mall...";
        Mall* mallWindow = new Mall(jugador);
        ResetearMovimiento();
        mallWindow->show();
        this->close();
        return;
    }

    // Entrar al Supermercado (Ruta 9)
    if (rutaActual == 9 && event->key() == Qt::Key_A && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando al Supermercado...";
        supermercado* mallWindow = new supermercado(jugador);
        ResetearMovimiento();
        mallWindow->show();
        this->close();
        return;
    }

    // Entrar al Laboratorio (Ruta 10)
    if (rutaActual == 10 && event->key() == Qt::Key_A && labelPresionarT->isVisible())
    {
        qDebug() << "Entrando al Laboratorio...";
        laboratorio* mallWindow = new laboratorio(jugador);
        ResetearMovimiento();
        mallWindow->show();
        this->close();
        return;
    }

    // ----------- AGREGADO: Mostrar Info del Nivel con la tecla B -----------
    if (labelPresionarT->isVisible() && event->key() == Qt::Key_B)
    {
        mostrarInfoDelNivel(rutaActual);
        return;
    }
    // -----------------------------------------------------------------------

    if(!jugador)
    {
        jugador->DetenerAnimacion();
        auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
        jugador->SetAnimacion(anim.ruta,anim.frames);
    }
}

