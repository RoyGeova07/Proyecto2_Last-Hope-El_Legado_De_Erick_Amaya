#include "AtributosPersonaje.h"
#include <QPixmap>
#include <QDebug>
#include"bala.h"
#include"zombie.h"
#include"Ciudad.h"
#include <QMessageBox>
#include<QPainter>
#include <QStackedLayout>
#include<QScrollArea>

AtributosPersonaje::AtributosPersonaje(personaje *jugadorExistente, QWidget *parent):QWidget(parent),

    jugador(jugadorExistente), movimientoTimer(nullptr),
    shiftPresionado(false), izquierdaPresionada(false),
    derechaPresionada(false), arribaPresionado(false),
    abajoPresionado(false), ZPresionado(false), tablaNiveles(TablaHash::getInstance())
{

    inventarioWidget=new InventarioWidget(Inventario::getInstance());
    tablaWidget = new TablaWidget(&TablaHash::getInstance(), &TablaHash::getInstance(), this);
   // tablaWidget = new TablaWidget(TablaHash::getInstance(), TablaHash::getInstance());
    inicializarTabWidget();
    //barra de vida contenedor
    barraVidaLabel = new QLabel(this);
    barraVidaLabel->setStyleSheet("background-color: #333333; border: 2px solid black; border-radius: 5px; padding: 0px;");
    barraVidaLabel->setGeometry(10, 90, 200, 30);
    barraVidaLabel->show();

    //Barra rellena (acortable
    barraVidaInterna = new QWidget(barraVidaLabel);
    barraVidaInterna->setStyleSheet("background-color: green; border-radius: 3px;");
    barraVidaInterna->setGeometry(0, 0, 200, 30);
    barraVidaInterna->show();

    //Texto encima
    barraVidaTexto = new QLabel(barraVidaLabel);
    barraVidaTexto->setAlignment(Qt::AlignCenter);
    barraVidaTexto->setStyleSheet("color: white; font-weight: bold; font-size: 14px; background: transparent;");
    barraVidaTexto->setGeometry(0, 0, 200, 30);
    barraVidaTexto->show();

    // Barra de escudo (contenedor)
    barraEscudoLabel = new QLabel(this);
    barraEscudoLabel->setStyleSheet("background-color: #333333; border: 2px solid black; border-radius: 5px; padding: 0px;");
    barraEscudoLabel->setGeometry(10, 125, 200, 20); // debajo de la barra de vida
    barraEscudoLabel->show();

    // Barra azul interna (relleno)
    barraEscudoInterna = new QWidget(barraEscudoLabel);
    barraEscudoInterna->setStyleSheet("background-color: blue; border-radius: 3px;");
    barraEscudoInterna->setGeometry(0, 0, 0, 20); // inicia en 0
    barraEscudoInterna->show();

    // Texto encima de escudo
    barraEscudoTexto = new QLabel(barraEscudoLabel);
    barraEscudoTexto->setAlignment(Qt::AlignCenter);
    barraEscudoTexto->setStyleSheet("color: white; font-weight: bold; font-size: 12px; background: transparent;");
    barraEscudoTexto->setGeometry(0, 0, 200, 20);
    barraEscudoTexto->show();

    //etiqueta para mostrar las municiones
    labelMuniciones=new QLabel(this);
    labelMuniciones->setGeometry(10,45,300,40);
    labelMuniciones->setStyleSheet(
        "color: #FFD700;"
        "font-size: 18px;"
        "font-weight: bold;"
        "text-shadow: 1px 1px black;"
        "background: rgba(0,0,0,100);"
        "padding: 5px;"
        );
    labelMuniciones->show();

    ActualizarBarraVida();
    barraVidaLabel->raise();

    ActualizarBarraEscudo();
    barraEscudoLabel->raise();


}

AtributosPersonaje::~AtributosPersonaje()
{

    delete movimientoTimer;
    delete inventarioWidget;

}

