#include "personaje.h"
#include<QDebug>

personaje::personaje(QWidget*parent):QLabel(parent),frameActual(0),miradoDerecha(true),ultimaDireccionDerecha(true),
    municiones(80)
{

    this->resize(128,128);//aqui tamanio del frame
    this->move(600,500);//aqui posiciion inicial del personaje

    //Cargar datos guardados o usar valores por defecto si no hay archivo
    QMap<QString, int> datos = cargarDatosJugador();
    vida = datos.value("Vida", 30);
    if(vida<=0)
    {

        vida=30;
        guardarDatosJugador();

    }

    energia = datos.value("Energia", 10);
    municiones = datos.value("Municiones", 80);

    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&personaje::AvanzarFrame);

}

void personaje::SetAnimacion(const QString &ruta, int cantidadFrames)
{

    bool debeRefrescarFlip=false;
    if(estaMuerto)return;

    if(miradoDerecha!=ultimaDireccionDerecha)
    {

        debeRefrescarFlip=true;
        ultimaDireccionDerecha=miradoDerecha;

    }

    if(animacionActual==ruta&&!debeRefrescarFlip)
    {

        //aqui, si ya es la animacion actual, asegurarse que el timer esta corriendo
        if(!timer->isActive())
            timer->start(100);
        return;

    }

    QPixmap spriteSheet(ruta);
    if(spriteSheet.isNull())
    {

        qDebug()<<"no se pudo cargar la animacion: "<<ruta;
        return;

    }
    animacionActual=ruta;
    frames.clear();
    frameActual=0;//con esto reinicia desde el primer frame

    int frameAncho=spriteSheet.width()/cantidadFrames;

    for(int i=0;i<cantidadFrames;++i)
    {

        QPixmap frame = spriteSheet.copy(i*frameAncho,0,frameAncho,spriteSheet.height());
        if(!miradoDerecha)
        {

            frame=frame.transformed(QTransform().scale(-1,1));//volteo horizontallll

        }
        frames.append(frame);

    }

    setPixmap(frames[frameActual]);//con esto muestro el primer frame

    timer->start(100);//con esto inicia para que avance la primera animacion


}

void personaje::DetenerAnimacion()
{

    timer->stop();
    if(!frames.isEmpty())
    {

        setPixmap(frames[0]);//dejar el primer frame

    }
    animacionActual.clear();//reinicia la animacion actual

}

void personaje::AvanzarFrame()
{

    if(frames.isEmpty())return;

    frameActual=(frameActual+1)%frames.size();
    setPixmap(frames[frameActual]);

}

void personaje::MoverIzquierda()
{

    this->move(x()-getVelocidadMovimiento(),y());
    miradoDerecha=false;

}

void personaje::MoverDerecha()
{

    this->move(x()+getVelocidadMovimiento(),y());
    miradoDerecha=true;

}

void personaje::MoverArriba()
{

    this->move(x(),y()-getVelocidadMovimiento());

}

void personaje::MoverAbajo()
{

    this->move(x(),y()+getVelocidadMovimiento());

}

void personaje::MoverSiNoColisiona(int dx, int dy, const QVector<QRect> &obstaculos)
{

    QRect nuevaPos=this->geometry();
    nuevaPos.translate(dx,dy);

    //con este for, revisa la colision con cada obstaculo
    for(const QRect& obs:obstaculos)
    {

        if(nuevaPos.intersects(obs))
        {

            //si colisiona, no se mueve
            return;

        }

    }
    //si no, si se mueve
    this->move(nuevaPos.topLeft());

    //aqui se actualiza la direccion para el flip
    if(dx>0)miradoDerecha=true;
    else if(dx<0)miradoDerecha=false;

}

void personaje::guardarDatosJugador()
{
    QFile archivo("jugador.dat");
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&archivo);
        out << "Vida: " << vida << "\n";
        out << "Energia: " << energia << "\n";
        out << "Municiones: " << municiones << "\n";
        archivo.close();
        qDebug() << "Datos del jugador guardados correctamente.";
    } else {
        qDebug() << "Error al abrir el archivo para guardar los datos del jugador.";
    }
}

