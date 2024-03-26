#include"AnalizadorLexico.h"
#include"Divisor.h"
#include"AnalizadorSemantico.h"
#include"CRObjetos.h"
#include<fstream>

int main(int argc,char** argv)
{
	AnalizadorLexico AL;
	Divisor D;
	AnalizadorSemantico AS;
	int error = 0;

	obj.push_back(new Objeto("0", "var1"));

	string archivodir;
	string texto;

	for (int i = 1; i < argc; i++)
	{
		archivodir += argv[i];
	}

	archivodir += ".txt";

	ifstream archivo(archivodir);

	if (archivo.is_open()) {

		while(getline(archivo,texto) )
		{

			AL.SetTexto(texto);

			if (AL.Proceso())
			{
				OutputDebugString("\nproceso completado con exito\n");

				D.SetText(AL.GetText());
			}
			else
			{
				cout << "\nError al analizar\n";
				error++;
			}
			if (D.Dividir())
			{
				OutputDebugString("\nDivision completada\n");
				D.Obtener(AS._variable, AS._impresion, AS._igualdad, AS._tipo);
				AS._variables = D.VARAOPE();
				AS._operadores = D.OPERADORES();
				AS.Inicializar();
			}
			else
			{
				cout << "\nError division\n";
				error++;
			}

			if (AS.Dividir())
			{
				OutputDebugString("\nReglas escritas correctamente\n");

			}
			else
			{
				cout << "\nError en las reglas\n";
				error++;
			}

			AL.LimpiarVariables();
			D.Limpiar();
			AS.Limpiar();
		}

	}
	else
	{
		cout << archivodir << " no existe\n";
	}

	BOBJ.Borrar();


	return 0;
}