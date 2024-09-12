#include "Comandos_App.h"

Comandos_App::Comandos_App(int argc, char** argv)
{
	string* comandos = new string[argc];

	for (int i = 0; i < argc; i++)
	{
		for (int j = 0; i < strlen(argv[i]); j++)
		{
			comandos[i] += argv[i][j];
		}
	}

	if (comandos[0] == "ayuda")
	{
		help.Set_Ayuda(comandos, argc);
	}
	if (comandos[0] == "compilar")
	{

	}
	if (comandos[0] == "interpretar")
	{

	}
	if (comandos[0] == "compilar_I")
	{

	}


}

void Tipo_Comandos::Ayuda::Set_Ayuda(string* comandos, int argc)
{
	if (comandos[1] == "comandos")
	{
		Comandos();
	}
	if (comandos[1] == "compilar")
	{
		Compilar();
	}
	if (comandos[1] == "sintaxis")
	{
		Sintaxis(comandos[2]);
	}
	if (comandos[1] == "interpretar")
	{
		Interpretar();
	}
	if (comandos[1] == "compilar_I")
	{
		Compilar_I();
	}
	
}

void Tipo_Comandos::Ayuda::Comandos()
{
	cout << "-comandos\n-compilar\n-sintaxis\n-interpretar\n-compilar_I\n";
}

void Tipo_Comandos::Ayuda::Compilar()
{
	cout << "\"Lenguaje Numeros\" compilar archivo_codigo.txt archivo_a_compilar.txt\n";
}

void Tipo_Comandos::Ayuda::Sintaxis(string comando)
{
	if (comando == "todo")
	{
		cout << "-todo\n-imprimir\n-pedir\n-declarar\n-operacion\n";
	}
	if (comando == "imprimir")
	{
		cout << "Imprimir : mi_variable;\nImprimir : mi_variable,...;\nImprimir : $ \"hola mundo\";\nImprimir : $ \"El valor es: {mi_variable}.\";\n";
	}
	if (comando == "pedir")
	{
		cout << "Pedir : mi_variable;\n Pedir : mi_variable,...;\n";
	}
	if (comando == "declarar")
	{
		cout << "Tipo Variables:\n-Entero\n-Decimal\n-Dinamico\n\nSintaxis:\nDinamico : var = 1;\nDinamico : var = 1, var2 = 1.5,...;\n";
	}
	if (comando == "operacion")
	{
		cout << "Operacion : variable = 1;\nOperacion : variable = otra_variable + 1;\n";
	}
}

void Tipo_Comandos::Ayuda::Interpretar()
{
	cout << "\"Lenguaje Numeros\" interpretar archivo_compilado.txt\n";
}

void Tipo_Comandos::Ayuda::Compilar_I()
{
	cout << "\"Lenguaje Numeros\" compilar_I archivo_codigo.txt archivo_compilado_interpretar.txt\n";
}


void Tipo_Comandos::Compilar::Set_Compilar(string archivo_compilar, string archivo_compilado)
{
	string linea_contenido = "";
	size_t linea = 1;
	Analizador_Tokens_Compilacion ATC;
	ifstream archivo_a_compilar(archivo_compilar);
	ofstream archivo_a_interpretar(archivo_compilado);

	if (archivo_a_compilar.is_open())
	{
		cout << "El archivo " << archivo_compilar << " no existe.\n";
		exit(1);
	}

	while (getline(archivo_a_compilar,linea_contenido))
	{
		try
		{
			for (string contenido : divisor.Divisiones_Varias_lineas_Comandos(linea_contenido))
			{
				ATC.linea = linea;

				divisor.Inicio(linea_contenido);

				auto mapa = tokenizador.Mapa_Informacion(divisor.Get_Comandos(), divisor.Info());

				ATC.Inicio_analizacion(mapa);

				divisor.Limpiar();
				tokenizador.Limpiar();
				ATC.Limpiar();

				linea++;
			}
		}
		catch (runtime_error& error)
		{
			cerr << error.what();
		}
		catch (out_of_range& error)
		{
			cerr << error.what();
		}
	}
}

void Tipo_Comandos::Interpretar::Set_Interpretar(string archivo_compilado_interpretar)
{

}