#include"AnalizadorSemantico.h"
//Varibles hacer el objeto y ya imprimir

bool AnalizadorSemantico::Variable()
{
	auto Igual = [&]()
	{
		for (int i = 0; i < igualdad.length(); i++)
		{
			if (!isdigit(igualdad[i])) return false;
		}
		return true;
	};

	return isalpha(variable[0]) && Igual();
}

bool AnalizadorSemantico::Operador()
{
	int r = 0,pos = 0,n1 = 0;

	r = stoi(Variables[0]);
	archivo << variable << "=" << Variables[0];

	for (int i = 0; i < Operadores.size(); i++)
	{

		if (pos <= Variables.size())
		{
			pos++;
		}
		else
		{
			break;
		}

		if (Operadores[i] == "+")
		{
			n1 = stoi(Variables[pos]);
			archivo << "+" << Variables[pos];
			r += n1;
		}
		else if (Operadores[i] == "-")
		{
			n1 = stoi(Variables[pos]);
			archivo << "-" << Variables[pos];
			r -= n1;
		}
		else if (Operadores[i] == "*")
		{
			n1 = stoi(Variables[pos]);
			archivo << "*" << Variables[pos];
			r *= n1;
		}
		else if (Operadores[i] == "/")
		{
			n1 = stoi(Variables[pos]);
			archivo << "/" << Variables[pos];
			r /= n1 ;
		}

	}

	string aux = "";
	int j = 0;

	for (int i = 0; i < variable.size(); i++)
	{
		if (variable[i] != '0')
		{
			aux += variable[i];
			j++;
		}
		
	}
	variable = aux;

	char respuesta[100];
	itoa(r, respuesta,100);

	OutputDebugString(respuesta);

	igualdad = to_string(r);

	archivo << endl;


	return true;
}

bool AnalizadorSemantico::Imprimir()
{
	//VER SI ES VARIABLE O SI NO LO ES A LA HORA DE IMPRIMIR
	string var = "";
	string igual = "";
	bool crear = false;

	auto RevisionCaracterInvalido = [&]() {
		for (int i = 1; i < Impresion.length() - 1; i++)
		{
			//Verificamos que no contenga otro tipo de caracter
			if (!isdigit(Impresion[i]) && !isalpha(Impresion[i]))return false;
			var += Impresion[i];
		}
	};

	if (RevisionCaracterInvalido()) {
		if (Impresion[0] == '(' && Impresion[Impresion.length() - 1] == ')')
		{
			//Verificar si es o no variable
			if (!isdigit(var[0]))
			{
				archivo << obj[A.PosOBj(var)]->GetValor() << endl;
			}
			else
			{
				archivo << var << endl;
			}
		}
		//PEDIR DATOS
		else if (Impresion[0] == '&' && Impresion[Impresion.length() - 1] == '&')
		{
			
			for (auto i : obj)
			{
				if (i->GetNombre() != var) 
				{
					crear = false;
				}
				else
				{
					crear = true;
					break;
				}
			}

			if (crear == true)
			{
				cin >> igual;
				A.NuevaIgualdad(var, igual);
			}
			else
			{
				cout << "\nObjeto inexistente(" << var << ")cree uno con ese nombre\n";
				return false;
			}

		}
		else
		{
			cout << "Error al pedir datos o imprimir: " << Impresion << " es incorrecto\n";
			return false;
		}

		return true;
	}
	return false;
}

void AnalizadorSemantico::Inicializar()
{
	variable = _variable;
	igualdad = _igualdad;
	Impresion = _impresion;
	Variables = _variables;
	Operadores = _operadores;
	tipo = _tipo;
	archivo = ofstream(Archivo,ios::app);
}

bool AnalizadorSemantico::Dividir(string archivodir)
{	 
	this->Archivo = archivodir;
	
	bool retorno = false;
	switch (tipo)
	{
	case 1:
		if (Variable()) {
			retorno = Variable();
			A.Crear(variable, igualdad);
		}
		else
		{
			cout << "Error En la creacion de variables: " << variable << " no creara correctamente\n";
		}
		break;
	case 2:
		retorno = Operador();
		if (retorno) {
			A.NuevaIgualdad(variable,igualdad);
		}
		else
		{
			cout << "Error al operar " << variable << endl;
		}
		break;
	case 3:
		retorno = Imprimir();
		break;
	case 4:
		retorno = false;
		break;
	}
	archivo.close();
	return retorno;
}

void AnalizadorSemantico::Limpiar()
{
	variable = "";
	igualdad = "";
	Impresion = "";
	Variables.clear();
	Operadores.clear();
	tipo = 0;
}

