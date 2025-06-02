#include "personaje.h"
#include<QDebug>

personaje::personaje(QWidget*parent):QLabel(parent),frameActual(0),velocidadMovimiento(10),miradoDerecha(true),
    vida(3),energia(10),municiones(20)
{

    this->resize(128,128);//aqui tamanio del frame
    this->move(600,500);//aqui posiciion inicial del personaje

    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&personaje::AvanzarFrame);

    guardarDatosJugador();

}

void personaje::SetAnimacion(const QString &ruta, int cantidadFrames)
{

    bool debeRefrescarFlip=false;

    //aqui si la direccion cambio la ultima vez que se dibujo
    static bool ultimaDireccionDerecha = true;

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

    this->move(x()-velocidadMovimiento,y());
    miradoDerecha=false;

}

void personaje::MoverDerecha()
{

    this->move(x()+velocidadMovimiento,y());
    miradoDerecha=true;

}

void personaje::MoverArriba()
{

    this->move(x(),y()-velocidadMovimiento);

}

void personaje::MoverAbajo()
{

    this->move(x(),y()+velocidadMovimiento);

}

void personaje::SetAnimacionMovimiento(int velocidad)
{

    velocidadMovimiento=velocidad;

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

    // Valores por defecto si el archivo no existe o está corrupto
    datos["Vida"] = 3;
    datos["Energia"] = 10;
    datos["Municiones"] = 30;

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
