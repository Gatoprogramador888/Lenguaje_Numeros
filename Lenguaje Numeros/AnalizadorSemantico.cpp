#include"AnalizadorSemantico.h"

void Analizador_Tokens_Compilacion::Fin_Linea(Tokens fin_tokens)
{
	if(fin_tokens != Tokens::FIN_COMANDO)
	{ 
		string error = "Se esperaba ';' en vez de " + comandos[comandos.size() - 1] + " .\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posiciones.size() - 1]) + ".\n";
		throw runtime_error(error.c_str());
	}
}

void Analizador_Tokens_Compilacion::Imprimir()
{
	enum class Estados
	{
		INICIO, ESPERA_DIVISOR, ESPERA_COMILLAS,
		ESPERA_PARENTESIS_DERECHO, ESPERA_PARENTESIS_IZQUIERDO, ESPERA_VARIABLE,
		ESPERA_CARACTER, ESPERA_COMAS_FIN_COMANDO, ERROR, ESPERA_TEXTO 
	};
	Estados estado = Estados::INICIO;
	bool Es_Texto = tokens[2] != Tokens::TEXTO ? false : true;
	int8_t comillas = Es_Texto ? 1 : 0;
	archivo_a_compilar << "\n>\n";

	for (size_t posicion = 0; posicion < tokens.size(); posicion++)
	{
		if (!Es_Texto)
		{
			switch (estado)
			{

			case Estados::INICIO:
			{
				estado = Estados::ESPERA_DIVISOR;
				if (tokens[posicion] != Tokens::IMPRIMIR)
				{
					error = comandos[posicion] + " no es de tipo Imprimir.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ESPERA_DIVISOR:
			{
				estado = Estados::ESPERA_VARIABLE;
				if (tokens[posicion] != Tokens::DIVISOR)
				{
					error = "Se esperaba ':' no "+comandos[posicion]+".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ESPERA_VARIABLE:
			{
				estado = Estados::ESPERA_COMAS_FIN_COMANDO;
				if (tokens[posicion] != Tokens::VARIABLE)
				{
					error = comandos[posicion] + " no existe dicha variable.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
					break;
				}
				if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
				{
					error = comandos[posicion] + " no existe dicha variable.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
					break;
				}
				archivo_a_compilar << comandos[posicion] << "\n";
				break;
			}

			case Estados::ESPERA_COMAS_FIN_COMANDO:
			{
				if (tokens[posicion] == Tokens::COMAS)
				{
					estado = Estados::ESPERA_VARIABLE;
				}
				else if (tokens[posicion] != Tokens::FIN_COMANDO)
				{
					error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ERROR:
			{
				throw runtime_error(error.c_str());
			}

			}

		}
		else
		{
			switch (estado)
			{
			case Estados::INICIO:
			{
				estado = Estados::ESPERA_DIVISOR;
				if (tokens[posicion] != Tokens::IMPRIMIR)
				{
					error = comandos[posicion] + " no es de tipo Imprimir.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ESPERA_DIVISOR:
			{
				estado = Estados::ESPERA_TEXTO;
				if (tokens[posicion] != Tokens::DIVISOR)
				{
					error = "Se esperaba ':' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ESPERA_TEXTO:
			{
				estado = Estados::ESPERA_COMILLAS;
				break;
			}

			case Estados::ESPERA_COMILLAS:
			{
				if (posicion + 1 < tokens.size())
				{
					switch (tokens[posicion + 1])
					{
					case Tokens::CARACTER:
						estado = Estados::ESPERA_CARACTER;
						break;
					case Tokens::PARENTESIS_IZQUIERDO:
						estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
						break;
					case Tokens::COMILLAS:

						if (comillas != 2)
						{
							estado = Estados::ESPERA_COMILLAS;
							comillas++;
						}
						else
						{
							estado = Estados::ESPERA_COMAS_FIN_COMANDO;
						}
						break;
					case Tokens::FIN_COMANDO:
						if (comillas == 2)
							estado = Estados::ESPERA_COMAS_FIN_COMANDO;
						else
						{
							error = "Se esperaba'\"' en vez de " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
							estado = Estados::ERROR;
						}
						break;
					default:
						error = "Se esperaba'\"' en vez de " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
						estado = Estados::ERROR;
						break;
					}
				}
				else
				{
					error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}


				if (tokens[posicion] != Tokens::COMILLAS)
				{
					error = "Se esperaba \" en vez de " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ESPERA_CARACTER:
			{

				if (posicion + 1 < tokens.size())
				{
					switch (tokens[posicion + 1])
					{
					case Tokens::CARACTER:
						estado = Estados::ESPERA_CARACTER;
						break;
					case Tokens::PARENTESIS_IZQUIERDO:
						estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
						break;
					case Tokens::COMILLAS:

						if (comillas != 2)
						{
							estado = Estados::ESPERA_COMILLAS;
							comillas++;
						}
						else
						{
							estado = Estados::ESPERA_COMAS_FIN_COMANDO;
						}
						break;
					case Tokens::FIN_COMANDO:
						if (comillas == 2)
							estado = Estados::ESPERA_COMAS_FIN_COMANDO;
						else
						{
							error = "Se esperaba'\"' en vez de " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
							estado = Estados::ERROR;
						}
						break;
					default:
						error = "Se esperaba'\"' en vez de " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
						estado = Estados::ERROR;
						break;
					}
				}
				else
				{
					error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				if (estado != Estados::ERROR)
					archivo_a_compilar  << comandos[posicion] << "\n";

				break;
			}

			case Estados::ESPERA_PARENTESIS_IZQUIERDO:
			{

				estado = Estados::ESPERA_VARIABLE;

				if (tokens[posicion] != Tokens::PARENTESIS_IZQUIERDO)
				{
					error = "Se esperaba '(' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ESPERA_VARIABLE:
			{
				if (posicion + 1 < tokens.size())
					estado = tokens[posicion + 1] != Tokens::COMAS ? Estados::ESPERA_PARENTESIS_DERECHO : Estados::ESPERA_COMAS_FIN_COMANDO;
				else
				{
					error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				if (tokens[posicion] != Tokens::VARIABLE)
				{
					error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " en vez de VARIABLE.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
				{
					error = comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				if (estado != Estados::ERROR)
					archivo_a_compilar << comandos[posicion] << "\n";

				break;
			}

			case Estados::ESPERA_PARENTESIS_DERECHO:
			{
				if (posicion + 1 < tokens.size())
				{
					switch (tokens[posicion + 1])
					{
					case Tokens::CARACTER:
						estado = Estados::ESPERA_CARACTER;
						break;
					case Tokens::PARENTESIS_IZQUIERDO:
						estado = Estados::ESPERA_PARENTESIS_IZQUIERDO;
						break;
					case Tokens::COMILLAS:

						if (comillas != 2)
						{
							estado = Estados::ESPERA_COMILLAS;
							comillas++;
						}
						else
						{
							estado = Estados::ESPERA_COMAS_FIN_COMANDO;
						}
						break;
					case Tokens::FIN_COMANDO:
						if (comillas == 2)
							estado = Estados::ESPERA_COMAS_FIN_COMANDO;
						else
						{
							error = "Se esperaba'\"' en vez de " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
							estado = Estados::ERROR;
						}
						break;
					default:
						error = "Se esperaba'\"' en vez de " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
						estado = Estados::ERROR;
						break;
					}
				}
				else
				{
					error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				if (tokens[posicion] != Tokens::PARENTESIS_DERECHO)
				{
					error = "Se esperaba ')' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				break;
			}

			case Estados::ESPERA_COMAS_FIN_COMANDO:
			{
				if (tokens[posicion] == Tokens::COMAS)
				{
					estado = Estados::ESPERA_VARIABLE;
				}
				else if (tokens[posicion] != Tokens::FIN_COMANDO)
				{
					error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}
				break;
			}

			case Estados::ERROR:
			{
				throw runtime_error(error.c_str());
			}

			}
		}
	}
	archivo_a_compilar << "\n>\n";
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

	for (size_t posicion = 0; posicion < tokens.size(); posicion++)
	{
		switch (estado)
		{
		case Estados::INICIO:
			if (tokens[posicion] == Tokens::ENTERO || tokens[posicion] == Tokens::DECIMAL || tokens[posicion] == Tokens::DINAMICO)
			{
				estado = Estados::Espera_DIVISOR;
				Tipo_Dato = tokens[posicion];
				variable.Tipo = Tokenizador::Get_Tipo(tokens[posicion]);
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
			else if (tokens[posicion] == Tokens::COMAS || tokens[posicion] == Tokens::FIN_COMANDO)
			{
				posicion--;
				error = "La variable: " + comandos[posicion] + " no ah sido inicializada.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				cout << error;
				variable.valor = Tipo_Dato != Tokens::DECIMAL ? "0" : "0.00";

				estado = Estados::Espera_COMA_O_FIN;
			}
			else {
				error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " no de tipo Igual.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;

		case Estados::Espera_IGUALDAD:

			for (int i = 0; i < comandos[posicion].size(); i++)
			{
				if (isalpha(comandos[posicion][i]))
				{
					error = "Los Numeros no pueden tener letras.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					throw runtime_error(error.c_str());
				}
			}

			estado = Estados::Espera_COMA_O_FIN;

			if (tokens[posicion] == Tokens::NUMERO && Tipo_Dato == Tokens::DECIMAL)
			{
				size_t punto = comandos[posicion].find(".");
				if (punto == string::npos)
				{
					error = "La igualdad " + comandos[posicion] + " de la variable " + nombre_variable + " es incorrecta.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicion) + ".\n";
					throw runtime_error(error.c_str());
				}
			}
			else if(tokens[posicion] == Tokens::NUMERO && Tipo_Dato == Tokens::ENTERO)
			{
				size_t punto = comandos[posicion].find(".");
				if (punto != string::npos)
				{
					error = "La igualdad " + comandos[posicion] + " de la variable " + nombre_variable + " es incorrecta.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicion) + ".\n";
					throw runtime_error(error.c_str());
				}
			}
			else if(Tipo_Dato != Tokens::DINAMICO)
			{
				error = comandos[posicion] + " no es de tipo Numero es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}

			variable.valor = comandos[posicion];

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

	Variables.push_back(variable);
	for (auto informacion : Variables)
	{
		administrador.Crear(informacion);
	}

}

void Analizador_Tokens_Compilacion::Operacion()
{

	enum class Estados{INICIO, DIVISOR, ESPERA_VARIABLE, ESPERA_IGUAL, ESPERA_NUMERO, ESPERA_OPERADOR, ESPERA_FIN_COMANDO, ERROR};
	Estados estado = Estados::INICIO;
	string tipo = "",variable = "";
	archivo_a_compilar << "\n";

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
			break;
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
				break;
			}

			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = comandos[posicion] + " no existe.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}
			tipo = obj[administrador.PosOBj(comandos[posicion])]->GetType();
			variable = comandos[posicion];

			archivo_a_compilar << variable;

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

			if (estado != Estados::ERROR)
				archivo_a_compilar << " = ";

			break;
		}
		case Estados::ESPERA_NUMERO:
		{
			if (tokens[posicion] != Tokens::VARIABLE && tokens[posicion] != Tokens::NUMERO)
			{
				error = comandos[posicion] + " es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " en vez de tipo Variable.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}

			if (posicion + 1 < tokens.size())
				estado = tokens[posicion + 1] != Tokens::FIN_COMANDO ? Estados::ESPERA_OPERADOR : Estados::ESPERA_FIN_COMANDO;
			else
			{
				error = "Se esperaba un ';'.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}

			if (tokens[posicion] == Tokens::NUMERO)
			{

				if (posicion < tokens.size())
					estado = tokens[posicion + 1] != Tokens::FIN_COMANDO ? Estados::ESPERA_OPERADOR : Estados::ESPERA_FIN_COMANDO;
				else
				{
					error = "Se esperaba ';'.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
					break;
				}

				//Numero
				if (obj[administrador.PosOBj(variable)]->GetType() == "ENTERO")
				{
					size_t punto = comandos[posicion].find(".");
					if (punto != string::npos)
					{
						error = "El numero " + comandos[posicion] + " no es de tipo entero.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
						estado = Estados::ERROR;
					}
				}
				if (obj[administrador.PosOBj(variable)]->GetType() == "DECIMAL")
				{
					size_t punto = comandos[posicion].find(".");
					if (punto == string::npos)
					{
						error = "El numero " + comandos[posicion] + " no es de tipo Decimal.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
						estado = Estados::ERROR;
					}
				}

				for (int i = 0; i < comandos[posicion].size(); i++)
				{
					if (isalpha(comandos[posicion][i]))
					{
						error = "Los Numeros no pueden tener letras.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
						estado = Estados::ERROR;
					}
				}

			}
			else if(tokens[posicion] == Tokens::VARIABLE)
			{
				//Variable

				if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
				{
					error = comandos[posicion]+" no existe.\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
					break;
				}

				if (obj[administrador.PosOBj(comandos[posicion])]->GetType() != tipo || obj[administrador.PosOBj(comandos[posicion])]->GetType() != "DINAMICO")
				{
					error = comandos[posicion] + " no es del mismo tipo que " + variable + ".\nLinea: " + to_string(linea) + ", posicion : " + to_string(posiciones[posicion]) + ".\n";
					estado = Estados::ERROR;
				}

				if (posicion < tokens.size())
					estado = tokens[posicion + 1] != Tokens::FIN_COMANDO ? Estados::ESPERA_OPERADOR : Estados::ESPERA_FIN_COMANDO;
				else
				{
					error = "Se esperaba ';'.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion])+".\n";
					estado = Estados::ERROR;
				}
			}

			if (estado != Estados::ERROR)
				archivo_a_compilar << comandos[posicion];

			break;
		}

		case Estados::ESPERA_OPERADOR:
		{
			if (posicion + 1 < tokens.size())
			{
				estado = tokens[posicion + 1] != Tokens::NUMERO ? Estados::ERROR : Estados::ESPERA_NUMERO;

				if (estado == Estados::ERROR)
				{
					error = "Se esperaba una variable en " + comandos[posicion + 1] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
					break;
				}
			}
			else
			{
				error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
				break;
			}

			if (estado == Estados::ERROR)break;

			if (tokens[posicion] != Tokens::OPERADOR)
			{
				error = comandos[posicion] + " Es de tipo " + Tokenizador::Get_Tipo(tokens[posicion]) + " no tipo Operador.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}

			if (estado != Estados::ERROR)
				archivo_a_compilar << " " << comandos[posicion] << " ";

			break;
		}

		case Estados::ESPERA_FIN_COMANDO:
		{
			if (tokens[posicion] != Tokens::FIN_COMANDO)
			{
				error = "Se esperaba ';' no " + comandos[posicion] + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				estado = Estados::ERROR;
			}
			break;
		}

		case Estados::ERROR:
			throw runtime_error(error.c_str());
			break;
		}

	}
	archivo_a_compilar << "\n";
}

void Analizador_Tokens_Compilacion::Pedir()
{
	//Iniciar pedir
	enum class Estados{INICIO, ESPERA_DIVISOR, ESPERA_VARIABLE, ESPERA_COMA_O_FIN, ERROR};
	Estados estado = Estados::INICIO;
	archivo_a_compilar << "\n<\n";
	
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

			if (administrador.PosOBj(comandos[i]) == SIZE_MAX)
			{
				error = comandos[i]+" no existe.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[i]) + ".\n";
				estado = Estados::ERROR;
			}

			if (estado != Estados::ERROR)
				archivo_a_compilar << comandos[i] << "\n";

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

	archivo_a_compilar << "<\n";
}

void Analizador_Tokens_Compilacion::Inicio_analizacion(map<string, Informacion> mapa, string nombre_archivo)
{

	archivo_a_compilar.open(nombre_archivo);

	for (auto iterador : mapa)
	{
		tokens.push_back(iterador.second.token);
		comandos.push_back(iterador.second.comando);
		posiciones.push_back(iterador.second.posicion);
	}

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

	Fin_Linea(tokens.at(tokens.size() - 1));

}

void Analizador_Tokens_Compilacion::Limpiar()
{
	tokens.clear();
	comandos.clear();
	posiciones.clear();
	error = "";
}
