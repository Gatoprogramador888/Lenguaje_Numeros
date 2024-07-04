#pragma once
#include<string>
#include<iostream>
#include<vector>
//Vincular el lib de matematicas prueba
#include"../Operaciones de numeros infinitos static/Dividir.h"
#include"../Operaciones de numeros infinitos static/Multiplicar.h"
#include"../Operaciones de numeros infinitos static/Restar.h"
#include"../Operaciones de numeros infinitos static/Sumar.h"

using namespace std;

class Interpretar
{
private:
	char* Sumar(vector<int> n1, vector<int> n2);
	char* Restar(vector<int> n1, vector<int> n2);
	char* Multiplicar(vector<int> n1, vector<int> n2);
	char* Dividir(vector<int> n1, vector<int> n2);
	void Calcular(), Imprimir(), Divisor();

private:

	string Texto;

public:
	void SetText(string texto);
};

