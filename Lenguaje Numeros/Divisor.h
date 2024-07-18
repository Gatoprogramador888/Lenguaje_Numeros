#pragma once
#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<Windows.h>
#include"CRObjetos.h"
using namespace std;


class Divisor {
private:
	//variables
	string texto;
	string variable, igualdad;
	vector<string> variables;
	vector<char> operaciones;
	int Tipo = 0;
private:
	//funciones
	void Variable(), Operaciones(), Impresion();

public:
	void SetText_Tipo(string txt,int t);
	void Obtener(string& Variable,string& Impresione,string& Igualdad,int& t);
	vector<string> VARAOPE();
	vector<char> OPERADORES();
	bool Dividir();
	void Limpiar();
};