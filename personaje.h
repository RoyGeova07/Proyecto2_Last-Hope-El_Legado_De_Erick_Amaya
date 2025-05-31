#ifndef PERSONAJE_H
#define PERSONAJE_H

#include<QLabel>
#include<QPixmap>
#include<QTimer>
#include<QStringList>

//para las colisicones necesito QRect
//que hace la libreria QRect?
//R:es una clase de Qt que representa un rectangulo.Es perfecto para decir:"El objeto ocupa este espacio en pantalla".

//Que es QVector<QRect>?
//es simplemente una lista de rectangulos.
//en este caso en una lista de obstaculos

//lo uso asi porque tiene muchos obstaculos


class personaje:public QLabel
{

    Q_OBJECT

public:
   explicit personaje(QWidget*parent=nullptr);

    void SetAnimacion(const QString&ruta,int cantidadFrames);
    void AvanzarFrame();
    void MoverIzquierda();
    void MoverDerecha();
    void MoverArriba();
    void MoverAbajo();
    void DetenerAnimacion();
    void SetAnimacionMovimiento(int velocidad);
    void MoverSiNoColisiona(int dx,int dy,const QVector<QRect>&obstaculos);
    int getVelocidadMoviento()const{return velocidadMovimiento;}

    QString animacionActual;//bandera para evitar que se reinicie una animacion si ya esta en curso

private:

    QVector<QPixmap>frames;
    int frameActual;
    QTimer*timer;
    int velocidadMovimiento;
    bool miradoDerecha;

    QVector<QRect> obstaculos; //aqui obstaculos

};

#endif // PERSONAJE_H
