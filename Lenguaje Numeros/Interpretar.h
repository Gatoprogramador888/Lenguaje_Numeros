#pragma once
#include "Dividir.h"
#include "Multiplicar.h"
#include "Restar.h"
#include "Sumar.h"
#include "Conversor.h"

#include "InfDec.h"
#include "InfInt.h"
#include "InfDinamico.h"

#include "AnalizadorSemantico.h"

#include <string>
#include <vector>

// listnum es vector<int> — definido en la librería de operaciones
typedef std::vector<int> listnum;

class Interpretar
{
private:
    // BUG FIX #1: retorno cambiado de char* a std::string.
    // El original devolvía char* al buffer interno de un objeto temporal;
    // ese objeto se destruía al salir de la función → puntero colgante (UB).
    // Ahora se llama const std::string& Resultado() y se copia a std::string.
    [[deprecated("Ahora se usa InfDec o InfInt o InfDinamico.")]]
    std::string Sumar(listnum n1, listnum n2);
    [[deprecated("Ahora se usa InfDec o InfInt o InfDinamico.")]]
    std::string Restar(listnum n1, listnum n2);
    [[deprecated("Ahora se usa InfDec o InfInt o InfDinamico.")]]
    std::string Multiplicar(listnum n1, listnum n2);
    [[deprecated("Ahora se usa InfDec o InfInt o InfDinamico.")]]
    std::string Dividir(listnum n1, listnum n2);

	InfInt OperacionInt(std::string_view n1, std::string_view n2, char op);

	InfDec OperacionDec(std::string_view n1, std::string_view n2, char op);

	InfDinamico OperacionDinamico(std::string_view n1, std::string_view n2, char op);



    void Calcular(size_t& cantidad);
    void Imprimir();
    void Divisor();
    void Peticion();

    void Declarar();

    // ── estado de cálculo ─────────────────────────────────────────────────────
    std::string Variable = "";
    std::string segunda_variable = "";
    std::string operador = "";
    std::string resultado = "";
    // BUG FIX #1 contd.: var_conver1 era char*; ahora es std::string.
    std::string var_conver1;

    listnum    var1, var2;
    ConversionI conversion;

    std::string Texto;
    enum class Estados { OPERACION, IMPRIMIR, PEDIR, DECLARAR, NINGUNO };
    Estados estado = Estados::NINGUNO;
    size_t  cantidad = 1;
    bool    accion = false;

public:
    void SetText(const std::string& texto);

    // BUG FIX #3: el destructor original solo ponía cantidad = 0, sin liberar
    // ningún recurso real. Se deja vacío — el compilador genera el correcto.
    ~Interpretar() = default;
};