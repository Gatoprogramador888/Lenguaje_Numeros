#pragma once
#include"Informacion.h"
using namespace std;

class Objeto {
private:
	string valor;
	string nombre;
public:
	Objeto(string _valor, string _nombre) : valor(_valor), nombre(_nombre) {}
	void SetObjeto(string _valor);
	string GetNombre();
	string GetValor();
	~Objeto(){}
};

class BorrarOBJ {
public:
	void Borrar();
};


class Administrador {
private:
	bool Iguales(string _nombre);
public:
	void Crear(string _nombre, string _valor);
	void NuevaIgualdad(string _nombre, string _valor);
	size_t PosOBj(string _nombre);
	bool Borrar_Objeto(string nombre);
};

extern vector<Objeto*> obj;
extern BorrarOBJ BOBJ;
extern Administrador administrador;