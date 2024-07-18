#pragma once
#include<iostream>
#include<Windows.h>
#include<conio.h>
using namespace std;

class AnalizadorLexico {
private:
	//variables
	string texto;
	string textoSinEspacio;
	
private:
	//funciones
	void Limpiar();
	bool CaracterRaro();
	bool ABECEDARIO(int i);
	bool NUMEROS(int i);
	bool CARACTERES(int i);
	bool OPERADORES(int i);

public:
	//funciones
	void SetTexto(string txt);
	bool Proceso();
	string GetText();
	void LimpiarVariables();
};