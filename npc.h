#ifndef NPC_H
#define NPC_H

#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QStringList>
#include"inventario.h"
#include "NodoDialogo.h"

class QLabel;

class NPC : public QLabel
{
    Q_OBJECT
public:
    enum class Tipo { NPC1, NPC2, NPC3, NPC4, NPC5, NPC6 };

    NPC(Tipo tipo, QWidget* parent = nullptr);
    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void mostrarDialogo(class DialogoNPC* dialogoUI);
    void mostrarHintInteractuar();
    void ocultarHintInteractuar();
    bool estaHablando() const { return hablando; }
    void setInventario(Inventario* inv) { inventarioRef = inv; }

signals:
    void dialogoTerminado();

private slots:
    void AvanzarFrame();
    void manejarOpcionSeleccionada(int opcion);

private:
    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual;
    QStringList dialogos;
    int indiceDialogo;
    bool hablando;
    QTimer* animacionTimer;
    QTimer* dialogoTimer;
    bool dialogoFinalizado;
    QLabel* labelPresionaH;
    QString dialogoActual;
    DialogoNPC* dialogoActualUI = nullptr;
    bool yaHabloCompleto = false;
    Inventario* inventarioRef = nullptr;
    QLabel* labelNotificacion = nullptr;

    NodoDialogo* arbolDialogos;
    NodoDialogo* nodoActual;

    void mostrarNotificacion(const QString& texto);
    bool tieneItem(const QString& nombreItem);

    QPixmap obtenerImagenNPC() const;
    QStringList obtenerOpcionesDialogo() const;

    void construirArbolDecisiones();

    bool misionCompletada; // Ej: "llave_entregada"
    QString itemRequerido; // Ej: "llave"
    QString itemRecompensa; // Ej: "municiones"
};

#endif // NPC_H
