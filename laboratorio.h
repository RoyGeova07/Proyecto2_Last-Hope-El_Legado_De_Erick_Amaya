#ifndef LABORATORIO_H
#define LABORATORIO_H

#include "AtributosPersonaje.h"
#include "zombie.h"
#include "npc.h"
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QList>

class laboratorio : public AtributosPersonaje {
    Q_OBJECT

private:
    void configurarEscena() override;
    void configurarObstaculos() override;
    void verificarZombiesYMostrarMensaje();
    bool zombiesVivos();
    bool estaCercaAntidoto();
    bool jugadorEstaCercaCofre();
    bool tieneLlaveFinal();
    void recogerAntidoto();
    void abrirCofreFinal();
    void mostrarFinalStandard();
    void mostrarFinalZombies();
    void deshabilitarControlesFinal();
    void mostrarDialogoFinalBonito();

    // --- Boss y enemigos ---
    QList<Zombie*> zombies;
    Zombie* boss = nullptr;
    Zombie* zombieCercano = nullptr;

    // --- Antídoto y mensajes ---
    QLabel* antidotoLabel = nullptr;
    QLabel* mensajeAntidoto = nullptr;
    bool antidotoRecogido = false;

    // --- Para el cofre y notificaciones ---
    QLabel* cofreLabel = nullptr;
    QLabel* mensajeCofre = nullptr;
    QPixmap cofreCerrado;
    QPixmap cofreAbierto;
    QLabel* labelNotificacion = nullptr;
    QPushButton* btnSalir = nullptr;
    bool cofreAbiertoYa = false;
    bool llaveUsadaEnFinal = false;

    // --- Final ---
    bool finalYaDesplegado = false;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mostrarNotificacion(const QString& texto);
    void onMovimientoUpdate() override;

public:
    laboratorio(personaje* jugadorExistente, QWidget* parent = nullptr);
};

#endif // LABORATORIO_H