void AtributosPersonaje::inicializarJugador() {
    if(!jugador)
    {

        jugador = new personaje(this);//Solo si NO hay jugador
        auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
        jugador->SetAnimacion(anim.ruta,anim.frames);

    }else{

        jugador->setParent(this);//si ya se actualizo solo se actualiza el parent

    }
    jugador->show();
    jugador->raise();

}

void AtributosPersonaje::Movimientos() {
    movimientoTimer=new QTimer(this);
    connect(movimientoTimer, &QTimer::timeout, this, [=]() {

        if(jugador->estaMuertoPersonaje())
        {

            movimientoTimer->stop();
            return;

        }

        bool moviendo=false;

        //Disparo continuo
        if(ZPresionado)
            intentarDisparar();

        //Movimiento (solo si no esta disparando)
        if(!disparandoAhora)
        {

            if(izquierdaPresionada)
            {

                jugador->MoverSiNoColisiona(-jugador->getVelocidadMovimiento(),0,obstaculos);
                moviendo=true;

            }else if(derechaPresionada){

                jugador->MoverSiNoColisiona(jugador->getVelocidadMovimiento(),0,obstaculos);
                moviendo=true;
            }

            if(arribaPresionado)
            {
                jugador->MoverSiNoColisiona(0,-jugador->getVelocidadMovimiento(),obstaculos);
                moviendo=true;

            }else if(abajoPresionado){

                jugador->MoverSiNoColisiona(0,jugador->getVelocidadMovimiento(),obstaculos);
                moviendo=true;

            }
        }

        //Animaciones segun estado
        if(moviendo)
        {

            if(shiftPresionado)
            {

                jugador->SetAnimacionMovimiento(6);
                auto anim=jugador->obtenerAnimacion("run",jugador->personajeActual);
                jugador->SetAnimacion(anim.ruta,anim.frames);

            }else{

                jugador->SetAnimacionMovimiento(2);
                auto anim=jugador->obtenerAnimacion("walk",jugador->personajeActual);
                jugador->SetAnimacion(anim.ruta,anim.frames);

            }
        }else{

            movimientoTimer->stop();
            if(!disparandoAhora)
            {

                jugador->DetenerAnimacion();
                auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
                jugador->SetAnimacion(anim.ruta,anim.frames);

            }
        }

        ActualizarBarraVida();
        onMovimientoUpdate();
    });

    movimientoTimer->setInterval(30);
}

void AtributosPersonaje::keyPressEvent(QKeyEvent* event)
{
    if(!jugador)return;
    if(jugador->estaMuertoPersonaje())return;
    if(estadoCurandose)return;//si esta curandose no mover

    //aqui tecla l para abrir y cerrar el inventario
    if (event->key() == Qt::Key_I) {
        if (tabWidget) {
            if (tabWidget->isVisible()) {
                tabWidget->hide();
            } else {
                // Centrado preciso considerando la geometría de la ventana
                QPoint center = mapToGlobal(rect().center());
                tabWidget->move(center.x() - tabWidget->width()/2,
                                center.y() - tabWidget->height()/2);

                inventarioWidget->actualizarVista();
                tabWidget->show();
                tabWidget->raise();
            }
        }
    }

    //Si estamos disparando y la tecla NO es Z, cancelar disparo
    if(ZPresionado&&event->key()!=Qt::Key_Z)
    {

        detenerDisparo();

    }



    switch (event->key()) {
    case Qt::Key_Shift:
        shiftPresionado = true;
        break;
    case Qt::Key_Left:
        izquierdaPresionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Right:
        derechaPresionada = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Up:
        arribaPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Down:
        abajoPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;
    case Qt::Key_Z:
        ZPresionado = true;
        if (!movimientoTimer->isActive()) movimientoTimer->start();
        break;

    case Qt::Key_C:
        intentarCurarse();
        break;
    case Qt::Key_X:
        if (!XPresionado) {           // se acaba de pulsar
            XPresionado = true;
            intentarMelee();          // primer golpe
        }
        break;

    }
}

