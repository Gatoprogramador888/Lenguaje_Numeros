#pragma once
#include"Informacion.h"

class AnalizadorLexico {
private:
	//variables
	string texto;
	string textoSinEspacio;

private:
	//funciones
	void Limpiar();
	//bool CaracterRaro();
	bool ABECEDARIO(int i);
	bool NUMEROS(int i);
	bool CARACTERES(int i);
	bool OPERADORES(int i);

public:
	size_t linea = {};
	//funciones
	void SetTexto(string txt);
	void CaracterRaro();
	bool Proceso();
	string GetText();
	void LimpiarVariables();
};