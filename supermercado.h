#ifndef SUPERMERCADO_H
#define SUPERMERCADO_H

#include "AtributosPersonaje.h"
#include "zombie.h"
#include"personaje.h"

class supermercado : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

    // NUEVOS
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    QLabel* cofreLabel = nullptr;
    QLabel* mensajeCofre = nullptr;
    QLabel* labelNotificacion = nullptr;
    bool cofreAbiertoYa = false;

public:
    explicit supermercado(personaje*jugadorExistente,QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    void mostrarNotificacion(const QString& texto);
    void onMovimientoUpdate() override;
    void verificarZombiesYMostrarMensaje();
};

#endif // SUPERMERCADO_H