void AtributosPersonaje::keyReleaseEvent(QKeyEvent* event) {
    if (!jugador) return;
    if(estadoCurandose)return;//si esta curando no mover

    switch (event->key()) {
    case Qt::Key_Shift:
        shiftPresionado = false;
        break;
    case Qt::Key_Left:
        izquierdaPresionada = false;
        break;
    case Qt::Key_Right:
        derechaPresionada = false;
        break;
    case Qt::Key_Up:
        arribaPresionado = false;
        break;
    case Qt::Key_Down:
        abajoPresionado = false;
        break;
    case Qt::Key_Z:
        ZPresionado = false;

        //si no esta disparando y no hay movimiento, volver a quedar quieto
        if(!disparandoAhora&&!izquierdaPresionada&&!derechaPresionada&&!arribaPresionado&&!abajoPresionado)
        {

            jugador->DetenerAnimacion();
            auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
            jugador->SetAnimacion(anim.ruta,anim.frames);

        }

        break;

    case Qt::Key_X:
        XPresionado = false;          // se solto la tecla
        break;


    }

}

void AtributosPersonaje::ResetearMovimiento()
{

    if (movimientoTimer && movimientoTimer->isActive())
        movimientoTimer->stop();

    shiftPresionado = false;
    izquierdaPresionada = false;
    derechaPresionada = false;
    arribaPresionado = false;
    abajoPresionado = false;
    ZPresionado = false;

    if (jugador)
    {
        jugador->DetenerAnimacion();
        auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
        jugador->SetAnimacion(anim.ruta,anim.frames);
    }

    ActualizarBarraVida();
}

void AtributosPersonaje::ActualizarBarraVida()
{

    if(!jugador)return;

    int vidaActual=jugador->getVidas();
    int vidaMaxima=30;//valor maximo de vida

    //con estas condicionales se asegura los limites
    if(vidaActual>vidaMaxima)vidaActual=vidaMaxima;
    if(vidaActual<0)vidaActual=0;

    //aqui se calcula el porcentaje
    float porcentaje=static_cast<float>(vidaActual)/vidaMaxima;
    int anchoTotal=barraVidaLabel->contentsRect().width();
    int altoTotal=barraVidaLabel->contentsRect().height();

    int anchoBarra=static_cast<int>(porcentaje*anchoTotal);

    QString colorBarra;
    if(vidaActual>=18)
    {

        colorBarra="green";

    }else if(vidaActual>=13){

        colorBarra="orange";

    }else{

        colorBarra="red";

    }

    barraVidaInterna->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(colorBarra));
    barraVidaInterna->setGeometry(0,0,anchoBarra,altoTotal);

    //aqui se hace el texto de la barra
    QString texto = QString("VIDA: %1 / %2").arg(vidaActual).arg(vidaMaxima);
    barraVidaTexto->setText(texto);

    barraVidaLabel->raise();
}

void AtributosPersonaje::ActualizarMuniciones()
{

    if(!jugador)return;

    int mun=jugador->getMuniciones();
    QString texto=QString("MUNICIONES: %1").arg(mun);
    labelMuniciones->setText(texto);
    labelMuniciones->raise();
}

