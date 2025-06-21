#ifndef MALL_H
#define MALL_H

#include "AtributosPersonaje.h"
#include "zombie.h"
#include <QLabel>
#include <QPixmap>
#include <QKeyEvent>

class Mall : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    void onMovimientoUpdate() override;
    void verificarZombiesYMostrarMensaje();
    QList<Zombie*> zombies;
    Zombie* zombieCercano;

    QPushButton*btnSalir=nullptr;

public:
    explicit Mall(personaje* jugadorExistente,QWidget* parent=nullptr);
    void keyPressEvent(QKeyEvent* event) override;
    void mostrarNotificacion(const QString& texto);
    bool eventFilter(QObject* obj, QEvent* event) override;

    QLabel* cofreLabel;
    QLabel* mensajeCofre;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    QLabel* labelNotificacion = nullptr;
    bool cofreAbiertoYa = false;
};

#endif // MALL_H
