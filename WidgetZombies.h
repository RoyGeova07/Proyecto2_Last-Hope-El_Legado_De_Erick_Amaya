#ifndef WIDGETZOMBIES_
#define WIDGETZOMBIES_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include "ArbolAvl.h"

class WidgetZombies : public QWidget {
    Q_OBJECT

public:
    explicit WidgetZombies(ArbolAvl* arbol, QWidget* parent = nullptr);
    void actualizar(bool ordenPorNivel = true);

private:
    ArbolAvl* arbolZombies;
    QVBoxLayout* layoutZombies;
    QScrollArea* scroll;
};

#endif // WIDGETZOMBIES_H