void AtributosPersonaje::inicializarTabWidget() {
    tabWidget = new QTabWidget(this);
    tabWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 2px solid #444; background: #222; border-radius: 5px; }"
        "QTabBar::tab { background: #333; color: white; padding: 8px; border-top-left-radius: 5px; border-top-right-radius: 5px; }"
        "QTabBar::tab:selected { background: #555; }"
        "QTabBar::tab:hover { background: #444; }"
        );

    mapaTab = new QWidget();

    QLabel* backgroundLabel = new QLabel(mapaTab);
    backgroundLabel->setAlignment(Qt::AlignCenter);
    backgroundLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPixmap background(":/imagenes/assets/mapas/Mapa.jpg");
    if(!background.isNull()) {
        backgroundLabel->setPixmap(background);
        backgroundLabel->setScaledContents(true);
    }

    mapaWidget = new Mapa(mapaTab);
    mapaWidget->setStyleSheet("background: transparent;");
    mapaWidget->setAttribute(Qt::WA_TranslucentBackground);

    // Crear el label de distancias
    labelDistancias = new QLabel(mapaTab);
    labelDistancias->setAlignment(Qt::AlignCenter);

    QPixmap fondoLabel(":/imagenes/assets/mapas/LetreroEntrar.png");
    if(!fondoLabel.isNull()) {
        fondoLabel = fondoLabel.scaled(200, 60, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        labelDistancias->setPixmap(fondoLabel);
        labelDistancias->setFixedSize(165,70);
    } else {
        labelDistancias->setStyleSheet(
            "QLabel {"
            "   background-color: rgba(0, 0, 0, 180);"
            "   color: white;"
            "   font-size: 12px;"
            "   padding: 8px;"
            "   border-radius: 8px;"
            "   border: 1px solid #555;"
            "   min-width: 100px;"
            "   min-height: 30px;"
            "}"
            );
    }

    QLabel *textoLabel = new QLabel(labelDistancias);
    textoLabel->setAlignment(Qt::AlignCenter);
    textoLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   background: transparent;"
        "}"
        );
    textoLabel->setText("Seleccione un destino");
    textoLabel->setGeometry(0, 0, labelDistancias->width(), labelDistancias->height());

    labelDistancias->move(10, 10);
    labelDistancias->raise();
    labelDistancias->show();

    // Conectar la señal para actualizar el texto
    connect(mapaWidget, &Mapa::actualizarDistancias, this, [this, textoLabel](float principal, float alterna,const QString& origen, const QString& destino) {
        QString texto;
        if (principal > 0 && alterna > 0) {
            texto = QString("Ruta rápida: %1 km\nAlternativa: %2 km")
                        .arg(principal, 0, 'f', 1)
                        .arg(alterna, 0, 'f', 1);
        } else if (principal > 0) {
            texto = QString("Distancia: %1 km").arg(principal, 0, 'f', 1);
        } else {
            texto = "No hay rutas disponibles";
        }
        textoLabel->setText(texto);
    });

    // Conectar la señal para actualizar el texto
    connect(mapaWidget, &Mapa::actualizarDistancias, this, [this, textoLabel](float principal, float alterna, const QString& origen, const QString& destino) {
        QString texto;

        QString ruta = QString("<div style='margin-bottom: 5px; font-size: 11fpx;'>%1 → %2</div><br/>").arg(origen).arg(destino);

        if (principal > 0 && alterna > 0) {
            texto = ruta +
                    QString("<span style='color: yellow;'>Ruta rápida: %1 km</span><br/>"
                            "<span style='color: #3498db;'>Alternativa: %2 km</span>")
                        .arg(principal, 0, 'f', 1)
                        .arg(alterna, 0, 'f', 1);
        } else if (principal > 0) {
            texto = ruta +
                    QString("<span style='color: yellow;'>Distancia: %1 km</span>")
                        .arg(principal, 0, 'f', 1);
        } else {
            texto = "No hay rutas disponibles";
        }
        textoLabel->setText(texto);
    });

    // Conectar señal de seleccion de nodo
    connect(mapaWidget, &Mapa::nodoSeleccionadoDesdeCompacto, this, [this](const QString& nodo) {
        QMessageBox::information(this, "Nodo seleccionado",
                                 QString("Has seleccionado: %1").arg(nodo));
    });

    mapaWidget->setModoCompacto(true);
    mapaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    QStackedLayout* stackedLayout = new QStackedLayout(mapaTab);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);
    stackedLayout->addWidget(backgroundLabel);
    stackedLayout->addWidget(mapaWidget);
    stackedLayout->setContentsMargins(0, 0, 0, 0);


    backgroundLabel->lower();
    mapaWidget->raise();

    // Configurar pestaña de inventario
    inventarioWidget = new InventarioWidget(Inventario::getInstance());
    tablaWidget = new TablaWidget(&TablaHash::getInstance(), &TablaHash::getInstance(), this);
    //WidgetDesbloqueo *widget = new WidgetDesbloqueo(this);

    tabWidget->addTab(mapaTab, "Mapa");
    tabWidget->addTab(inventarioWidget, "Inventario");



    //==================================Crear pestaña de personajes/habilidades=================================================================
    QScrollArea*scrollArea=new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: #222; border: none;");

    QWidget*contenidoScroll=new QWidget();
    QVBoxLayout*layoutPersonajes=new QVBoxLayout(contenidoScroll);
    layoutPersonajes->setSpacing(30);
    layoutPersonajes->setContentsMargins(30,20,30,20);

    // Lista de skins
    struct Skin
    {

        QString nombre;
        QString rutaImagen;
        bool desbloqueado;

    };

    QVector<Skin> skins=
    {

    {"P1",":/imagenes/assets/protagonista/P1.png",true},
        {"P2",":/imagenes/assets/protagonista/P2.png",Inventario::getInstance()->getPersonajeP2Desbloqueado()},
    {"P3",":/imagenes/assets/protagonista/P3.png",Inventario::getInstance()->getPersonajeP3Desbloqueado()}

};

