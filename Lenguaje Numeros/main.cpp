#include"AnalizadorLexico.h"
#include"Divisor.h"
#include"AnalizadorSemantico.h"
#include"CRObjetos.h"
#include"Interpretar.h"
#include<fstream>

using namespace std;

int main(int argc,char** argv)
{
	AnalizadorLexico AL;
	Divisor D;
	AnalizadorSemantico AS;
	Interpretar I;

	obj.push_back(new Objeto("0", "var1"));

	string archivodir;
	string texto;
	string archivocon;
	int i2 = 2;
	bool errores = true;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-c") == 0)break;

		i2++;
		archivodir += argv[i];
	}

	for (int i = i2; i < argc; i++)
	{
		archivocon += argv[i];
	}

	archivodir += ".txt";
	archivocon += ".txt";

	ifstream archivo(archivodir);
	fstream archivo2(archivocon, ios::out);

	if (archivo.is_open()) {
		while(getline(archivo,texto) )
		{

			AL.SetTexto(texto);


			if (!AL.Proceso())
			{
				cout << "\nError al analizar\n";
				errores = true;
			}
			else
			{
				OutputDebugString("\nproceso completado con exito\n");
				D.SetText(AL.GetText());
			}
			if (!D.Dividir())
			{
				cout << "\nError division\n";
				errores = true;
			}
			else
			{
				OutputDebugString("\nDivision completada\n");
				D.Obtener(AS._variable, AS._impresion, AS._igualdad, AS._tipo);
				AS._variables = D.VARAOPE();
				AS._operadores = D.OPERADORES();
				AS.Inicializar();
			}

			if (!AS.Dividir(archivocon))
			{
				cout << "\nError en las reglas\n";
				errores = true;
			}
			else
			{
				OutputDebugString("\nReglas escritas correctamente\n");
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

	archivo2.close();

	ifstream ArchivoInterpretar(archivocon);
	string ContenidoInterpretar;

	while (getline(ArchivoInterpretar, ContenidoInterpretar))
	{
		I.SetText(ContenidoInterpretar);
	}

	ArchivoInterpretar.close();
	BOBJ.Borrar();

	return 0;
}