#include"AnalizadorSemantico.h"
//Varibles hacer el objeto y ya imprimir

/************** OBTENER TEXTO **************************/

void AnalizadorSemanticoReglas::SetTexto(string _texto)
{
	this->texto = _texto;
}

/************** FIN TEXTO **************************/



/***************** DIVIDIR **************************/

bool AnalizadorSemanticoReglas::Division()
{
	if (isalpha(texto[0]))
	{
		Variable();
		Tipo = 1;
	}
	else if (texto[0] == '0')
	{
		Operacion();
		Tipo = 2;
	}
	else if (texto[0] == '(')
	{
		Impresion();
		Tipo = 3;
	}
	else if (texto[0] == '&')
	{
		Peticion();
		Tipo = 4;
	}
	else
	{
		cout << texto << ", no existe\n";
		cout << texto[0] << endl;
		Tipo = 5;
		error = true;
	}

	return error;
}


/***************** FIN DIVIDIR **************************/



/************** OBTENER IMPRESION Y PETICION **************************/

void AnalizadorSemanticoReglas::Impresion()
{
	bool escribir = (texto[1] == '"' && texto[texto.length() - 2] == '"') || (isalpha(texto[1]))  ? true : false;
	bool termino = texto[texto.length()-1] == ')'? true : false;
	error = escribir != termino ? true : false;


	if (error)cout << texto << ", error de imprimir, un objeto\n";
}

bool AnalizadorSemanticoReglas::OPERADORES(int i)
{
	return (texto[i] == '-' || texto[i] == '+' || texto[i] == '/' || texto[i] == '*');
}

/************** FIN IMPERESION Y PETICION **************************/



/************** OBTENER OPERACION **************************/

void AnalizadorSemanticoReglas::Operacion()
{
	bool signo = false;
	size_t igual = texto.find("=");

	for (size_t i = igual; i < igual; i++) {

		if (OPERADORES(static_cast<int>(i)) && signo)
		{
			cout << texto[i - 1] << "," << texto[i] << " no puedes colocar dos signos matematicos seguidos\n";
			break;
			error = true;
		}

		signo = OPERADORES(static_cast<int>(i));

	}

	if (!error)
	{
		error = igual != string::npos && !OPERADORES(static_cast<int>(texto.length())) ? false : true;
		if (error)cout << texto << ", a la variable le falta el signo igual, para hacer dicha operacion\n";
	}
}

/************** FIN OPERACIONES **************************/


void AnalizadorSemanticoReglas::Peticion()
{
	error = (texto[0] == '&' && texto[texto.length()-1] == '&') && (isalpha(texto[1])) ? false : true;


	if (error)cout << texto << ", peticion pedida de manera incorrecta\n";
}


/************** OBTENER TIPO **************************/

int AnalizadorSemanticoReglas::Get_Tipo()
{
	return Tipo;
}

/************** FIN  TIPO *********************/



/************** OBTENER LIMPIAR **************************/

void AnalizadorSemanticoReglas::Limpiar()
{
	Tipo = 0;
	texto = "";
}

/************** FIN LIMPIAR **************************/


void AnalizadorSemanticoReglas::Variable()
{
	error = isalpha(texto[0]) ? false : true;
	
	if (error)
	{
		string variable = "";
		size_t igual = texto.find("=");
		if (igual != string::npos)
		{
			for (int i = 0; i < igual; i++)
			{
				variable += texto[i];
			}
		}
		else
		{
			//en caso de no colocar un signo igual para darle
			//una igualdad 
			for (int i = 0; i < texto.length()-1; i++)
			{
				variable += texto[i];
			}
		}

		cout << "creacion de " + variable + " tiene un nombre no permitito\n";
	}
}

/********************************** ANALIZADOR SEMANTICO REGLAS FIN *******************************************/

/*size_t igual = texto.find("=");

	if (igual != string::npos) {
		cout<<"Advertencia variable sin inicializar"
	}*/



	/********************************** INICIO ANALIZADOR SEMANTICO COMPROBACION  *******************************************/

void AnalizadorSemanticoComprobacion::Inicio()
{
	archivo.open(dirarchivo,ios::app);

	switch (tipo)
	{
	case 1:
		//variable
		Creacion();
		break;
	case 2:
		//Operaciones
		Operacion();
		break;
	case 3:
		//Impresion
		Impresion();
		break;
	case 4:
		//Peticion
		Peticion();
		break;
	}
	archivo.close();
}

void AnalizadorSemanticoComprobacion::Impresion()
{
	string imprimir = "",mandar = "";

	for (int i = 1; i < impresion_peticion.length() - 1; i++) {
		imprimir += impresion_peticion[i];
	}

	if (!isalpha(imprimir[0])){
		mandar = imprimir;
	}
	else {
		mandar = "N" + imprimir;
	}

	archivo << mandar << endl;
}

void AnalizadorSemanticoComprobacion::Peticion()
{
	string imprimir = "",igualdad = "";
	for (int i = 1; i < impresion_peticion.length() - 1; i++) {
		imprimir += impresion_peticion[i];
	}
	//cin >> igualdad;
	//A.NuevaIgualdad(imprimir, igualdad);
	archivo << "P" << imprimir << endl;
}

void AnalizadorSemanticoComprobacion::Operacion()
{
	vector<string> valores;
	int pos = 0;
	for (auto i : variables)
	{
		if (!isdigit(i[0]))
		{
			pos = A.PosOBj(i);
			valores.push_back(obj[pos]->GetValor());
		}
		else
		{
			valores.push_back(i);
		}
	}

	int posop = 0;
	archivo << "O" << variable << "=";

	for (auto i : valores) {
		archivo << i;
		if (posop != operadores.size())archivo << operadores[posop++];
	}
	archivo << endl;
	error = variables.size() / operadores.size() == 2 ? false : true;
}

void AnalizadorSemanticoComprobacion::Creacion()
{
	if (igualdad != "") {
		//aaaaaaaaaaaaaaaaaaaaaaaaa
	}
	else {
		cout << "Advertencia variable " + variable + " no inicializada\n";
		igualdad = "0";
	}

	A.Crear(variable, igualdad);

}


void AnalizadorSemanticoComprobacion::Limpiar()
{
	tipo = 0;
	variable = "";
	impresion_peticion = "";
	igualdad = "";
	variables.clear();
	operadores.clear();
	error = false;
	tipo = 0;
}
