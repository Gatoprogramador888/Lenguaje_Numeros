#include "Tokens.h"


bool Tokenizador::Variable(string palabra)
{
	string TIPO_VARIABLE[] = { "Entero", "Decimal", "Dinamico" };
	for (string Tipo_Variable : TIPO_VARIABLE)
	{
		if (palabra == Tipo_Variable)
		{
			if (palabra == TIPO_VARIABLE[0])token = Tokens::ENTERO;
			else if (palabra == TIPO_VARIABLE[1])token = Tokens::DECIMAL;
			else token = Tokens::DINAMICO;
			return true;
		}
	}
	return false;
}

bool Tokenizador::Impresion_Peticion(string palabra)
{
	string TIPO_PETICION[] = { "Pedir", "Imprimir" };
	for (string Tipo_Peticion : TIPO_PETICION)
	{
		if (palabra == Tipo_Peticion) { 
			if (palabra != TIPO_PETICION[0])token = Tokens::IMPRIMIR;
			else token = Tokens::PEDIR;
			return true;
		}
	}

	return false;
}

bool Tokenizador::Caracter(string palabra)
{
	return (palabra == "." || palabra == "_");
}

void Tokenizador::Recopilar_informacion(Informacion info,Tokens token)
{
	Informacion aux = info;
	aux.token = token;
	informacion.push_back(aux);
}

map<string,Informacion> Tokenizador::Mapa_Informacion(vector<string> instruccion, vector<Informacion> info)
{
	size_t comillas = 0;
	bool parentesis = false;
	size_t i = 0;
	map<string, Informacion> retorno;

	for (string palabra : instruccion)
	{

		if (comillas % 2 != 1)
		{
			if (Variable(palabra))Recopilar_informacion(info[i], token);

			else if (Impresion_Peticion(palabra))Recopilar_informacion(info[i], token);

			else if (palabra == "Operador")Recopilar_informacion(info[i], Tokens::OPERACION);

			else if (palabra == "#")break;

			else if (palabra[0] >= '0' && palabra[0] <= '9')Recopilar_informacion(info[i], Tokens::NUMERO);

			else if (palabra == ";")Recopilar_informacion(info[i], Tokens::FIN_COMANDO);

			else if (palabra == ",")Recopilar_informacion(info[i], Tokens::COMAS);

			else if (palabra == "+" || palabra == "-" || palabra == "*" || palabra == "/")Recopilar_informacion(info[i], Tokens::OPERADOR);

			else if (palabra == "=")Recopilar_informacion(info[i], Tokens::IGUAL);

			else if (palabra == ":")Recopilar_informacion(info[i], Tokens::DIVISOR);

			else if (palabra == "\"")
			{
				Recopilar_informacion(info[i], Tokens::COMILLAS);
				comillas++;
			}

			else if (palabra == "(")Recopilar_informacion(info[i], Tokens::PARENTESIS_IZQUIERDO);

			else if (palabra == ")")Recopilar_informacion(info[i], Tokens::PARENTESIS_DERECHO);

			else if (isalpha(palabra[0]))Recopilar_informacion(info[i], Tokens::VARIABLE);

			else if (palabra == "$")Recopilar_informacion(info[i], Tokens::TEXTO);

			else if (palabra[0] == NULL) {}

			else Recopilar_informacion(info[i], Tokens::CARACTER);

		}
		else
		{
			if (palabra == "(")
			{
				Recopilar_informacion(info[i], Tokens::PARENTESIS_IZQUIERDO);
				parentesis = true;
			}
			else if (palabra == ")")
			{
				Recopilar_informacion(info[i], Tokens::PARENTESIS_DERECHO);
				parentesis = false;
			}
			else if (palabra == "\"")
			{
				Recopilar_informacion(info[i], Tokens::COMILLAS);
				comillas++;
			}
			else if (parentesis)Recopilar_informacion(info[i], Tokens::VARIABLE);

			else Recopilar_informacion(info[i], Tokens::CARACTER);
		}
		i++;
	}
	
	size_t posicion_Token = 1;

	for (auto it : informacion)
	{
		string ID = to_string(linea) + "." + to_string(posicion_Token);
		retorno.insert(make_pair(ID, it));
		posicion_Token++;
	}

	return retorno;
}



string Tokenizador::Get_Tipo(Tokens token)
{
	string retorno[20] = { "NULO" , "ENTERO", "DECIMAL", "DINAMICO", "NUMERO",
	"IMPRIMIR", "PEDIR", "OPERADOR", "OPERACION",
	"COMAS", "FIN_COMANDO",	"ESPACIO", "COMILLAS",
	"PARENTESIS_DERECHO", "PARENTESIS_IZQUIERDO",
	"VARIABLE", "IGUAL", "CARACTER", "DIVISOR", "TEXTO"};
	return retorno[(int)token];
}