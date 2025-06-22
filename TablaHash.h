#ifndef TABLAHASH_H
#define TABLAHASH_H

#include <QObject>
#include <QString>
#include <vector>
#include <list>
#include <mutex>

class TablaHash : public QObject {
    Q_OBJECT

private:
    int capacidad;
    std::vector<std::list<std::pair<QString, bool>>> tabla;

    explicit TablaHash(QObject *parent = nullptr, int cap = 10);
    TablaHash(const TablaHash&) = delete;
    TablaHash& operator=(const TablaHash&) = delete;

    int funcionHash(const QString &clave);

signals:
    void datoModificado(const QString& clave);

public:
    static TablaHash& getInstance();
    void insertar(const QString &clave, bool descubierto);
    bool estaDescubierto(const QString &clave);
    void descubrir(const QString &clave);
};

#endif // TABLAHASH_H
