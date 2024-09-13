#include "Interpretar.h"
//Vincular el lib de matematicas prueba

void Interpretar::SetText(string texto)
{
	this->Texto = texto;
	Divisor();
}

char* Interpretar::Sumar(vector<int> n1, vector<int> n2)
{
	Suma suma(n1, n2);
	return suma.Resultado();
}

char* Interpretar::Restar(vector<int> n1, vector<int> n2)
{
	Resta resta(n1,n2);
	return resta.Resultado();
}

char* Interpretar::Multiplicar(vector<int> n1, vector<int> n2)
{
	Multiplicacion multiplicacion(n1, n2);
	return multiplicacion.Resultado();
}

char* Interpretar::Dividir(vector<int> n1, vector<int> num2)
{
	Division division(n1,num2);
	return division.Resultado();
}

void Interpretar::Calcular(size_t& cantidad)
{
	if (Texto != "%%")
	{

		if (segunda_variable != "")
		{
			if (operador == "+")
			{
				var1 = conversion.STOII(resultado.c_str());
				var2 = conversion.STOII(segunda_variable.c_str());
				resultado = Sumar(var1, var2);
			}
			else if (operador == "-")
			{
				var1 = conversion.STOII(resultado.c_str());
				var2 = conversion.STOII(segunda_variable.c_str());
				resultado = Restar(var1, var2);
			}
			else if (operador == "*")
			{
				var1 = conversion.STOIM(resultado.c_str());
				var2 = conversion.STOIM(segunda_variable.c_str());
				resultado = Multiplicar(var1, var2);
			}
			else if (operador == "/")
			{
				var1 = conversion.STOII(resultado.c_str());
				var2 = conversion.STOII(segunda_variable.c_str());
				resultado = Dividir(var1, var2);
			}
			segunda_variable = "";
		}

		if (cantidad == 0)
		{
			Variable = Texto;
		}
		else if(isalnum(Texto[0]))
		{

			if(resultado != "")
				segunda_variable = Texto;
			else
				resultado = Texto;

			if (obj[administrador.PosOBj(Variable)]->GetType() != "ENTERO" && segunda_variable != "")
			{
				strcpy(var_conver1, resultado.c_str());
				strcpy(var_conver2, segunda_variable.c_str());
				conversion.ajustarPrecision(var_conver1);
				conversion.ajustarPrecision(var_conver2);
				resultado = var_conver1;
				segunda_variable = var_conver2;
			}
		}
		else if (isalpha(Texto[0]))
		{

			if (resultado != "")
			{
				segunda_variable = obj[administrador.PosOBj(Texto)]->GetValor();
			}
			else
			{
				resultado = obj[administrador.PosOBj(Texto)]->GetValor();
			}

			if (obj[administrador.PosOBj(Variable)]->GetType() != "ENTERO" && segunda_variable != "")
			{
				strcpy(var_conver1, resultado.c_str());
				strcpy(var_conver2, segunda_variable.c_str());
				conversion.ajustarPrecision(var_conver1);
				conversion.ajustarPrecision(var_conver2);
				resultado = var_conver1;
				segunda_variable = var_conver2;
			}

		}
		else
		{
			operador = Texto;
		}

		cantidad++;
	}
	else
	{
		cantidad = 0;
		estado = Estados::NINGUNO;
	}
	administrador.NuevaIgualdad(Variable, resultado);
}

void Interpretar::Imprimir()
{
	if (Texto != ">>")
	{
		cout << Texto;
	}
	else
	{
		cout << endl;
		estado = Estados::NINGUNO;
	}
}

void Interpretar::Divisor()
{
	if (Texto == ">")
	{
		estado = Estados::IMPRIMIR;
	}
	else if (Texto == "<")
	{
		estado = Estados::PEDIR;
	}
	else if (Texto == "%")
	{
		estado = Estados::OPERACION;
	}


	switch (estado)
	{
		case Estados::IMPRIMIR:
			Imprimir();
			break;
		case Estados::PEDIR:
			Peticion();
			break;
		case Estados::OPERACION:
			Calcular(cantidad);
			break;
	}
}

void Interpretar::Peticion()
{
	if (Texto != "<<")
	{
		Analizador_Semantico_Interpretacion ASI;
		string nuevaigualdad;
		cin >> nuevaigualdad;
		ASI.Verificar_Peticion(nuevaigualdad);
		administrador.NuevaIgualdad(Texto, nuevaigualdad);
	}
	else
	{
		estado = Estados::NINGUNO;
	}
}


void Interpretar::EncontraIgualdad() {
	string variable = "";
	size_t pos = 0;

	for (int i = 1; i < Texto.length(); i++)
	{
		variable += Texto[i];
	}

	pos = administrador.PosOBj(variable);
	Texto = obj[pos]->GetValor();
}

