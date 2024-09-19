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
		string result = "";
		switch (Texto[0])
		{
		case '+':
		case '-':
		case '*':
		case '/':
			operador = Texto;
			break;
		default:
			if (isalnum(Texto[0]) && resultado != "" && cantidad > 2)
			{
				segunda_variable = Texto;
				if (resultado[0] != '0')
				{
					string aux = "0" + resultado;
					resultado = aux;
				}
			}
			else if (isalnum(Texto[0]) && cantidad == 2)
			{
				resultado = Texto;
			}
			else if (isalnum(Texto[0]) && cantidad == 1)
			{
				Variable = Texto;
			}
			else if(Texto != "" && cantidad > 2)
			{
				segunda_variable = obj[administrador.PosOBj(Texto)]->GetValor();
			}
			else if (Texto != "" && cantidad == 2)
			{
				resultado = obj[administrador.PosOBj(Texto)]->GetValor();
			}
			break;
		}

		if (segunda_variable != "")
		{
			switch (operador[0])
			{
			case '+':
				var1 = conversion.STOII(resultado.c_str());
				var2 = conversion.STOII(segunda_variable.c_str());
				var_conver1 =  Sumar(var1, var2);
				break;
			case '-':
				var1 = conversion.STOII(resultado.c_str());
				var2 = conversion.STOII(segunda_variable.c_str());
				var_conver1 = Restar(var1, var2);
				break;
			case '*':
				var1 = conversion.STOIM(resultado.c_str());
				var2 = conversion.STOIM(segunda_variable.c_str());
				var_conver1 = Multiplicar(var1, var2);
				break;
			case '/':
				resultado += "0";
				segunda_variable += "0";
				var1 = conversion.STOII(resultado.c_str());
				var2 = conversion.STOII(segunda_variable.c_str());
				var_conver1 = Dividir(var1, var2);
				break;
			}
			segunda_variable = "";
			resultado = var_conver1;
			operador = "";
			var_conver1[0] = '\0';
		}
		cantidad++;
		if (resultado[0] != '0' || (strlen(resultado.c_str()) < 2 && resultado == "0"))
			result = "0" + resultado;
		else
			result = resultado;

		administrador.NuevaIgualdad(Variable, result);
	}
}

void Interpretar::Imprimir()
{
	if (Texto != ">>")
	{
		if (Texto.find("{") != string::npos)
		{
			string variable = "";
			for (size_t i = 2; i < Texto.length(); i++)
			{
				variable += Texto[i];
			}
			cout  << obj[administrador.PosOBj(variable)]->GetValor();
		}
		else
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
		return;
	}
	else if (Texto == "<")
	{
		estado = Estados::PEDIR;
		return;
	}
	else if (Texto == "%")
	{
		estado = Estados::OPERACION;
		return;
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


Interpretar::~Interpretar()
{
	cantidad = 0;

}