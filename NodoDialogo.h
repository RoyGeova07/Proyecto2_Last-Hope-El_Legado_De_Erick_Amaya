#ifndef NODODIALOGO_H
#define NODODIALOGO_H

#include <QString>
#include <QList>

class NodoDialogo {
public:
    QString texto;
    QStringList opciones;
    QString consecuencia;
    QList<NodoDialogo*> hijos;

    NodoDialogo(const QString& texto,
                const QStringList& opciones = {},
                const QString& consq = "")
        : texto(texto), opciones(opciones), consecuencia(consq) {}
};

#endif // NODODIALOGO_H