QVector<QPushButton*> botonesUsar;

for(int i=0;i<skins.size();++i)
{
    QVBoxLayout*skinLayout=new QVBoxLayout();

    QLabel*imagenLabel=new QLabel();
    QPixmap img(skins[i].rutaImagen);
    imagenLabel->setPixmap(img.scaled(220, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imagenLabel->setAlignment(Qt::AlignCenter);
    imagenLabel->setStyleSheet("background-color: black; border-radius: 10px;");
    skinLayout->addWidget(imagenLabel);

    QLabel* nombreLabel = new QLabel(skins[i].nombre);
    nombreLabel->setAlignment(Qt::AlignCenter);
    nombreLabel->setStyleSheet("color: white; font-weight: bold; font-size: 18px;");
    skinLayout->addWidget(nombreLabel);

    QPushButton* usarBtn = new QPushButton();
    usarBtn->setEnabled(skins[i].desbloqueado);
    usarBtn->setText("Cambiar");
    if (skins[i].desbloqueado) {
        usarBtn->setText("Cambiar");
        usarBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #5a5aff;"
            "   color: white;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 6px 20px;"
            "   border-radius: 8px;"
            "}"
            "QPushButton:hover { background-color: #7373ff; }"
            );
    } else {
        usarBtn->setText("🔒 Arma bloqueada");
        usarBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #444;"
            "   color: red;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 6px 20px;"
            "   border-radius: 8px;"
            "}"
            );
    }
    botonesUsar.append(usarBtn);
    skinLayout->addWidget(usarBtn);

    QLabel* habilidad1 = new QLabel("Habilidad 1: Vacía");
    QLabel* habilidad2 = new QLabel("Habilidad 2: Vacía");
    habilidad1->setStyleSheet("color: lightgray; font-style: italic;");
    habilidad2->setStyleSheet("color: lightgray; font-style: italic;");
    habilidad1->setAlignment(Qt::AlignCenter);
    habilidad2->setAlignment(Qt::AlignCenter);
    skinLayout->addWidget(habilidad1);
    skinLayout->addWidget(habilidad2);

    QFrame* skinFrame = new QFrame();
    skinFrame->setStyleSheet("background-color: #2d2d2d; border: 2px solid #555; border-radius: 12px;");
    skinFrame->setLayout(skinLayout);

    layoutPersonajes->addWidget(skinFrame);

    connect(usarBtn, &QPushButton::clicked, this, [=]() {

        if (!skins[i].desbloqueado)
            return;

        //aqui verifica si el jugador ya esta usando este personaje
        if (jugador->personajeActual==static_cast<personaje::TipoPersonaje>(i))
            return;

        //aqui cambia el personaje actual del jugador
        jugador->personajeActual=static_cast<personaje::TipoPersonaje>(i);

        //aqui cambia animacion a idle con el nuevo personaje
        auto anim = jugador->obtenerAnimacion("idle", jugador->personajeActual);
        jugador->SetAnimacion(anim.ruta, anim.frames);

        //aqui muestra notificacion visual del cambio
        mostrarNotificacion(QString("🧍 Atuendo cambiado a: %1").arg(skins[i].nombre));
    });
}

