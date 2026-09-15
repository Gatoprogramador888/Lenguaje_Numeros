#include "CRObjetos.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include <algorithm>

vector<Objeto*> obj;

// ── BorrarOBJ ────────────────────────────────────────────────────────────────

void BorrarOBJ::Borrar()
{
    for (auto* i : obj)
        delete i;           // properly invokes destructor AND frees memory
    obj.clear();            // leave the vector in a valid, empty state
}

// ── Objeto ───────────────────────────────────────────────────────────────────

void Objeto::SetObjeto(std::string_view _valor)
{
    valor = _valor;
}

std::string Objeto::GetNombre() const { return nombre; }
std::string Objeto::GetValor()  const { return valor; }
Tipos Objeto::GetType()   const { return tipo; }

// ── Administrador ─────────────────────────────────────────────────────────────

bool Administrador::Iguales(std::string_view _nombre) const
{
    size_t i = PosOBj(_nombre);
    return i != SIZE_MAX;
}

void Administrador::Crear(Informacion_Variable informacion_variable)
{
    if (Iguales(informacion_variable.nombre) &&  informacion_variable.nombre != "")
        NuevaIgualdad(informacion_variable.nombre, informacion_variable.valor);
    else
        obj.push_back(new Objeto(informacion_variable.valor,
            informacion_variable.nombre,
            informacion_variable.tipo,
            informacion_variable.id));
}

void Administrador::NuevaIgualdad(std::string_view _nombre,
    std::string_view _valor)
{
    size_t i = PosOBj(_nombre);
    obj[i]->SetObjeto(_valor);
}

void Administrador::NuevaIgualdad(size_t _id_variable, std::string_view _valor)
{
	obj[_id_variable]->SetObjeto(_valor);
}

size_t Administrador::PosOBj(std::string_view _nombre) const
{
    auto it = std::find_if(obj.begin(), obj.end(),
        [&_nombre](const Objeto* o) { return o->GetNombre() == _nombre; });
    if (it == obj.end())return SIZE_MAX;
    size_t pos_t = std::distance(obj.begin(), it);
    return pos_t;
}

// Returns true and removes the object; returns false if not found.
bool Administrador::Borrar_Objeto(size_t _id_variable)
{
	if (_id_variable < obj.size())
	{
		delete obj[_id_variable];                          // ① proper destruction + free
		obj.erase(obj.begin() + static_cast<std::ptrdiff_t>(_id_variable)); // ② remove slot
		return true;
	}
    return false;
}

// ── Global singletons ─────────────────────────────────────────────────────────
BorrarOBJ    BOBJ;
Administrador administrador;

size_t TablaSimbolos::Registrar(const std::string& nombre, Tipos type)
{
    size_t id;
    if (!free_list.empty()) {
        id = free_list.back();   // reutiliza ID liberado
        free_list.pop_back();
    }
    else {
        id = proximo_id++;       // ID nuevo
    }
    tabla[nombre] = { id, type };
    return id;
}

size_t TablaSimbolos::RegistrarLocal(const std::string& nombre, Tipos type)
{
    size_t id;
    if (!free_list.empty()) {
        id = free_list.back();   // reutiliza ID liberado
        free_list.pop_back();
    }
    else {
        id = proximo_id++;       // ID nuevo
    }
    tabla_local[nombre] = { id, type };
    return id;
}

void TablaSimbolos::EliminarDeTabla(size_t id_variable)
{
    std::erase_if(tabla, [id_variable](const auto& pair) {
        return pair.second.id == id_variable;
        });
}

size_t TablaSimbolos::BuscarId(const std::string& nombre) const
{
    // 1. Buscar primero en el ámbito/tabla local
    auto it_local = tabla_local.find(nombre);
    if (it_local != tabla_local.end()) {
        return it_local->second.id;
    }

    // 2. Si no está en la local, buscar en la tabla global
    auto it_global = tabla.find(nombre);
    if (it_global != tabla.end()) {
        return it_global->second.id;
    }

    // 3. No existe en ningún scope
    return SIZE_MAX;
}

Tipos TablaSimbolos::BuscarTipo(const std::string& nombre) const
{
    // 1. Buscar en el ámbito local
    auto it_local = tabla_local.find(nombre);
    if (it_local != tabla_local.end()) {
        return it_local->second.type;
    }

    // 2. Si no existe en local, buscar en el ámbito global
    auto it_global = tabla.find(nombre);
    if (it_global != tabla.end()) {
        return it_global->second.type;
    }

    // 3. Si no existe en ningún ámbito, retornar DINAMICO por defecto
    return Tipos::DINAMICO;
}

Tipos TablaSimbolos::BuscarTipo(size_t id_variable) const
{
    for (const auto& [nombre, contenido] : tabla) {
        if (contenido.id == id_variable) {
            return contenido.type;
        }
    }
    for (const auto& [nombre, contenido] : tabla_local) {
        if (contenido.id == id_variable) {
            return contenido.type;
        }
    }
    return Tipos::DINAMICO; // Valor por defecto si no se encuentra
}


