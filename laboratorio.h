
#ifndef LABORATORIO_H
#define LABORATORIO_H

#include "AtributosPersonaje.h"
#include "zombie.h"
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

class laboratorio : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    void verificarZombiesYMostrarMensaje();
    QList<Zombie*> zombies;
    Zombie* zombieCercano = nullptr;

    // --- Para el cofre y notificaciones ---
    QLabel* cofreLabel = nullptr;
    QLabel* mensajeCofre = nullptr;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    QLabel* labelNotificacion = nullptr;
    QPushButton* btnSalir = nullptr;
    bool cofreAbiertoYa = false;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mostrarNotificacion(const QString& texto);
    void onMovimientoUpdate() override;

public:
    laboratorio(personaje* jugadorExistente, QWidget* parent = nullptr);
};

#endif // LABORATORIO_H
