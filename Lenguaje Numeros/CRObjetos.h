#pragma once
#include "Informacion.h"

#include <cstddef>   // SIZE_MAX
#include <string>
#include <vector>
#include <algorithm>
#include <variant>
#include "InfDec.h"
#include "InfInt.h"
#include "InfDinamico.h"
#include <sstream>

// ── Objeto ───────────────────────────────────────────────────────────────────
using ValorNumerico = std::variant<InfInt, InfDec, InfDinamico>;

class Objeto {
    ValorNumerico valor;
    Tipos tipo;
    size_t id_variable{ 0 };
public:
    Objeto(ValorNumerico _v, Tipos _tipo, size_t _id_variable)
        : valor(_v)
        , tipo(_tipo)
        , id_variable(_id_variable)
    {}

    // Non-copyable: these objects are always owned through a raw pointer in
    // `obj`; accidental copies would cause double-free.
    Objeto(const Objeto&) = delete;
    Objeto& operator=(const Objeto&) = delete;

    void SetValor(InfInt    v) { valor = std::move(v); }
    void SetValor(InfDec    v) { valor = std::move(v); }
    void SetValor(InfDinamico v) { valor = std::move(v); }

    size_t   GetIdVariable() const { return id_variable; }
    template<typename T>
    T& GetValor() { return std::get<T>(valor); }
    InfDinamico ObtenerComoDinamico() const;

    Tipos GetType()   const;


    std::string GetValorStr() const
    {
        return std::visit([](const auto& v) -> std::string {
            using T = std::decay_t<decltype(v)>;

            // Manejo personalizado explícito para InfDinamico
            if constexpr (std::is_same_v<T, InfDinamico>) {
                // Aquí controlas exactamente cómo formatearlo
                if (v.esDecimal()) {
                    return v.operator InfDec().str();
                }
                else {
                    return v.operator InfInt().str();
                }
            }
            else if constexpr (std::is_same_v<T, InfDec> || std::is_same_v<T, InfInt>) {
                return v.str();
            }
            }, valor);
    }

    ~Objeto() = default;
};


// ── Garbage Collector ─────────────────────────────────────────────────────────────
class GC
{
private:
    std::vector<Objeto*> pool;
public:
    void    Alojar(size_t id, Informacion_Variable iv);
    void    Liberar(size_t id);
    void    Actualizar(size_t id, InfDinamico valor);
    Objeto* Obtener(size_t id);
    ~GC();
};

// ── Tabla de Símbolos ─────────────────────────────────────────────────────────────
struct TablaSimbolos {
    struct Contenido
    {
        size_t id{};
        Tipos type{};
        bool es_constante{ false }, es_nulo{ false };
        //size_t offset_ultimo_nop{ SIZE_MAX };
    };
    std::map<std::string, Contenido> tabla;
    std::map<std::string, Contenido> tabla_local;
	std::vector<size_t> free_list;  // IDs liberados para reutilización
    size_t proximo_id{ 0 };

    // Registra y devuelve el ID asignado
    size_t Registrar(const std::string& nombre, Tipos type, bool es_constante); 

    size_t RegistrarLocal(const std::string& nombre, Tipos type, bool es_constante);

    void EliminarDeTabla(size_t id_variable);

    bool Es_Constante(std::string nombre);

    bool Es_Nulo(std::string nombre); 

    // Devuelve SIZE_MAX si no existe
    size_t BuscarId(const std::string& nombre) const; 

    Tipos BuscarTipo(const std::string& nombre) const; 

    Tipos BuscarTipo(size_t id_variable) const; 

    bool Existe(const std::string& nombre) const {
        return tabla_local.count(nombre) > 0 || tabla.count(nombre) > 0;
    }

    void Limpiar() { tabla.clear(); proximo_id = 0; }
};

// ── Globals ───────────────────────────────────────────────────────────────────
extern GC                    administrador;