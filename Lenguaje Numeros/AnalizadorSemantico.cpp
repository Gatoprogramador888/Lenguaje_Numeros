#include"AnalizadorSemantico.h"

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens)
{
	if(fin_tokens != Tokens::FIN_COMANDO)
	{ 
		string error = "falta se ; en la Linea: " + to_string(linea) + ", posicion: " + to_string(tokens.size()) + ".\n";
		throw runtime_error(error.c_str());
	}
}

void Analizador_Tokens_Compilacion::Imprimir()
{
	enum class Estados
	{
		INICIO, ESPERA_DIVISOR, ESPERA_COMILLAS,
		ESPERA_PARENTESIS_DERECHO, ESPERA_PARENTESIS_IZQUIERDO, ESPERA_VARIABLE,
		ESPERA_CARACTER, ESPERA_COMILLAS, ESPERA_COMAS_FIN_COMANDO,ERROR
	};
	Estados estado = Estados::INICIO;


	for (size_t posicion = 0; posicion < tokens.size(); posicion++)
	{
		switch (estado)
		{
		case Estados::INICIO:
			if (tokens[posicion] != Tokens::IMPRIMIR)
			{
				error = comandos[posicion] + " no es de tipo Imprimir.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			estado = Estados::ESPERA_DIVISOR;
			break;

		case Estados::ESPERA_DIVISOR:
			if (tokens[posicion] != Tokens::DIVISOR)
			{
				error = "Se esperaba un ':' no un " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}

			estado = tokens[posicion + 1] != Tokens::COMILLAS ? Estados::ESPERA_VARIABLE : Estados::ESPERA_COMILLAS;

			break;

		case Estados::ESPERA_VARIABLE:

			if (tokens[posicion] != Tokens::VARIABLE)
			{
				error = comandos[posicion]+" es de tipo "+Tokenizador::Get_Tipo(tokens[posicion])+" no de tipo Variable.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}

			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}

			estado = Estados::ESPERA_COMAS_FIN_COMANDO;

			break;

		case Estados::ESPERA_COMILLAS:
			break;
		}
	}
}

void Analizador_Tokens_Compilacion::Entero_Decimal_Dinamico()
{

	enum class Estados {
		INICIO, Espera_DIVISOR, Espera_VARIABLE,Espera_IGUAL, Espera_IGUALDAD, Espera_COMA_O_FIN, ERROR
	};

	Tokens Tipo_Dato;
	string nombre_variable = "";
	Estados estado = Estados::INICIO;
	vector<Informacion_Variable> Variables;
	Informacion_Variable variable;

	for (size_t posicion = 2; posicion < tokens.size(); posicion++)
	{
		switch (estado)
		{
		case Estados::INICIO:
			if (tokens[posicion] == Tokens::ENTERO || tokens[posicion] == Tokens::DECIMAL || tokens[posicion] == Tokens::DINAMICO)
			{
				estado = Estados::Espera_DIVISOR;
				Tipo_Dato = tokens[posicion];
			}
			else
			{
				error = "Se esperaba un identificador.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;

		case Estados::Espera_DIVISOR:
			if (tokens[posicion] == Tokens::DIVISOR)
			{
				estado = Estados::Espera_VARIABLE;
			}
			else
			{
				error = "Se esperaba un ':' no un " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
			}
			break;

		case Estados::Espera_VARIABLE:
			if (tokens[posicion] == Tokens::VARIABLE)
			{
				estado = Estados::Espera_IGUAL;
				nombre_variable = comandos[posicion];
				variable.nombre = nombre_variable;
			}
			else 
			{
				error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " no de tipo Variable.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;

		case Estados::Espera_IGUAL:
			if (tokens[posicion] == Tokens::IGUAL)
			{
				estado = Estados::Espera_IGUALDAD;
			}
			else if(tokens[posicion] == Tokens::COMAS || tokens[posicion] == Tokens::FIN_COMANDO)
			{
				error = "La variable " + comandos[posicion - 1] + " no ah sido inicializada.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				cout << error;
				variable.valor = "0";
				estado = Estados::Espera_COMA_O_FIN;
			}
			else {
				error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " no de tipo Igual.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;

		case Estados::Espera_IGUALDAD:
			if (tokens[posicion] == Tokens::NUMERO && (Tipo_Dato == Tokens::DECIMAL || Tipo_Dato == Tokens::DINAMICO))
			{
				if (tokens[posicion + 1] != Tokens::PUNTO && tokens[posicion + 2] != Tokens::NUMERO)
				{
					error = "La igualdad" + comandos[posicion] + comandos[posicion + 1] + comandos[posicion + 2] + "de la variable " + nombre_variable + " es incorrecta.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicion) + ".\n";
					throw runtime_error(error.c_str());
				}
				variable.valor = comandos[posicion] + comandos[posicion + 1] + comandos[posicion + 2];
			}
			else if(tokens[posicion] == Tokens::NUMERO && (Tipo_Dato == Tokens::DINAMICO || Tipo_Dato == Tokens::ENTERO))
			{
				if (tokens[posicion + 1] != Tokens::COMAS || tokens[posicion + 1] != Tokens::FIN_COMANDO)
				{
					error = "La igualdad" + comandos[posicion] + "de la variable " + nombre_variable + " es incorrecta.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicion) + ".\n";
					throw runtime_error(error.c_str());
				}
				variable.valor = comandos[posicion];
			}
			else
			{
				error = comandos[posicion] + " no es de tipo NUMERO es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			variable.Tipo = Tokenizador::Get_Tipo(tokens[posicion]);
			break;

		case Estados::Espera_COMA_O_FIN:
			if (tokens[posicion] == Tokens::COMAS)
			{
				estado = Estados::Espera_VARIABLE;
			}
			else if (tokens[posicion] != Tokens::FIN_COMANDO)
			{
				error = comandos[posicion] + " no es de tipo Fin de linea ';'.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			Variables.push_back(variable);
			break;

		case Estados::ERROR:
			error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " en vez de tipo VARIABLE.\nLinea : " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
			throw runtime_error(error.c_str());
			break;
		}
	}

}

void Analizador_Tokens_Compilacion::Operacion()
{
	for (size_t posicion = 0; posicion < tokens.size(); posicion++)
	{
		switch (tokens[posicion])
		{
		case Tokens::OPERACION:break;
		case Tokens::OPERADOR:break;
		case Tokens::DIVISOR:break;
		case Tokens::NUMERO:break;
		case Tokens::PUNTO:break;
		case Tokens::IGUAL:break;
		case Tokens::VARIABLE:
			if (administrador.PosOBj(comandos[posicion]) != SIZE_MAX)
			{
				error = "La variable " + comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;
		default:
			error = comandos[posicion] + " es de tipo " + to_string((int)tokens[posicion]) + " y no de tipo VARIABLE.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
			throw runtime_error(error.c_str());
		}

	}
}

void Analizador_Tokens_Compilacion::Pedir()
{
	size_t Comas = 0, Variables = 0;
	vector<size_t> pos_coma;

	for (size_t posicion = 2; posicion < tokens.size(); posicion++)
	{
		switch (tokens[posicion])
		{
		case Tokens::VARIABLE:
			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = "La variable " + comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			Variables++;
			break;
		case Tokens::COMAS:
			Comas++; 
			pos_coma.push_back(posiciones[posicion]);
			break;
		case Tokens::FIN_COMANDO:break;
		default:
			error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " en vez de tipo VARIABLE.\nLinea : " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
			throw runtime_error(error.c_str());
		}
	}

	if (Comas != Variables - 1)
	{
		error = "Faltantes de comas.\nLinea: " + to_string(linea) + ", posicion: " + to_string(pos_coma[pos_coma.size()]) + ".\n";
		throw runtime_error(error.c_str());
	}

}

void Analizador_Tokens_Compilacion::Inicio_analizacion(map<string, Informacion> mapa)
{
	for (auto iterador : mapa)
	{
		tokens.push_back(iterador.second.token);
		comandos.push_back(iterador.second.comando);
		posiciones.push_back(iterador.second.posicion);
	}

	//revisar que no se repitan 
	Fin_Linea(tokens[tokens.size()]);

	switch (tokens[0])
	{
		case Tokens::IMPRIMIR:Imprimir(); break;
		case Tokens::PEDIR:Pedir(); break;
		case Tokens::ENTERO:
		case Tokens::DECIMAL:
		case Tokens::DINAMICO:Entero_Decimal_Dinamico(); break;
		case Tokens::OPERACION:Operacion(); break;
		default:
			error = comandos[0] + " no es una palabra clave.\nLinea: " + to_string(linea) + ", posicion: 0.\n";
			throw runtime_error(error.c_str());
			break;
	}
}

void Analizador_Tokens_Compilacion::Limpiar()
{
	tokens.clear();
	comandos.clear();
	posiciones.clear();
	error = "";
}
