#pragma once
//Vincular el lib de matematicas prueba
#include"../Operaciones de numeros infinitos static/Dividir.h"
#include"../Operaciones de numeros infinitos static/Multiplicar.h"
#include"../Operaciones de numeros infinitos static/Restar.h"
#include"../Operaciones de numeros infinitos static/Sumar.h"
#include"AnalizadorSemantico.h"


class Interpretar
{
private:
	char* Sumar(vector<int> n1, vector<int> n2);
	char* Restar(vector<int> n1, vector<int> n2);
	char* Multiplicar(vector<int> n1, vector<int> n2);
	char* Dividir(vector<int> n1, vector<int> n2);
	void Calcular(size_t& cantidad), Imprimir(), Divisor(), Peticion();

private:
	//Para Calcular
	string Variable = "", segunda_variable = "", operador = "", resultado = "";
	char* var_conver1;
	char* var_conver2;
	listnum var1, var2;
	ConversionI conversion;

	string Texto;
	enum class Estados { OPERACION, IMPRIMIR, PEDIR, NINGUNO };
	Estados estado = Estados::NINGUNO;
	size_t cantidad = 0;
	bool accion = false;

public:
	void SetText(string texto);
};