QMap<QString, int> personaje::cargarDatosJugador() {
    QMap<QString, int> datos;
    QFile archivo("jugador.dat");

    // Valores por defecto si el archivo no existe o esta corrupto
    datos["Vida"] = 30;
    datos["Energia"] = 10;
    datos["Municiones"] = 80;

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return datos;
    }

    QTextStream in(&archivo);
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();  // Elimina espacios extras
        QStringList partes = linea.split(": ");
        if (partes.size() == 2) {
            QString clave = partes[0];  // "Vida", "Energia", "Municiones"
            int valor = partes[1].toInt();
            datos[clave] = valor;  // Actualiza el mapa
        }
    }

    archivo.close();
    return datos;
}

void personaje::Morir()
{
    if (estaMuerto) return;

    estaMuerto = true;

    timer->stop();
    animacionActual.clear();

    auto anim = obtenerAnimacion("dead", personajeActual);
    frames.clear();
    QPixmap spriteSheet(anim.ruta);
    int frameAncho = spriteSheet.width() / anim.frames;

    for (int i = 0; i < anim.frames; ++i) {
        QPixmap frame = spriteSheet.copy(i * frameAncho, 0, frameAncho, spriteSheet.height());
        if (!miradoDerecha)
            frame = frame.transformed(QTransform().scale(-1, 1));
        frames.append(frame);
    }

    frameActual = 0;
    setPixmap(frames[frameActual]);

    // Animar muerte y detener al finalizar
    timer->start(100);
    connect(timer, &QTimer::timeout, this, [=]() mutable {
        if (frameActual + 1 < frames.size()) {
            ++frameActual;
            setPixmap(frames[frameActual]);
        } else {
            timer->stop();
        }
    });
}

personaje::DatosAnimacion personaje::obtenerAnimacion(const QString &tipo, TipoPersonaje personaje)
{

    QString base=":/imagenes/assets/protagonista/";
    DatosAnimacion datos;

    if(personaje==P1)
    {
        if(tipo=="idle")     datos={base+"Idle.png",         7};
        else if(tipo=="run") datos={base+"Run.png",          8};
        else if(tipo=="walk")datos={base+"Walk.png",         7};
        else if(tipo=="shot")datos={base+"Shot_1.png",       4};
        else if(tipo=="curar")datos={base+"curandose.png",   7};
        else if(tipo=="dead") datos={base+"Dead.png",        4};
        else if(tipo=="melee") datos={base+"Attack_P1.png", 3};


    }else if(personaje==P2){

        if(tipo=="idle")     datos={base+"Idle_P2.png",      9};
        else if(tipo=="run")datos={base+"Run_P2.png",       8};
        else if(tipo=="walk")datos={base+"Walk_P2.png",      8};
        else if(tipo=="shot")datos={base+"Shot_1_P2.png",    4};
        else if(tipo=="curar")datos={base+"Curandose_P2.png",9};
        else if(tipo=="dead") datos={base+"Dead_P2.png",     4};
        else if(tipo=="melee")datos={base+"Attack_P2.png", 4};


    }else if(personaje==P3){

        if(tipo=="idle")     datos={base+"Idle_P3.png",      7};
        else if(tipo=="run") datos={base+"Run_P3.png",       6};
        else if(tipo=="walk")datos={base+"Walk_P3.png",      8};
        else if(tipo=="shot")datos={base+"Shot_1_P3.png",    4};
        else if(tipo=="curar")datos={base+"Curandose_P3.png",7};
        else if(tipo=="dead") datos={base+"Dead_P3.png",     5};
        else if(tipo=="melee") datos={base+"Attack_P3.png", 5};


    }

    return datos;

}

void personaje::SetAnimacionMovimiento(int velocidad)
{

    if(personajeActual==P3)
    {

        velocidadMovimiento=std::max(1,velocidad-1);

    }else{

        this->velocidadMovimiento=velocidad;

    }


}
