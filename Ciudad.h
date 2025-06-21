#include "AtributosPersonaje.h"
#include "zombie.h"
#include"bala.h"
#include"personaje.h"
#include<QPushButton>


class Ciudad : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;
    void onMovimientoUpdate() override;
    void verificarZombiesYMostrarMensaje();

    QPushButton*btnSalir=nullptr;

public:
   explicit Ciudad(personaje* jugadorExistente,QWidget* parent=nullptr);

    QLabel* cofreLabel;
    QLabel* mensajeCofre;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    bool cofreAbiertoYa = false;
    void keyPressEvent(QKeyEvent* event) override;
    QLabel* labelNotificacion = nullptr;
    void mostrarNotificacion(const QString& texto);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

};
