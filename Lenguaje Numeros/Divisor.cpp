#include"Divisor.h"

void Divisor::SetText_Tipo(string txt, int t)
{
	this->texto = txt;
	this->Tipo = t;
}


bool Divisor::Dividir()
{
	if (Tipo == 1)
	{
		Variable();
	}
	if (Tipo == 2)
	{
		Operaciones();
	}
	if (Tipo == 3)
	{
		Impresion();
	}
	if (Tipo == 4)
	{
		Impresion();
	}

	return Tipo == 5;
}


/**************DIVISOR VARIABLES**********************/

void Divisor::Variable()
{
	OutputDebugString("\nEs variable\n");

	size_t igual = texto.find("=");
	
	if (igual != string::npos) {

		for (int i = 0; i < igual; i++) {
			variable += texto[i];
		}

		for (size_t i = igual + 1; i < texto.length(); i++) {
			igualdad += texto[i];
		}
	}
	else {
		variable = texto;
		igualdad = "";
	}

	//OutputDebugString(texto);
}

/***************FIN VARIABLES************************/




/**************DIVISOR OPERACIONES**********************/

void Divisor::Operaciones()
{
	OutputDebugString("\nEs Operacion\n");
	size_t igual = texto.find("=");
	string var = "";
	
	if (igual != string::npos) {

		for (int i = 1; i < igual; i++) {
			variable += texto[i];
		}

		for (size_t i = igual + 1; i < texto.length(); i++) {
			switch (texto[i])
			{
			case '+':
			case '-':
			case '*':
			case '/':
				variables.push_back(var);
				operaciones.push_back(texto[i]);
				var = "";
				break;
			default:
				var += texto[i];
				break;
			}
		}

		if (var != "")variables.push_back(var);
	}

}

/**************FIN OPERACIONES**********************/



/**************DIVISOR IMPRESIONES**********************/

void Divisor::Impresion()
{
	OutputDebugString("\nEs Impresion\n");
}

/***************FIN OPERACIONES**********************/




void Divisor::Obtener(string& Variable, string& Impresione, string& Igualdad, int& t)
{
	Variable = variable;
	Igualdad = igualdad;
	Impresione = texto;
	t = Tipo;
}


vector<string> Divisor::VARAOPE()
{
	return variables;
}

vector<char> Divisor::OPERADORES()
{
	return operaciones;
}

void Divisor::Limpiar()
{
	texto = "";
	variable = "";
	igualdad = "";
	operaciones.clear();
	variables.clear();
	Tipo = 0;
}
