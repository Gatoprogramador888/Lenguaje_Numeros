#include"Divisor.h"

void Divisor::Obtener(string& Variable, string& Impresione, string& Igualdad, int& t)
{
	Variable = variables;
	Igualdad = igualdades;
	Impresione = texto;
	t = Tipo;
}

vector<string> Divisor::VARAOPE()
{
	return operadoresvariable;
}

vector<string> Divisor::OPERADORES()
{
	return igualdadoperadores;
}

bool Divisor::Dividir()
{
	if (((texto[0] >= 'A' && texto[0] >= 'Z') || (texto[0] >= 'a' && texto[0] >= 'z')))
	{
		DividirVariables();
		Tipo = 1;
		return true;
	}
	if (texto[0] == '0') 
	{
		DividirOperaciones();
		Tipo = 2;
		return true;
	}
	if (texto[0] == '(')
	{
		DividirImpresiones();
		Tipo = 3;
		return true;
	}
	else
	{
		Tipo = 4;
		OutputDebugString("\nInexistente "+texto[0]+'\n');
		return false;
	}
}

void Divisor::Limpiar()
{
	texto = "";
	variables = "";
	igualdades = "";
	igualdadoperadores.clear();
	operadoresvariable.clear();
	posoperadores.clear();
	Tipo = 0;
}

void Divisor::DividirVariables()
{
	OutputDebugString("\nEs variable\n");

	int j = 0;
	for (int i = 0; texto[i] != '=' ; i++)
	{
		if (texto[i] == '\0')break;
		variables += texto[i];
		j++;
	}


	if (texto[j] == '=')
	{
		for (int i = j+1; i != texto.length(); i++)
		{
			igualdades += texto[i];
			
		}
	}
	else
	{
		igualdades = "0";
	}

	char* texto = new char[100];
	string texto2 = "Variable: " + variables + "\nigualdad: " + igualdades.c_str() + '\n';
	strcpy(texto, texto2.c_str());

	OutputDebugString(texto);
}

void Divisor::DividirOperaciones()
{
	OutputDebugString("\nEs Operacion\n");
	int j = 0;
	for (int i = 0; texto[i] != '='; i++)
	{
		if (texto[i] == '\0')break;
		variables += texto[i];
		j++;
	}

	posoperadores.push_back(j);

	if (texto[j] != NULL)
	{
		igualdadoperadores = OperadoresObtener(texto);

		posoperadores.push_back((int)texto.length());

		operadoresvariable = OperadoresVariableObtener(posoperadores);
		
	}
}

void Divisor::DividirImpresiones()
{
	OutputDebugString("\nEs Impresion\n");
}

vector<string> Divisor::OperadoresObtener(string oracion)
{
	vector<string>vectoroperator;
	for (int i = 0; i < oracion.length(); i++)
	{
		switch (oracion[i])
		{
		case '+':
			vectoroperator.push_back("+");
			posoperadores.push_back(i);
			break;
		case '-':
			vectoroperator.push_back("-");
			posoperadores.push_back(i);
			break;
		case '*':
			vectoroperator.push_back("*");
			posoperadores.push_back(i);
			break;
		case '/':
			vectoroperator.push_back("/");
			posoperadores.push_back(i);
			break;
		}
	}
	return vectoroperator;
}

vector<string> Divisor::OperadoresVariableObtener(vector<int> vectorial)
{
	vector<string> str;
	string var;
	for (int i = 0; i < vectorial.size()-1; i++)
	{
			for (int k = posoperadores[i]+1; k < posoperadores[i+1] ; k++)
			{
				var += texto[k];
			}
			const char* v = var.c_str();
			OutputDebugString(v);
			str.push_back(var);
			var = "";
	}
	return str;
}

void Divisor::SetText(string txt)
{
	this->texto = txt;
}