layoutPersonajes->addStretch();
contenidoScroll->setLayout(layoutPersonajes);
scrollArea->setWidget(contenidoScroll);
tabWidget->addTab(scrollArea, "Personajes - Habilidades");
tabWidget->addTab(tablaWidget, "Niveles - NPCs");

//=======================TERMINA HABILIDADES===========================================================================
tabWidget->resize(700, 450);

// Boton de cerrar
QPushButton* closeButton = new QPushButton("×", tabWidget);
closeButton->setStyleSheet(
    "QPushButton {"
    "   background: #ff5555;"
    "   color: white;"
    "   border: none;"
    "   border-radius: 10px;"
    "   min-width: 20px;"
    "   max-width: 20px;"
    "   min-height: 20px;"
    "   max-height: 20px;"
    "}"
    "QPushButton:hover {"
    "   background: #ff3333;"
    "}"
    );
closeButton->move(tabWidget->width() - 30, 10);
connect(closeButton, &QPushButton::clicked, tabWidget, &QWidget::hide);
}

void AtributosPersonaje::intentarCurarse()
{

    if(estadoCurandose||!jugador)return;

    if(izquierdaPresionada||derechaPresionada||arribaPresionado||abajoPresionado)
    {

        QMessageBox::information(this,"Movimiento activo","Detente para poder curarte.");
        return;

    }

    if(jugador->getVidas()>=30)
    {

        QMessageBox::information(this,"Vida completa","Ya tienes la vida completa.");
        return;

    }

    NodoInventario*curar1=Inventario::getInstance()->buscar(Inventario::getInstance()->obtenerRaiz(),"curar1");
    NodoInventario*curar2=Inventario::getInstance()->buscar(Inventario::getInstance()->obtenerRaiz(),"curar2");

    //si quiere curarse y tiene los 2 botiquines mostrar cual de los 2 usarrrr
    if(curar1&&curar1->cantidad>0&&curar2&&curar2->cantidad>0)
    {

        mostrarSeleccionBotiquin();
        return;

    }

    if(curar2&&curar2->cantidad>0)
    {

        iniciarCuracion(20,"curar2");

    }else if(curar1&&curar1->cantidad>0){

        iniciarCuracion(10,"curar1");

    }else{

        QMessageBox::information(this,"Sin botiquines","No tienes botiquines para curarte.");

    }

}

void AtributosPersonaje::iniciarCuracion(int cantidad, const QString &tipo)
{

    estadoCurandose=true;
    ResetearMovimiento();
    auto anim=jugador->obtenerAnimacion("curar",jugador->personajeActual);
    jugador->SetAnimacion(anim.ruta,anim.frames);

    if(curacionTimer)
    {

        curacionTimer->stop();
        delete curacionTimer;

    }

    curacionTimer=new QTimer(this);
    curacionTimer->setSingleShot(true);
    connect(curacionTimer,&QTimer::timeout,this,[=]()
            {

                terminarCuracion(cantidad);
                Inventario::getInstance()->insertarObjeto(tipo,-1,"","");

            });
    curacionTimer->start(2000);//2 segundos en curarse

}

void AtributosPersonaje::terminarCuracion(int cantidad)
{

    estadoCurandose=false;
    auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
    jugador->SetAnimacion(anim.ruta,anim.frames);//quitar efecto, vuelve a la normalidad

    int nuevaVida=jugador->getVidas()+cantidad;
    if(nuevaVida>30)nuevaVida=30;
    jugador->setVidas(nuevaVida);

    ActualizarBarraVida();

}

