#pragma once

#include"Tokens.h"


class Divisor : public Interfaz_Compilador{
private:
	string linea_comando;
	vector<string> comandos;
	vector<size_t> posicion;
	void Divisor_Caracteres();
	bool  Caracteres(size_t i), Operadores(size_t i);
public:

	void Inicio(string _linea)
	{
		linea_comando = _linea;
		Divisor_Caracteres();
	}

	void Limpiar() override 
	{
		linea_comando.clear();
		comandos.clear();
		posicion.clear();
		linea = 0;
	}

	vector<string> Get_Comandos();

	vector<string> Divisiones_Varias_lineas_Comandos();

	vector<Informacion> Info();

	~Divisor()
	{
		Limpiar();
		comandos.shrink_to_fit();
		posicion.shrink_to_fit();
	}
};