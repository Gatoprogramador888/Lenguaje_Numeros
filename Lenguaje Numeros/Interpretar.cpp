#include "Interpretar.h"

#include <cctype>
#include <iostream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

// ── SetText ───────────────────────────────────────────────────────────────────

void Interpretar::SetText(const std::string& texto)
{
    this->Texto = texto;
    Divisor();
}

// ── Operaciones aritméticas ───────────────────────────────────────────────────
//
// BUG FIX #1: las cuatro funciones devolvían char* al buffer interno de un
// objeto temporal (Suma, Resta, etc.).  Ese objeto se destruía al salir de la
// función y el puntero quedaba colgante → UB al desreferenciar.
//
// Corrección: crear el objeto, copiar Resultado() (const std::string&) a un
// std::string local y devolverlo por valor.  El objeto temporal ya puede
// destruirse de forma segura.

std::string Interpretar::Sumar(listnum n1, listnum n2)
{
    Suma suma(n1, n2);
    suma.Calculate();
    return suma.GetResultString();          // copia la const std::string& a std::string
}

std::string Interpretar::Restar(listnum n1, listnum n2)
{
    Resta resta(n1, n2);
    resta.Calculate();
    return resta.GetResultString();
}

std::string Interpretar::Multiplicar(listnum n1, listnum n2)
{
    Multiplicacion multiplicacion(n1, n2);
    multiplicacion.Calculate();
    return multiplicacion.GetResultString();
}

std::string Interpretar::Dividir(listnum n1, listnum n2)
{
    Division division(n1, n2);
    division.Calculate();
    return division.GetResultString();
}

InfInt Interpretar::OperacionInt(std::string_view n1, std::string_view n2, char op)
{
    switch (op)
    {
    case '+': return InfInt(n1) + InfInt(n2); break;
	case '-': return InfInt(n1) - InfInt(n2); break;
	case '*': return InfInt(n1) * InfInt(n2); break;
	case '/': return InfInt(n1) / InfInt(n2); break;
    }
}

InfDec Interpretar::OperacionDec(std::string_view n1, std::string_view n2, char op)
{
    switch (op)
    {
    case '+': return InfDec(n1) + InfDec(n2); break;
    case '-': return InfDec(n1) - InfDec(n2); break;
    case '*': return InfDec(n1) * InfDec(n2); break;
    case '/': return InfDec(n1) / InfDec(n2); break;
    }
}

InfDinamico Interpretar::OperacionDinamico(std::string_view n1, std::string_view n2, char op)
{
    switch (op)
    {
    case '+': return InfDinamico(n1) + InfDinamico(n2); break;
    case '-': return InfDinamico(n1) - InfDinamico(n2); break;
    case '*': return InfDinamico(n1) * InfDinamico(n2); break;
    case '/': return InfDinamico(n1) / InfDinamico(n2); break;
    }
}



// ── Calcular ──────────────────────────────────────────────────────────────────

