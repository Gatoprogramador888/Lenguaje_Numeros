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
    if (Iguales(informacion_variable.nombre))
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