#ifndef BALA_H
#define BALA_H
#include<QLabel>
#include<QTimer>

class Bala:public QLabel
{

    Q_OBJECT

public:
    explicit Bala(QWidget*parent=nullptr);
    void Disparar(bool haciaDerecha);
    void setDanio(int d){danio=d;}
    int getDanio()const{return danio;}

signals:

    void impactoBala(Bala*bala);//se emite cuando colisiona con algo

private slots:

    void Mover();

private:

    int velocidad=10;
    bool direccionDerecha=true;
    QTimer*timer;
    int danio=2;

};

#endif // BALA_H
