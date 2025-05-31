#ifndef LOBBY_H
#define LOBBY_H
#include"personaje.h"
#include<QVector>
#include<QRect>

#include<QWidget>

class lobby:public QWidget
{

    Q_OBJECT

public:
    explicit lobby(QWidget*parent=nullptr);
protected:
    void keyPressEvent(QKeyEvent*event)override;
    void keyReleaseEvent(QKeyEvent*event)override;

private:

    personaje*jugador;
    bool shiftPresionado;
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool arribaPresionado;
    bool abajoPresionado;
    QTimer*movimientoTimer;
    QVector<QRect> obstaculos; //aqui obstaculos

};

#endif // LOBBY_H
