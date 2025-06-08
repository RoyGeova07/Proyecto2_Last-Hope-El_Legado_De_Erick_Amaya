#ifndef INVENTARIO_H
#define INVENTARIO_H


#include <QString>
#include <QDebug>

// Nodo del ABB
struct NodoInventario
{
    QString nombre;    // clave de orden
    int cantidad;
    QString tipo;
    QString uso;

    NodoInventario* izquierda;
    NodoInventario* derecha;

    NodoInventario(const QString& n, int cant, const QString& t, const QString& u)
        : nombre(n), cantidad(cant), tipo(t), uso(u), izquierda(nullptr), derecha(nullptr) {}
};

class Inventario
{

private:

    NodoInventario* raiz;

    //aqui funciones auxiliares recursivos
    NodoInventario* insertar(NodoInventario* nodo, NodoInventario* nuevo);
    NodoInventario* buscar(NodoInventario* nodo, const QString& nombre);
    NodoInventario* eliminar(NodoInventario* nodo, const QString& nombre);
    NodoInventario* encontrarMin(NodoInventario* nodo);
    void listarInOrden(NodoInventario* nodo);

    void destruirArbol(NodoInventario* nodo);
    Inventario();
    ~Inventario();

    // Prohibir copiar
    Inventario(const Inventario&)=delete;
    Inventario& operator=(const Inventario&)=delete;

public:

    static Inventario* getInstance();//singleton

    void insertarObjeto(const QString& nombre, int cantidad, const QString& tipo, const QString& uso);
    void eliminarObjeto(const QString& nombre);
    void buscarObjeto(const QString& nombre);
    void listarInventario();

    NodoInventario* obtenerRaiz() { return raiz; }

};

#endif // INVENTARIO_H
