#ifndef INICIO_H
#define INICIO_H
#include<QWidget>
#include"personaje.h"

class QPushButton;
class QLabel;

class Inicio:public QWidget
{

    Q_OBJECT

public:
    explicit Inicio(QWidget *parent=nullptr);

private:

    QLabel *lblTitulo;
    QLabel *miniTitulo;
    QPushButton*btnNuevaPartida;
    QPushButton*btnCargarPartida;
    QPushButton*Salir;

    personaje*jugador;

    void AbrirJuego();
    void AbrirMenuCargarPartida();

};

#endif // INICIO_H
