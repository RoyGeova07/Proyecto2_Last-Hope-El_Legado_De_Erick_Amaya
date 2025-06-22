#ifndef NPC_H
#define NPC_H

#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include"inventario.h"
#include"NodoDialogo.h"
#include"dialogonpc.h"

class DialogoNPC;


class NPC : public QLabel
{
    Q_OBJECT
public:
    enum class Tipo { NPC1, NPC2, NPC3, NPC4, NPC5, NPC6 };

    explicit NPC(Tipo tipo, QWidget* parent=nullptr);

    void SetAnimacion(const QString& ruta, int cantidadFrames);
    void mostrarDialogo(class DialogoNPC* dialogoUI);  // Cambiado a DialogoNPC
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

    // ----DATOS----

    Tipo tipo;
    QVector<QPixmap> frames;
    int frameActual=0;
    bool hablando=false;
    QTimer* animacionTimer=nullptr;
    QLabel* labelPresionaH=nullptr;
    DialogoNPC* dialogoActualUI = nullptr;
    Inventario* inventarioRef = nullptr;
    QLabel* labelNotificacion = nullptr;

    NodoDialogo* arbolDialogos=nullptr;
    NodoDialogo* nodoActual=nullptr;

    bool recompensaEntregada=false;

    void mostrarNotificacion(const QString& texto);
    bool tieneItem(const QString& nombreItem);


    QPixmap obtenerImagenNPC() const;
    QStringList obtenerOpcionesDialogo() const;

    void construirArbolDecisiones();

    bool misionCompletada; // Ej: "llave_entregada"
    QString itemRequerido; // Ej: "llave"
    QString itemRecompensa; // Ej: "municiones"

    void ejecutarConsecuencia(NodoDialogo* hoja);

  };

#endif // NPC_H
