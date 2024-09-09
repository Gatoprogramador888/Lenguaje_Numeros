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
			estado = Estados::ESPERA_DIVISOR;
			if (tokens[posicion] != Tokens::IMPRIMIR)
			{
				error = comandos[posicion] + " no es de tipo Imprimir.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			break;

		case Estados::ESPERA_DIVISOR:

			if (posicion + 1 < tokens.size())
				estado = tokens[posicion + 1] != Tokens::COMILLAS ? Estados::ESPERA_VARIABLE : Estados::ESPERA_COMILLAS;
			else
			{
				error = "Se esperaba un ';'.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}

			if (tokens[posicion] != Tokens::DIVISOR)
			{
				error = "Se esperaba un ':' no un " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}

			break;

		case Estados::ESPERA_VARIABLE:

			estado = Estados::ESPERA_PARENTESIS_DERECHO;

			if (tokens[posicion] != Tokens::VARIABLE)
			{
				error = comandos[posicion]+" es de tipo "+Tokenizador::Get_Tipo(tokens[posicion])+" no de tipo Variable.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}

			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}

			if(posicion + 1 < tokens.size())
			estado = tokens[posicion + 1] != Tokens::PARENTESIS_DERECHO? Estados::ESPERA_COMAS_FIN_COMANDO : Estados::ESPERA_PARENTESIS_DERECHO;
			else
			{
				error = "Se esperaba un ';'.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}

			break;

		case Estados::ESPERA_COMILLAS:
			if (tokens[posicion] != Tokens::COMILLAS)
			{
				error = "Se esperaba '\"' en vez de " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}

			if (posicion + 1 < tokens.size())
			{
				switch (tokens[posicion + 1])
				{
				case Tokens::PARENTESIS_IZQUIERDO:
					estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
					break;
				case Tokens::CARACTER:
					estado = Estados::ESPERA_CARACTER;
					break;
				case Tokens::COMILLAS:
					estado = Estados::ESPERA_COMILLAS;
					break;
				default:
					error = "Se esperaba '\"' en vez de " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
					break;
				}
			}
			else
			{
				error = "Se esperaba un ';'.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}
			break;

		case Estados::ESPERA_CARACTER:

			if (posicion + 1 < tokens.size())
			{
				switch (tokens[posicion + 1])
				{
				case Tokens::PARENTESIS_IZQUIERDO:
					estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
					break;
				case Tokens::CARACTER:
					estado = Estados::ESPERA_CARACTER;
					break;
				case Tokens::COMILLAS:
					estado = Estados::ESPERA_COMILLAS;
					break;
				default:
					error = "Se esperaba '\"' en vez de " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
					break;
				}
			}
			else
			{
				error = "Se esperaba un ';'.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}
			break;

		case Estados::ESPERA_PARENTESIS_IZQUIERDO:
			if (tokens[posicion] != Tokens::PARENTESIS_IZQUIERDO)
			{
				error = "Se esperaba un '(' no un " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			estado = Estados::ESPERA_VARIABLE;
			break;

		case Estados::ESPERA_PARENTESIS_DERECHO:

			if (posicion < tokens.size())
				estado = tokens[posicion + 1] != Tokens::COMAS ? Estados::ESPERA_CARACTER : Estados::ESPERA_COMAS_FIN_COMANDO;

			if (tokens[posicion] != Tokens::PARENTESIS_DERECHO && tokens[posicion] != Tokens::COMAS)
			{
				error = "Se esperaba un ')' no un " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			
			if (posicion + 1 < tokens.size())
				estado = tokens[posicion + 1] != Tokens::COMILLAS ? Estados::ESPERA_CARACTER : Estados::ESPERA_COMILLAS;
			else
			{
				error = "Se esperaba un ';'.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}
			break;

		case Estados::ESPERA_COMAS_FIN_COMANDO:
			if (tokens[posicion] == Tokens::COMAS)
			{
				estado = Estados::ESPERA_VARIABLE;
			}
			else if(tokens[posicion] != Tokens::FIN_COMANDO)
			{
				error = "Se esperaba un ';' no un " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			break;

		case Estados::ERROR:
			throw runtime_error(error.c_str());
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
			if (tokens[posicion] == Tokens::VARIABLE && administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
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

	enum class Estados{INICIO, DIVISOR, ESPERA_VARIABLE, ESPERA_IGUAL, ESPERA_NUMERO, ESPERA_OPERADOR, ESPERA_FIN_COMANDO, ERROR};
	Estados estado = Estados::INICIO;

	for (size_t posicion = 0; posicion < tokens.size(); posicion++)
	{
		switch (estado)
		{
		case Estados::INICIO:
		{
			estado = Estados::DIVISOR;

			if (tokens[posicion] != Tokens::OPERACION)
			{
				error = comandos[posicion] + " no es de tipo Operacion.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
		}
		case Estados::DIVISOR:
		{
			estado = Estados::ESPERA_VARIABLE;
			if (tokens[posicion] != Tokens::DIVISOR)
			{
				error = "Se esperaba ':' no " + comandos[posicion] + ".\nLinea: " + to_string(posicion) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			break;
		}
		case Estados::ESPERA_VARIABLE:
		{
			estado = Estados::ESPERA_IGUAL;
			if (tokens[posicion] != Tokens::VARIABLE)
			{
				error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " en vez de tipo Variable.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			break;
		}
		case Estados::ESPERA_IGUAL:
		{
			estado = Estados::ESPERA_NUMERO;
			if (tokens[posicion] != Tokens::IGUAL)
			{
				error = "Se esperaba '=' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			break;
		}
		case Estados::ERROR:
			throw runtime_error(error.c_str());
			break;
		}

	}
}

void Analizador_Tokens_Compilacion::Pedir()
{
	//Iniciar pedir
	enum class Estados{INICIO, ESPERA_DIVISOR, ESPERA_VARIABLE, ESPERA_COMA_O_FIN, ERROR};
	Estados estado = Estados::INICIO;
	
	for (size_t i = 0; i < tokens.size(); i++)
	{
		switch (estado)
		{

		case Estados::INICIO:
		{
			estado = Estados::ESPERA_DIVISOR;
			if (tokens[i] != Tokens::PEDIR)
			{
				error = comandos[i] + " no es de tipo Pedir es de tipo " + Tokenizador::Get_Tipo(tokens[i]) + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[i]) + ".\n";
				estado = Estados::ERROR;
			}
			break;
		}

		case Estados::ESPERA_DIVISOR:
		{
			estado = Estados::ESPERA_VARIABLE;

			if (tokens[i] != Tokens::DIVISOR)
			{
				error = "Se esperaba ':' no " + comandos[i] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[i]) + ".\n";
				estado = Estados::ERROR;
			}
			break;
		}

		case Estados::ESPERA_VARIABLE:
		{
			estado = Estados::ESPERA_COMA_O_FIN;

			if (tokens[i] != Tokens::VARIABLE)
			{
				error = comandos[i] + " es de tipo " + Tokenizador::Get_Tipo(tokens[i]) + " en vez de tipo Variable.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[i]) + ".\n";
				estado = Estados::ERROR;
			}

			break;
		}

		case Estados::ESPERA_COMA_O_FIN:
		{
			if (tokens[i] == Tokens::COMAS)
			{
				estado = Estados::ESPERA_VARIABLE;
				break;
			}

			if (tokens[i] != Tokens::FIN_COMANDO)
			{
				error = "Se esperaba ';' no un " + comandos[i] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[i]);
				estado = Estados::ERROR;
			}

			break;
		}

		case Estados::ERROR:
			throw runtime_error(error.c_str());
			break;
		}
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
