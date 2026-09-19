#include "CRObjetos.h"

#include <cassert>
#include <stdexcept>
#include <string>

// ── Objeto ───────────────────────────────────────────────────────────────────


InfDinamico Objeto::ObtenerComoDinamico() const
{
    return std::visit([](const auto& v) -> InfDinamico {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, InfDinamico>) {
            return v; // Coincidencia exacta, costo cero
        }
        else {
            // Conversión nativa entre tipos de Boost en tiempo de compilación
            return InfDinamico(v);
        }
        }, valor);
}

Tipos Objeto::GetType()   const { return tipo; }


// ── Administrador ─────────────────────────────────────────────────────────────
size_t TablaSimbolos::Registrar(const std::string& nombre, Tipos type, bool es_constante)
{
    size_t id;
    if (!free_list.empty()) {
        id = free_list.back();   // reutiliza ID liberado
        free_list.pop_back();
    }
    else {
        id = proximo_id++;       // ID nuevo
    }
    tabla[nombre] = Contenido{
    .id = id,
    .type = type,
    .es_constante = es_constante
    };
    return id;
}

size_t TablaSimbolos::RegistrarLocal(const std::string& nombre, Tipos type, bool es_constante)
{
    size_t id;
    if (!free_list.empty()) {
        id = free_list.back();   // reutiliza ID liberado
        free_list.pop_back();
    }
    else {
        id = proximo_id++;       // ID nuevo
    }
    tabla_local[nombre] = Contenido{
    .id = id,
    .type = type,
    .es_constante = es_constante
    };
    return id;
}

void TablaSimbolos::LimpiarLocales()
{
    for (const auto& [nombre, contenido] : tabla_local)
    {
        free_list.push_back(contenido.id); // Reutiliza el ID para futuras operaciones
    }
    tabla_local.clear();
}

void TablaSimbolos::EliminarDeTabla(size_t id_variable)
{
    //Buscar el objeto por si id
    auto it = std::find_if(tabla.begin(), tabla.end(), [id_variable](const auto& pair) {
        return pair.second.id == id_variable;
        });

    it->second.es_nulo = true;
}

bool TablaSimbolos::Es_Constante(std::string nombre)
{
    if (tabla.contains(nombre)) return tabla.at(nombre).es_constante;
    if (tabla_local.contains(nombre)) return tabla_local.at(nombre).es_constante;
    return false;
}

bool TablaSimbolos::Es_Nulo(std::string nombre)
{
    if (tabla.contains(nombre)) return tabla.at(nombre).es_nulo;
    if (tabla_local.contains(nombre)) return tabla_local.at(nombre).es_nulo;
    return false;
}

size_t TablaSimbolos::BuscarId(const std::string& nombre) const
{
    if (tabla.contains(nombre)) return tabla.at(nombre).id;
    if (tabla_local.contains(nombre)) return tabla_local.at(nombre).id;
    return SIZE_MAX;
}

Tipos TablaSimbolos::BuscarTipo(const std::string& nombre) const
{
    if (tabla.contains(nombre)) return tabla.at(nombre).type;
    if (tabla_local.contains(nombre)) return tabla_local.at(nombre).type;
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


// ── Garbage Collector ─────────────────────────────────────────────────────────────

void GC::Alojar(size_t id, Informacion_Variable iv)
{
    if(pool.empty())
    {
        pool.resize(50, nullptr);
    }
    else if(id >= pool.size())
    {
        pool.resize(id * 2, nullptr);
    }
    ValorNumerico valor;
    switch (iv.tipo) {
    case Tipos::ENTERO:   valor = InfInt(iv.valor);      break;
    case Tipos::DECIMAL:  valor = InfDec(iv.valor);      break;
    case Tipos::DINAMICO: valor = InfDinamico(iv.valor); break;
    }
    pool[id] = new Objeto(valor, iv.tipo, id);
}

void GC::Liberar(size_t id)
{
	pool[id] = nullptr; // Evitar dangling pointer]
	//Se reutiliza el id liberado para futuros objetos
}

void GC::Actualizar(size_t id, InfDinamico valor)
{
    switch (pool[id]->GetType()) {
    case Tipos::ENTERO:   pool[id]->SetValor(InfInt(valor.operator InfInt()));      break;
    case Tipos::DECIMAL:  pool[id]->SetValor(InfDec(valor.operator InfDec()));      break;
    case Tipos::DINAMICO: pool[id]->SetValor(valor); break;
    }
}

Objeto* GC::Obtener(size_t id)
{
    return pool[id];
}



GC::~GC()
{
	//Limpiar la memoria de todos los objetos alojados
	for (size_t i = 0; i < pool.size(); ++i)
	{
        delete pool[i];
		pool[i] = nullptr; // Evitar dangling pointer
	}
	pool.clear();
}


// ── Global singletons ─────────────────────────────────────────────────────────
GC administrador;