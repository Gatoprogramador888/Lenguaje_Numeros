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
	int8_t comillas = 0;
	bool texto = comandos[3] != "$" ? false : true;
	size_t parentesis_derecho = 0, parentesis_izquierdo = 0, posicion = 0;
	vector<size_t> pos_parentesis_derecho, pos_parentesis_izquierdo, pos_comillas;

	for (Tokens token : tokens)
	{
		switch (token)
		{
		case Tokens::COMILLAS: 
			comillas++;
			pos_comillas.push_back(posiciones[posicion]);
			break;
		case Tokens::PARENTESIS_DERECHO: 
			parentesis_derecho++;
			pos_parentesis_derecho.push_back(posiciones[posicion]);
			break;
		case Tokens::PARENTESIS_IZQUIERDO:
			parentesis_izquierdo++;
			pos_parentesis_izquierdo.push_back(posiciones[posicion]);
			break;
		case Tokens::VARIABLE:
			if (administrador.PosOBj(comandos[posicion]) == SIZE_MAX)
			{
				error = "La variable: " + comandos[posicion] + " no existe\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[posicion]) + ".\n";
				throw runtime_error(error.c_str());
			}
			break;
		}
		posicion++;
	}

	if (!texto && (comillas != 0 || parentesis_izquierdo != 0 || parentesis_derecho != 0) )
	{
		error = "Imprimir un texto cuando no se especifico un texto en " + comandos[3] + "\nLinea: " + to_string(linea) + ", posicion: " + to_string(posiciones[3]) + ".\n";
		throw runtime_error(error.c_str());
	}
	else
	{
		if (parentesis_derecho != parentesis_izquierdo)
		{
			size_t aux = pos_parentesis_derecho.size(), aux2 = pos_parentesis_izquierdo.size();
			size_t posicionaux = aux > aux2 ? pos_parentesis_derecho[aux] : pos_parentesis_izquierdo[aux2];
			string faltante = aux > aux2 ? "derecho" : "izquierdo";
			error = "falta parentesis " + faltante + ".\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicionaux) + "\n.";
			throw runtime_error(error.c_str());
		}
		else if (comillas < 2)
		{
			int8_t posicionaux = pos_comillas.size() != 0 ? static_cast<int8_t>(pos_comillas[0]) : 0;
			error = "Faltante de comillas.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicionaux) + ".\n";
			throw runtime_error(error.c_str());
		}
		else if (comillas > 2)
		{
			size_t posicionaux = pos_comillas[pos_comillas.size()];
			error = "Sobran comillas.\nLinea: " + to_string(linea) + ", posicion: " + to_string(posicionaux) + ".\n";
			throw runtime_error(error.c_str());
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
