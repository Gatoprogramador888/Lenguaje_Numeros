#pragma once
#include "Informacion.h"

#include <cstddef>   // SIZE_MAX
#include <string>
#include <vector>

// ── Objeto ───────────────────────────────────────────────────────────────────

class Objeto {
    std::string valor;
    std::string nombre;
    std::string tipo;
public:
    Objeto(std::string _valor, std::string _nombre, std::string _tipo)
        : valor(std::move(_valor))
        , nombre(std::move(_nombre))
        , tipo(std::move(_tipo))
    {}

    // Non-copyable: these objects are always owned through a raw pointer in
    // `obj`; accidental copies would cause double-free.
    Objeto(const Objeto&) = delete;
    Objeto& operator=(const Objeto&) = delete;

    void        SetObjeto(std::string _valor);

    std::string GetNombre() const;
    std::string GetValor()  const;
    std::string GetType()   const;

    ~Objeto() = default;
};

// ── BorrarOBJ ────────────────────────────────────────────────────────────────

class BorrarOBJ {
public:
    /// delete-s every Objeto* in obj and clears the vector.
    void Borrar();
};

// ── Administrador ─────────────────────────────────────────────────────────────

class Administrador {
    bool Iguales(const std::string& _nombre) const;
public:
    void   Crear(Informacion_Variable informacion_variable);
    void   NuevaIgualdad(const std::string& _nombre, const std::string& _valor);

    /// Returns SIZE_MAX when the name is not found.
    /// !! Callers MUST check the return value before using it as an index !!
    size_t PosOBj(const std::string& _nombre) const;

    /// Deletes the object and erases its slot.  Returns false if not found.
    bool   Borrar_Objeto(const std::string& nombre);
};

// ── Globals ───────────────────────────────────────────────────────────────────
extern std::vector<Objeto*> obj;
extern BorrarOBJ             BOBJ;
extern Administrador         administrador;