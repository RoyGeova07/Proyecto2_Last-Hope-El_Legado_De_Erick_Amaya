#ifndef ARBOLAVL_H
#define ARBOLAVL_H

#include <QString>
#include <QList>

/* ---------- Nodo ---------- */
struct NodoAvl {
    QString nombre;
    int vida;
    int danio;
    int velocidad;
    int dificultad;      //  ← clave del AVL
    QString recompensa;

    NodoAvl* izquierda;
    NodoAvl* derecha;
    int altura;

    NodoAvl(const QString& nom, int v, int d, int vel,
            int dif, const QString& recomp) :
        nombre(nom), vida(v), danio(d), velocidad(vel),
        dificultad(dif), recompensa(recomp),
        izquierda(nullptr), derecha(nullptr), altura(1) {}
};

/* ---------- Árbol ---------- */
class ArbolAvl {
private:
    NodoAvl* raiz;

    /* utilidades */
    int altura(NodoAvl* n)                { return n ? n->altura : 0; }
    int balance(NodoAvl* n)               { return n ? altura(n->izquierda) - altura(n->derecha) : 0; }
    NodoAvl* rotDer(NodoAvl* y);
    NodoAvl* rotIzq(NodoAvl* x);
    NodoAvl* minValorNodo(NodoAvl* n);

    /* inserción / eliminación recursivas */
    NodoAvl* insertarRec(NodoAvl* n, const QString& nombre,
                         int vida, int danio, int velocidad,
                         int dificultad, const QString& recompensa);

    NodoAvl* eliminarRec(NodoAvl* n, int dificultad);

    /* recorrido para UI */
    void inOrden(NodoAvl* n, QList<NodoAvl*>& v, bool asc);

public:
    ArbolAvl() : raiz(nullptr) {}

    /* API pública */
    void insertar(const QString& nombre, int vida, int danio,
                  int velocidad, int dificultad, const QString& recompensa);

    void eliminar(int dificultad);                 // ← nuevo

    QList<NodoAvl*> obtenerEnOrden(bool asc = true);
};

#endif // ARBOLAVL_H
