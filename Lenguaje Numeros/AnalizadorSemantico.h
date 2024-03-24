#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"CRObjetos.h"
#include<Windows.h>

using namespace std;

class AnalizadorSemantico {
private:
	string variable, igualdad,Impresion;
	vector<string>Variables, Operadores;
	int tipo;
private:
	bool Variable(), Operador(), Imprimir(),Igual();
public:
	string _variable, _igualdad, _impresion;
	vector<string>_variables, _operadores;
	int _tipo;
	void Inicializar();
	bool Dividir();
	void Limpiar();
};