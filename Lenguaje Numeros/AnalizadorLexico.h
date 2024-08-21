#pragma once
#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<stdexcept>
#include<string>
using namespace std;

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
	int linea = {};
	//funciones
	void SetTexto(string txt);
	void CaracterRaro();
	bool Proceso();
	string GetText();
	void LimpiarVariables();
};