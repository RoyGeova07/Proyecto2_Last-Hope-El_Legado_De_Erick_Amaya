#ifndef WIDGETNIVELES_H
#define WIDGETNIVELES_H

#include <QWidget>
#include "ArbolAvl.h"
#include"QVBoxLayout"

class WidgetNiveles : public QWidget {
    Q_OBJECT
public:
    explicit WidgetNiveles(ArbolAvl* arbolMapas, QWidget* parent = nullptr);
    void actualizar(bool ascendente);

private:
    ArbolAvl* arbol;
    QVBoxLayout* layout;
};

#endif // WIDGETNIVELES_H
