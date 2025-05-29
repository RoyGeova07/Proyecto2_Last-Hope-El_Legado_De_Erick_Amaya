#ifndef LOBBY_H
#define LOBBY_H
#include"personaje.h"

#include<QWidget>

class lobby:public QWidget
{

    Q_OBJECT

public:
    explicit lobby(QWidget*parent=nullptr);


protected:
    void keyPressEvent(QKeyEvent*event)override;

private:

    personaje*jugador;

};

#endif // LOBBY_H
