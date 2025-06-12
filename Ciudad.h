#include "AtributosPersonaje.h"
#include "zombie.h"
#include"bala.h"


class Ciudad : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;
    void onMovimientoUpdate() override;
    void verificarZombiesYMostrarMensaje();


public:
    Ciudad(QWidget* parent = nullptr);

    QLabel* cofreLabel;
    QLabel* mensajeCofre;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    bool cofreAbiertoYa = false;
    void keyPressEvent(QKeyEvent* event) override;
    QLabel* labelNotificacion = nullptr;
    void mostrarNotificacion(const QString& texto);

};
