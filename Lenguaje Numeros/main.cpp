#include"Comandos_App.h"

/*HACER QUE SE PASE TODO A UN NUEVO ARCHIVO*/
/*MODIFICAR INTERPRETE*/
/*HACER UN SISTEMA DE RECIBIR ARCHIVOS Y CREAR ARCHIVOS*/
/*HACER SISTEMA DE AYUDA*/


int main(int argc, char** argv)
{	
	Comandos_App CA(argc,argv);

	CA.~Comandos_App();

	return 0;
}