void Interpretar::Calcular(size_t& cantidad)
{
    if (Texto == "%%")
    {
        std::string result = resultado;

        //const size_t pos_dest = administrador.PosOBj(Variable);
        if (/*pos_dest != SIZE_MAX && */ obj[id_variable]->GetType() == Tipos::ENTERO)
        {
            size_t punto = result.find('.');
            if (punto != std::string::npos)
                result = result.substr(0, punto);
        }

        administrador.NuevaIgualdad(Variable, result);

        Variable = "";
        id_variable = 0;
        segunda_variable = "";
		id_segunda_variable = 0;
        operador = "";
        resultado = "";
        var_conver1 = "";
        cantidad = 0;
        return;
    }

    const bool es_operador = (Texto == "+" || Texto == "-" || Texto == "*" || Texto == "/");

    if (es_operador)
    {
        operador = Texto;
    }
    else
    {
        const bool es_alfa = std::isalpha(static_cast<unsigned char>(Texto[0])) != 0;
        const bool es_alnum = std::isalnum(static_cast<unsigned char>(Texto[0])) != 0
            || Texto[0] == '-';

        if (es_alfa && cantidad != 1)
        {
            if (cantidad > 2)
            {
                segunda_variable = obj[administrador.PosOBj(Texto)]->GetValor();
            }
            else
            {
                resultado = obj[administrador.PosOBj(Texto)]->GetValor();
            }
        }
        else if (es_alnum && !resultado.empty() && cantidad > 2)
        {
            segunda_variable = Texto;
			id_segunda_variable = std::stoi(Texto);
        }
        else if (es_alnum && cantidad == 2)
        {
            resultado = Texto;
        }
        else if (es_alnum && cantidad == 1)
        {
            Variable = Texto;
            id_variable = std::stoi(Texto);
        }
    }

    if (!segunda_variable.empty())
    {
        /*const size_t pos_dest = administrador.PosOBj(Variable);

        if (pos_dest == SIZE_MAX)
        {
			std::string error_msg = "Variable '" + Variable + "' no encontrada.";
            throw std::runtime_error(error_msg.c_str());
        }*/


        InfDinamico resultInfDinamico = OperacionDinamico(resultado, segunda_variable, operador[0]);
        var_conver1 = resultInfDinamico.esDecimal() ? resultInfDinamico.operator InfDec().str() : resultInfDinamico.operator InfInt().str();

        segunda_variable = "";
        resultado = var_conver1;
        operador = "";
        var_conver1 = "";
    }

    cantidad++;
}

// ── Imprimir ──────────────────────────────────────────────────────────────────

void Interpretar::Imprimir()
{
    if (Texto != ">>")
    {
        if (Texto.find("{") != std::string::npos)
        {
            std::string variable;
            for (size_t i = 2; i < Texto.length(); i++)
                variable += Texto[i];
            size_t posVariable = std::stoi(variable);
            std::cout << obj[posVariable]->GetValor();
        }
        else
        {
            std::cout << Texto;
        }
    }
    else
    {
        std::cout << std::endl;
        estado = Estados::NINGUNO;
    }
}

// ── Divisor ───────────────────────────────────────────────────────────────────

void Interpretar::Divisor()
{
    if (Texto == ">") { estado = Estados::IMPRIMIR;  return; }
    if (Texto == "<") { estado = Estados::PEDIR;     return; }
    if (Texto == "%") { estado = Estados::OPERACION; return; }
    if (Texto == "!") { estado = Estados::DECLARAR; return; }

    switch (estado)
    {
    case Estados::IMPRIMIR:  Imprimir();        break;
    case Estados::PEDIR:     Peticion();        break;
    case Estados::OPERACION: Calcular(cantidad); break;
    case Estados::DECLARAR: Declarar();         break;
    default:                                    break;
    }
}

// ── Peticion ──────────────────────────────────────────────────────────────────

void Interpretar::Peticion()
{
    if (Texto != "<<")
    {
		size_t id_variable = std::stoi(Texto);
        Analizador_Semantico_Interpretacion ASI;
        std::string nuevaigualdad;
        std::cin >> nuevaigualdad;
        ASI.Verificar_Peticion(nuevaigualdad, obj[id_variable]->GetType());
        administrador.NuevaIgualdad(id_variable, nuevaigualdad);
    }
    else
    {
        estado = Estados::NINGUNO;
    }
}
// 

void Interpretar::Declarar()
{
    if (Texto != ">>")
    {
        // formato: "nombre valor"
        size_t espacio = Texto.find(' ');
		size_t tipo = Texto.find_last_of("idm");
        if (espacio != std::string::npos && tipo != std::string::npos)
        {
            Informacion_Variable var;
            var.nombre = Texto.substr(0, espacio);
            var.valor = Texto.substr(espacio + 1, tipo - espacio - 1);
            
			switch (Texto[tipo])
			{
			case 'i': var.tipo = Tipos::ENTERO; break;
			case 'd': var.tipo = Tipos::DECIMAL; break;
			case 'm': var.tipo = Tipos::DINAMICO; break;
			}
            administrador.Crear(var);
        }
        else
        {
			throw std::runtime_error("Formato de declaración inválido. Se esperaba 'nombre valor tipo'.");
        }
    }
    else
    {
        estado = Estados::NINGUNO;
    }
}