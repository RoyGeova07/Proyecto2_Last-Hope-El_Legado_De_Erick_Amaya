#ifndef TABLAWIDGET_H
#define TABLAWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QHash>
#include "tablahash.h"

class TablaWidget : public QWidget {
    Q_OBJECT

public:
    explicit TablaWidget( TablaHash* nivelesRef, TablaHash* npcsRef,QWidget *parent = nullptr);
    void actualizarUI();


private:
    TablaHash* tablaNiveles;
    TablaHash* tablaNPCS;
    QGridLayout *layout;
    QHash<QString, QString> rutasNiveles;
    QHash<QString, QString> rutasNPCs;

    void cargarImagenes();
    void configurarRutasNiveles();
    void configurarRutasNPCs();
    QWidget* crearImagenConCandado(const QString &ruta, bool descubierto,const QString &nombre);
    QPixmap aplicarCandado(const QPixmap &pixmap);

    //listas para que se impriman en orden
    QStringList ordenNiveles = {
        "Nivel1", "Nivel2", "Nivel3", "Nivel4", "Nivel5", "Nivel6", "Nivel7"
    };

    QStringList ordenNPCs = {
        "NPC1", "NPC2", "NPC3", "NPC4", "NPC5", "NPC6"
    };
};

#endif // TABLAWIDGET_H
