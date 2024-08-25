#include"CRObjetos.h"

vector<Objeto*> obj;
void BorrarOBJ::Borrar()
{
	for (auto i : obj)
	{
		i->~Objeto();
	}
}

void Objeto::SetObjeto(string _valor)
{
	valor = _valor;
}

string Objeto::GetNombre()
{
	return nombre;
}

string Objeto::GetValor()
{
	return valor;
}

bool Administrador::Iguales(string _nombre)
{
	for (auto i : obj)
	{
		if (_nombre == i->GetNombre()) {
			
			return true;
		}
	}
	return false;
}

void Administrador::Crear(string _nombre, string _valor)
{
	if (Iguales(_nombre))
	{
		NuevaIgualdad(_nombre, _valor);
	}
	else
	{
		obj.push_back(new Objeto(_valor,_nombre));
	}
}

void Administrador::NuevaIgualdad(string _nombre, string _valor)
{
	for (auto i : obj)
	{
		if (_nombre == i->GetNombre())
		{
			i->SetObjeto(_valor);
		}
	}
}

size_t Administrador::PosOBj(string _nombre)
{
	size_t retorno = 0;
	for (auto i : obj)
	{
		if (_nombre == i->GetNombre())return retorno;
		retorno++;
	}
	return SIZE_MAX;
}

bool Administrador::Borrar_Objeto(string nombre)
{
	size_t posicion = PosOBj(nombre);
	bool retorno = posicion != SIZE_MAX ? true : false;

	if (retorno)obj[posicion]->~Objeto();

	return retorno;
}


BorrarOBJ BOBJ;
Administrador administrador;