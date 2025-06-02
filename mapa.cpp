#include "mapa.h"
#include <lobby.h>
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include<QPainter>
#include <QPushButton>


Mapa::Mapa(QWidget* parent) {

    this->resize(1280, 720);
    this->setWindowTitle("Mapa - Last hope");

    QPixmap fondoPixmap(":/imagenes/assets/mapas/Mapa.png");
    if(fondoPixmap.isNull())
    {

        qDebug()<<"Error al cargar imagen desde assets/mapas/lobby.jpg";

    }else{

        QLabel*fondo=new QLabel(this);
        fondo->setPixmap(fondoPixmap.scaled(this->size()));
        fondo->setGeometry(0,0,width(),height());

    }

    // Boton para volver al lobby
    QPushButton *btnVolver = new QPushButton(this);
    btnVolver->setText("⬅");
    btnVolver->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 40px; }");
    btnVolver->move(10,10);

    connect(btnVolver, &QPushButton::clicked, this, [this]() {
        lobby* l = new lobby();
        l->show();
        this->close();
        qDebug() << "Volviendo al lobby...!";
    });
}
