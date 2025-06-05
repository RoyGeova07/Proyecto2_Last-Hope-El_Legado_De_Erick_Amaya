#include "inicio.h"
#include "Ciudad.h"
#include<QPushButton>
#include<QLabel>
#include<QVBoxLayout>
#include<QPixmap>
#include<QPalette>
#include<QFont>
#include<QDebug>
#include<QMovie>
#include<QApplication>
#include<QScreen>
#include<QFile>
#include<QDir>
#include<QDebug>
#include<QDataStream>
#include<QMessageBox>
#include"lobby.h"

Inicio::Inicio(QWidget*parent):QWidget(parent)
{

    this->resize(1100,700);
    this->setWindowTitle("Last Hope");

    // CENTRAR VENTANA
    QScreen *pantalla = QApplication::primaryScreen();
    QRect centroPantalla = pantalla->availableGeometry();
    this->move(centroPantalla.center() - this->rect().center());

    //aqui el gif de fondo
    QLabel*giflabel=new QLabel(this);
    QMovie*movie=new QMovie(":/imagenes/inicio.gif");

    movie->setScaledSize(QSize(this->width(),this->height()));//MAS GRNADEEEE
    giflabel->setMovie(movie);
    giflabel->setGeometry(0,0,width(),height());
    giflabel->lower();
    movie->start();

    //aqui fuentes internas pix
    QFont fuenteTitulo("Courier New",26,QFont::Bold);
    QFont fuenteSubTitulo("Courier New",14,QFont::Bold);

    //aqui el titulo
    lblTitulo=new QLabel("Last Hope: ");
    lblTitulo->setFont(fuenteTitulo);
    lblTitulo->setStyleSheet("color: yellow;");
    lblTitulo->setAlignment(Qt::AlignCenter);

    //aqui el subtitulo
    miniTitulo=new QLabel("El legado de Erick Amaya");
    miniTitulo->setFont(fuenteSubTitulo);
    miniTitulo->setStyleSheet("color: white;");
    miniTitulo->setAlignment(Qt::AlignCenter);


    //aqui los botones
    btnNuevaPartida=new QPushButton("Nueva Partida");
    btnCargarPartida=new QPushButton("Cargar Partida");
    Salir=new QPushButton("Salir");

    QString estiloBoton =
        "QPushButton {"
        " background-color: rgba(30, 30, 30, 190);"
        " color: white;"
        " border: 2px solid #ccc;"
        " border-radius: 8px;"
        " font-family: 'Courier New';"
        " padding: 14px 28px;"
        " font-size: 16px;"
        " min-width: 280px;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(60, 60, 60, 220);"
        "}";

    btnNuevaPartida->setStyleSheet(estiloBoton);
    btnCargarPartida->setStyleSheet(estiloBoton);
    Salir->setStyleSheet(estiloBoton);

    //aqui el layout general
    QVBoxLayout *layoutVertical = new QVBoxLayout();
    layoutVertical->addWidget(lblTitulo);
    layoutVertical->addWidget(miniTitulo);
    layoutVertical->addSpacing(50);
    layoutVertical->addWidget(btnNuevaPartida,0,Qt::AlignCenter);
    layoutVertical->addWidget(btnCargarPartida,0,Qt::AlignCenter);
    layoutVertical->addWidget(Salir,0,Qt::AlignCenter);
    layoutVertical->addStretch();

    //aqui capa frontal sobre el gif
    QWidget*contenedor=new QWidget(this);
    contenedor->setLayout(layoutVertical);
    //contenedor->setStyleSheet("background-color: rgba(0,0,0,130); border-radius: 20px;");
    contenedor->setFixedSize(600,400);
    contenedor->move(width()/2-contenedor->width()/2,height()/2-contenedor->height()/2);
    contenedor->raise();//esto se asegura q este dentro del gif

    connect(Salir,&QPushButton::clicked,this,&Inicio::close);
    connect(btnNuevaPartida,&QPushButton::clicked,this,&Inicio::AbrirJuego);


}

void Inicio::AbrirJuego()
{

    QString rutaBase=QDir::currentPath();
    QDir directorio(rutaBase);

    if(!directorio.exists("Partidas"))
    {

        if(!directorio.mkdir("Partidas"))
        {

            QMessageBox::warning(this,"Error","No se pudo crear la carpeta de partidas");
            return;

        }

    }

    //aqui se crea el archivo
    QString rutaArchivo=rutaBase+"/Partidas/Partida1.dat";
    QFile archivo(rutaArchivo);

    if(archivo.open(QIODevice::WriteOnly))
    {

        QDataStream salida(&archivo);
        QString mensaje="Partida creada correctamente";
        salida<<mensaje;
        archivo.close();


        qDebug()<<"Archivo de partida creado en: "<<rutaArchivo;
        QMessageBox::information(this,"Nueva Partida","Partida creada exitosamente!");

    }else{

        QMessageBox::warning(this,"Error","No se pudo crear el archivo de partida");
        return;

    }

    lobby*ventanaLobby=new lobby();
    ventanaLobby->show();
    //Ciudad* ventana=new Ciudad();
    //ventana->show();


    this->close();

}
