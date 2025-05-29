#ifndef PERSONAJE_H
#define PERSONAJE_H

#include<QLabel>
#include<QPixmap>
#include<QTimer>
#include<QStringList>

class personaje:public QLabel
{

    Q_OBJECT

public:
   explicit personaje(QWidget*parent=nullptr);

    void SetAnimacion(const QString&ruta,int cantidadFrames);
    void AvanzarFrame();
    void MoverIzquierda();
    void MoverDerecha();

private:

    QVector<QPixmap>frames;
    int frameActual;
    QTimer*timer;
    int velocidadMovimiento;


};

#endif // PERSONAJE_H
