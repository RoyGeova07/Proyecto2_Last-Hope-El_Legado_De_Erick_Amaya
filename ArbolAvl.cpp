#include "arbolavl.h"
#include <algorithm>

/* ---------- Rotaciones ---------- */
NodoAvl* ArbolAvl::rotDer(NodoAvl* y) {
    NodoAvl* x  = y->izquierda;
    NodoAvl* T2 = x->derecha;
    x->derecha = y;
    y->izquierda = T2;
    y->altura = std::max(altura(y->izquierda), altura(y->derecha)) + 1;
    x->altura = std::max(altura(x->izquierda), altura(x->derecha)) + 1;
    return x;
}

NodoAvl* ArbolAvl::rotIzq(NodoAvl* x) {
    NodoAvl* y  = x->derecha;
    NodoAvl* T2 = y->izquierda;
    y->izquierda = x;
    x->derecha = T2;
    x->altura = std::max(altura(x->izquierda), altura(x->derecha)) + 1;
    y->altura = std::max(altura(y->izquierda), altura(y->derecha)) + 1;
    return y;
}

/* ---------- Inserción ---------- */
NodoAvl* ArbolAvl::insertarRec(NodoAvl* n, const QString& nombre,
                               int vida, int danio, int velocidad,
                               int dificultad, const QString& recompensa)
{
    if (!n) return new NodoAvl(nombre, vida, danio, velocidad, dificultad, recompensa);

    if (dificultad < n->dificultad)
        n->izquierda = insertarRec(n->izquierda, nombre, vida, danio, velocidad, dificultad, recompensa);
    else if (dificultad > n->dificultad)
        n->derecha  = insertarRec(n->derecha,  nombre, vida, danio, velocidad, dificultad, recompensa);
    else
        return n;                 // no se permiten claves duplicadas

    n->altura = 1 + std::max(altura(n->izquierda), altura(n->derecha));
    int bf = balance(n);

    /* 4 casos */
    if (bf > 1 && dificultad < n->izquierda->dificultad)  return rotDer(n);
    if (bf < -1 && dificultad > n->derecha->dificultad)   return rotIzq(n);
    if (bf > 1 && dificultad > n->izquierda->dificultad)  { n->izquierda = rotIzq(n->izquierda);  return rotDer(n); }
    if (bf < -1 && dificultad < n->derecha->dificultad)   { n->derecha  = rotDer(n->derecha);     return rotIzq(n); }

    return n;
}

void ArbolAvl::insertar(const QString& nombre, int vida, int danio,
                        int velocidad, int dificultad, const QString& recompensa)
{
    raiz = insertarRec(raiz, nombre, vida, danio, velocidad, dificultad, recompensa);
}

/* ---------- Eliminación ---------- */
NodoAvl* ArbolAvl::minValorNodo(NodoAvl* n) {
    NodoAvl* actual = n;
    while (actual && actual->izquierda) actual = actual->izquierda;
    return actual;
}

NodoAvl* ArbolAvl::eliminarRec(NodoAvl* n, int dificultad) {
    if (!n) return n;

    /* búsqueda */
    if (dificultad < n->dificultad)
        n->izquierda = eliminarRec(n->izquierda, dificultad);
    else if (dificultad > n->dificultad)
        n->derecha  = eliminarRec(n->derecha,  dificultad);
    else {
        /* nodo encontrado */
        if (!n->izquierda || !n->derecha) {
            NodoAvl* temp = n->izquierda ? n->izquierda : n->derecha;
            if (!temp) {                      // sin hijos
                temp = n;
                n = nullptr;
            } else {                          // un hijo
                *n = *temp;
            }
            delete temp;
        } else {                              // dos hijos
            NodoAvl* temp = minValorNodo(n->derecha);
            n->nombre      = temp->nombre;
            n->vida        = temp->vida;
            n->danio       = temp->danio;
            n->velocidad   = temp->velocidad;
            n->dificultad  = temp->dificultad;
            n->recompensa  = temp->recompensa;
            n->derecha = eliminarRec(n->derecha, temp->dificultad);
        }
    }

    if (!n) return n;   // árbol vacío

    /* actualizar altura y balancear */
    n->altura = 1 + std::max(altura(n->izquierda), altura(n->derecha));
    int bf = balance(n);

    // 4 casos
    if (bf > 1 && balance(n->izquierda) >= 0)        return rotDer(n);
    if (bf > 1 && balance(n->izquierda) <  0)        { n->izquierda = rotIzq(n->izquierda); return rotDer(n); }
    if (bf < -1 && balance(n->derecha)  <= 0)        return rotIzq(n);
    if (bf < -1 && balance(n->derecha)  >  0)        { n->derecha  = rotDer(n->derecha);    return rotIzq(n); }

    return n;
}

void ArbolAvl::eliminar(int dificultad) {
    raiz = eliminarRec(raiz, dificultad);
}

/* ---------- Recorrido In-Orden ---------- */
void ArbolAvl::inOrden(NodoAvl* n, QList<NodoAvl*>& v, bool asc) {
    if (!n) return;
    if (asc) {
        inOrden(n->izquierda, v, asc);  v.append(n);  inOrden(n->derecha, v, asc);
    } else {
        inOrden(n->derecha,  v, asc);   v.append(n);  inOrden(n->izquierda, v, asc);
    }
}

QList<NodoAvl*> ArbolAvl::obtenerEnOrden(bool asc) {
    QList<NodoAvl*> v;
    inOrden(raiz, v, asc);
    return v;
}
