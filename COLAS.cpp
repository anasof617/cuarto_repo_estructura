#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <cassert>
#include <utility>
#include <cstddef>



template <typename T>
class ColaCircular {
private:
    std::unique_ptr<T[]> datos;

    std::size_t frente;
    std::size_t cantidad;
    std::size_t capacidad;

    //Amplia la capacidad cuando la cola esta llena.
    void redimensionar(std::size_t nuevaCapacidad) {
        std::unique_ptr<T[]> nuevo =
            std::make_unique<T[]>(nuevaCapacidad);

        //Copia los elementos conservando el orden FIFO.
        for (std::size_t i = 0; i < cantidad; ++i) {
            nuevo[i] = datos[(frente + i) % capacidad];
        }

        datos = std::move(nuevo);
        capacidad = nuevaCapacidad;
        frente = 0;
    }

    //Garantiza espacio para una nueva insercion.}
    void asegurarEspacio() {
        if (cantidad == capacidad) {
            if (capacidad >
                static_cast<std::size_t>(-1) / 2) {
                throw std::length_error(
                    "La capacidad maxima fue alcanzada."
                );
            }

            redimensionar(capacidad * 2);
        }
    }

public:
    // Constructor.
    explicit ColaCircular(std::size_t capacidadInicial = 4)
        : datos(nullptr),
        frente(0),
        cantidad(0),
        capacidad(capacidadInicial) {

        if (capacidadInicial == 0) {
            throw std::invalid_argument(
                "La capacidad inicial debe ser mayor que cero."
            );
        }

        datos = std::make_unique<T[]>(capacidad);
    }

    // Deshabilita copias superficiales.
    ColaCircular(const ColaCircular&) = delete;
    ColaCircular& operator=(const ColaCircular&) = delete;

    // Permite mover la propiedad de la cola.
    ColaCircular(ColaCircular&&) noexcept = default;
    ColaCircular& operator=(ColaCircular&&) noexcept = default;

    // Inserta un elemento al final.
    void enqueue(const T& elemento) {
        asegurarEspacio();

        const std::size_t final =
            (frente + cantidad) % capacidad;

        datos[final] = elemento;
        ++cantidad;
    }

    //Retira y devuelve el primer elemento.
    T dequeue() {
        if (empty()) {
            throw std::underflow_error(
                "No se puede extraer: la cola esta vacia."
            );
        }

        T resultado = std::move(datos[frente]);

        frente = (frente + 1) % capacidad;
        --cantidad;

        // Normaliza el frente cuando queda vacía.
        if (cantidad == 0) {
            frente = 0;
        }

        return resultado;
    }

    // Consulta el primer elmento.
    const T& front() const {
        if (empty()) {
            throw std::underflow_error(
                "La cola esta vacia."
            );
        }

        return datos[frente];
    }

    // Consulta el ultimo elemento.
    const T& back() const {
        if (empty()) {
            throw std::underflow_error(
                "La cola esta vacia."
            );
        }

        const std::size_t final =
            (frente + cantidad - 1) % capacidad;

        return datos[final];
    }

    // indica si no hay elementos.
    bool empty() const noexcept {
        return cantidad == 0;
    }

    // Devuelve el numero de elementos.
    std::size_t size() const noexcept{
        return cantidad;
    }

    // Devuelve la capacidad actual.
    std::size_t capacity() const noexcept {
        return capacidad;
    }

    // Elimina todos los elementos logicos.
    void clear() noexcept {
        frente = 0;
        cantidad = 0;
    }

    // Muestra los elmentos en orden FIFO.
    void mostrar() const {
        std::cout << "Frente -> ";

        for (std::size_t i = 0; i < cantidad; ++i) {
            const std::size_t indice =
                (frente + i) % capacidad;

            std::cout << datos[indice];

            if (i + 1 < cantidad) {
                std::cout << "-> ";
            }
        }

        std::cout << " <- Final\n";
    }
};

// ==================================================================
// MODELO DE UNA SOLICITUD
// ==================================================================
struct Solicitud {
    int id;
    std::string cliente;
    std::string descripcion;
    

    // Permite imprimir una solicitud con cout.
    friend std::ostream& operator<<(
        std::ostream& salida,
        const Solicitud& solicitud
    ){
        salida << "[ID: " << solicitud.id
            << ", Cliente " << solicitud.cliente   
            << ", Solicitud: " << solicitud.descripcion
            << "]";

            return salida;
    }
};

// ====================================================================
// PRUEBAS AUTOMATIZADAS
// ====================================================================

void ejecutarPruebas() {
    ColaCircular<int> cola(3);

    assert(cola.empty());
    assert(cola.size() == 0);
    
    cola.enqueue(10);
    cola.enqueue(20);
    cola.enqueue(30);

    assert(cola.size() == 3);
    assert(cola.front() == 10);
    assert(cola.back() == 30);

    // El cuarto elemento provoca crecimiento.
    cola.enqueue(40);

    assert(cola.size() == 4);
    assert(cola.front() == 10);
    assert(cola.back() == 40);

    // Verifica el orden FIFO.
    assert(cola.dequeue() == 10);
    assert(cola.dequeue() == 20);
    assert(cola.dequeue() == 30);
    assert(cola.dequeue() == 40);

    assert(cola.empty());

    // Verifica que se detecte la extracion invalida.
    bool excepcionDetectada = false;

    try {
        cola.dequeue();
    } catch (const std::underflow_error&) {
        excepcionDetectada = true;
    }

    assert(excepcionDetectada);

    std::cout
        << "\nTodas las pruebas finalizaon correctamente \n";
}

// =================================================================
// PROGRAMA PRINCIPAL
// =================================================================

int main() {
    std::cout  
        << "=========================================================\n"
        << " SIMULADOR DE COLA FIFO EN C++17\n"
        << "=========================================================\n\n";

    // Cola de solicitudes de atencion.
    ColaCircular<Solicitud> colaSolicitudes(3);

    // Llegada de solicitudes.
    colaSolicitudes.enqueue({
        101, "Ana", "Restablecer contrasena"
    });

    colaSolicitudes.enqueue({
        102, "Carlos", "Error de conexion"
    });

    colaSolicitudes.enqueue({
        103, "Maria", "Actualizar informacion"
    });

    std::cout << "Solicitudes iniciales:\n";
    colaSolicitudes.mostrar();

    // Se atiende la primera solicitud.
    std::cout << "\nAtendiendo la primera solicitud:\n";
    std::cout << colaSolicitudes.dequeue() << '\n';

    // Se agregan nuevas solicitudes.
    colaSolicitudes.enqueue({
        104, "Luis", "Problema de facturacion"
    });

    colaSolicitudes.enqueue({
        105, "Sofia", "Consulta de servicio"
    });

    std::cout << "\nEstado despues de nuevas llegadas:\n";
    colaSolicitudes.mostrar();

    // Se procesan las solicitudes restantes.

    std::cout << "\nProcesamiento FIFO: \n";

    while (!colaSolicitudes.empty()) {
        Solicitud actual = colaSolicitudes.dequeue();
        std::cout << "Procesando: "
                << actual << '\n';
    }

    std::cout << "\nCola vacia: "
            << std::boolalpha
            << colaSolicitudes.empty()
            << '\n';

    // Ejecuta las pruebas.
    ejecutarPruebas();

    return 0;
}
