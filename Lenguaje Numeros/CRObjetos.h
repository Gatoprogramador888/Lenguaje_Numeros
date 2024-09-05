#pragma once
#include"Informacion.h"
using namespace std;

class Objeto {
private:
	string valor = "";
	string nombre = "";
	string tipo = "";
public:
	Objeto(string _valor, string _nombre, string _tipo) : valor(_valor), nombre(_nombre), tipo(_tipo) {}
	void SetObjeto(string _valor);
	string GetNombre();
	string GetValor();
	string GetType();
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
	void Crear(Informacion_Variable informacion_variable);
	void NuevaIgualdad(string _nombre, string _valor);
	size_t PosOBj(string _nombre);
	bool Borrar_Objeto(string nombre);
};

extern vector<Objeto*> obj;
extern BorrarOBJ BOBJ;
extern Administrador administrador;