void AtributosPersonaje::mostrarSeleccionBotiquin()
{

    QMessageBox msgBox;
    msgBox.setWindowTitle("Elegir Botiquin");
    msgBox.setText("Tienes dos botiquines. ¿Cuál deseas usar?");
    QPushButton*btn10=msgBox.addButton("Curar1 (+10 vida)",QMessageBox::YesRole);
    QPushButton*btn20=msgBox.addButton("Curar2 (+20 vida)",QMessageBox::NoRole);
    msgBox.exec();

    if(msgBox.clickedButton()==btn10)
    {

        iniciarCuracion(10,"curar1");

    }else if(msgBox.clickedButton()==btn20){

        iniciarCuracion(20,"curar2");

    }

}

void AtributosPersonaje::intentarDisparar()
{

    if(!ZPresionado||!puedeDisparar||disparandoAhora||jugador->getMuniciones()<=0)
        return;

    disparandoAhora=true;
    jugador->setMuniciones(jugador->getMuniciones()-1);
    auto anim=jugador->obtenerAnimacion("shot",jugador->personajeActual);
    jugador->SetAnimacion(anim.ruta,anim.frames);
    ActualizarMuniciones();

    // Crear bala
    Bala* bala=new Bala(this);
    //daño de la bala dependiendo del arma
    int danio=3;
    if(jugador->personajeActual==personaje::P2)
    {

        danio=4;

    }else if(jugador->personajeActual==personaje::P3){

        danio=5;

    }
    bala->setDanio(danio);
    int offsetX=jugador->miradoDerecha?130:(128-130-10);//calculando de donde saldra la bala
    int offsetY=90;
    bala->move(jugador->x()+offsetX,jugador->y()+offsetY);
    bala->Disparar(jugador->miradoDerecha);
    balasActivas.append(bala);


    connect(bala,&Bala::impactoBala,this,[=](Bala*b)
            {

                balasActivas.removeOne(b);
                b->deleteLater();

            });

    // Timer para permitir el siguiente disparo
    if(disparoTimer)
    {

        disparoTimer->stop();
        delete disparoTimer;

    }

    disparoTimer=new QTimer(this);
    disparoTimer->setSingleShot(true);
    connect(disparoTimer, &QTimer::timeout,this,[=]()
            {

                disparandoAhora=false;

                // Si no hay municion, cancelar el estado Z
                if(jugador->getMuniciones()==0)
                    ZPresionado=false;

                if(!ZPresionado&&!izquierdaPresionada&&!derechaPresionada&&!arribaPresionado&&!abajoPresionado)
                {

                    auto anim=jugador->obtenerAnimacion("idle",jugador->personajeActual);
                    jugador->SetAnimacion(anim.ruta,anim.frames);

                }

                delete disparoTimer;
                disparoTimer=nullptr;
            });
    disparoTimer->start(400);

}

