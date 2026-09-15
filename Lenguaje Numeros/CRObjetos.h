#pragma once
#include "Informacion.h"

#include <cstddef>   // SIZE_MAX
#include <string>
#include <vector>
#include <algorithm>

// ── Objeto ───────────────────────────────────────────────────────────────────

class Objeto {
    std::string valor;
    std::string nombre;
    Tipos tipo;
	size_t id_variable{ 0 };
public:
    Objeto(std::string _valor, std::string _nombre, Tipos _tipo, size_t _id_variable)
        : valor(std::move(_valor))
        , nombre(std::move(_nombre))
        , tipo(_tipo)
        , id_variable(_id_variable)
    {}

    // Non-copyable: these objects are always owned through a raw pointer in
    // `obj`; accidental copies would cause double-free.
    Objeto(const Objeto&) = delete;
    Objeto& operator=(const Objeto&) = delete;

    void        SetObjeto(std::string_view _valor);

	size_t      GetIdVariable() const { return id_variable; }
    std::string GetNombre() const;
    std::string GetValor()  const;
    Tipos GetType()   const;

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
private:
	mutable size_t id_variable{ 0 };

private:
    bool Iguales(std::string_view _nombre) const;
public:
    void   Crear(Informacion_Variable informacion_variable);
    [[deprecated("Ahora es con el id de la variable no con el nombre.")]]
    void   NuevaIgualdad(std::string_view _nombre, std::string_view _valor);

    void   NuevaIgualdad(size_t _id_variable, std::string_view _valor);

    /// Returns SIZE_MAX when the name is not found.
    /// !! Callers MUST check the return value before using it as an index !!!
    size_t PosOBj(std::string_view _nombre) const;

    /// Deletes the object and erases its slot.  Returns false if not found.
    bool   Borrar_Objeto(size_t _id_variable);

    size_t GetIdVariable() const { return id_variable++; }
};



class GC
{

};

struct TablaSimbolos {
    struct Contenido
    {
        size_t id;
        Tipos type;
        //size_t offset_ultimo_nop{ SIZE_MAX };
    };
    std::map<std::string, Contenido> tabla;
    std::map<std::string, Contenido> tabla_local;
	std::vector<size_t> free_list;  // IDs liberados para reutilización
    size_t proximo_id{ 0 };

    // Registra y devuelve el ID asignado
    size_t Registrar(const std::string& nombre, Tipos type); 

    size_t RegistrarLocal(const std::string& nombre, Tipos type); 

    void EliminarDeTabla(size_t id_variable);

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
extern std::vector<Objeto*> obj;
extern BorrarOBJ             BOBJ;
extern Administrador         administrador;