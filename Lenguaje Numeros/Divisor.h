#pragma once

#include"Tokens.h"


class Divisor {
private:
	string linea_comando;
	vector<string> comandos;
	vector<size_t> posicion;
	void Divisor_Caracteres();
	bool  Caracteres(size_t i), Operadores(size_t i);
public:
	size_t linea;
	void Inicio(string _linea)
	{
		linea_comando = _linea;
		Divisor_Caracteres();
	}
	void Limpiar()
	{
		linea_comando = "";
		comandos.clear();
		posicion.clear();
	}
	vector<string> Get_Comandos();
	vector<string> Divisiones_Varias_lineas_Comandos();
	vector<Informacion> Info();
};