void AtributosPersonaje::mostrarNotificacion(const QString &texto)
{

    if(!labelNotificacion)
    {

        labelNotificacion=new QLabel(texto,this);
        labelNotificacion->setStyleSheet("background: rgba(0, 0, 0, 200); color: #00ffcc; font-size: 16px; padding: 10px; border-radius: 8px; border: 2px solid #00ffcc;");
        labelNotificacion->setAlignment(Qt::AlignCenter);
        labelNotificacion->setFixedSize(400,100);

    }

    labelNotificacion->setText(texto);
    labelNotificacion->move((width()-labelNotificacion->width())/2,40);
    labelNotificacion->show();
    labelNotificacion->raise();

    QTimer::singleShot(2500, labelNotificacion, [=]() {
        labelNotificacion->hide();
    });

}
void AtributosPersonaje::intentarMelee()
{
    if (!puedeMelee || !jugador || jugador->estaMuertoPersonaje() || estadoCurandose)
        return;

    puedeMelee = false;

    // Animación melee
    auto anim = jugador->obtenerAnimacion("melee", jugador->personajeActual);
    jugador->SetAnimacion(anim.ruta, anim.frames);

    // --- HITBOX MELEE ---
    QRect hitbox;
    if (jugador->miradoDerecha)
        hitbox = QRect(jugador->x() + jugador->width(), jugador->y() + 32, 50, 60);
    else
        hitbox = QRect(jugador->x() - 50, jugador->y() + 32, 50, 60);

    // Busca zombies en la escena (todos los QLabel hijos)
    QList<QLabel*> posibles = this->findChildren<QLabel*>();
    bool golpeoAlMenosUno = false;

    for(QLabel* obj:posibles)
    {
        if(obj->inherits("Zombie"))
        {
            Zombie* z=static_cast<Zombie*>(obj);
            if (z->muerto) continue;

            QRect zRect=z->geometry();

            //Verificar distancia simple entre centro del jugador y zombieeee
            int dx=std::abs(z->x()-jugador->x());
            int dy=std::abs(z->y()-jugador->y());

            if(dx<50 && dy<50&&hitbox.intersects(zRect))
            {
                z->recibirDanio(1);
                golpeoAlMenosUno=true;
            }
        }
    }

    if (golpeoAlMenosUno) {
        mostrarNotificacion("👊 ¡Golpeaste a un zombie!");
    }

    // COOLDOWN
    if (!meleeTimer)
        meleeTimer = new QTimer(this);

    meleeTimer->setSingleShot(true);
    connect(meleeTimer, &QTimer::timeout, this, [=]() {
        puedeMelee=true;

        if(!XPresionado&&jugador)
        {
            auto animIdle = jugador->obtenerAnimacion("idle", jugador->personajeActual);
            jugador->SetAnimacion(animIdle.ruta, animIdle.frames);
        }
    });

    meleeTimer->start(450);
}

void AtributosPersonaje::detenerDisparo()
{

    //marcamos aqui ya no queremos disparar
    ZPresionado=false;

    //aqui se cancela el timer de cadencia y resetea la bandera
    if(disparoTimer)
    {

        disparoTimer->stop();
        disparoTimer->deleteLater();
        disparoTimer=nullptr;

    }
    disparandoAhora=false;

    //si no se esta moviendo vuelve al idle
    if(!izquierdaPresionada&&!derechaPresionada&&!arribaPresionado&&!abajoPresionado&&jugador)
    {

        jugador->DetenerAnimacion();
        auto animIdle=jugador->obtenerAnimacion("idle", jugador->personajeActual);
        jugador->SetAnimacion(animIdle.ruta, animIdle.frames);

    }

}

void AtributosPersonaje::CancelarCuracion()
{

    if(!estadoCurandose)return;

    estadoCurandose=false;

    if(curacionTimer)
    {

        curacionTimer->stop();
        curacionTimer->deleteLater();
        curacionTimer=nullptr;

    }

    auto animIdle=jugador->obtenerAnimacion("idle",jugador->personajeActual);
    jugador->SetAnimacion(animIdle.ruta,animIdle.frames);

    mostrarNotificacion("CURACION INTERRUMPIDA POR DAÑO");

}

void AtributosPersonaje::ActualizarBarraEscudo()
{
    if(!jugador)return;

    int escudoActual=jugador->getEscudo();
    int escudoMaximo=20;

    escudoActual=std::clamp(escudoActual,0,escudoMaximo);

    float porcentaje=static_cast<float>(escudoActual)/escudoMaximo;
    int anchoTotal=barraEscudoLabel->contentsRect().width();
    int altoTotal=barraEscudoLabel->contentsRect().height();

    int anchoBarra=static_cast<int>(porcentaje* anchoTotal);

    barraEscudoInterna->setGeometry(0,0,anchoBarra,altoTotal);

    QString texto=QString("ESCUDO: %1 / %2").arg(escudoActual).arg(escudoMaximo);
    barraEscudoTexto->setText(texto);

    barraEscudoLabel->raise();
}
