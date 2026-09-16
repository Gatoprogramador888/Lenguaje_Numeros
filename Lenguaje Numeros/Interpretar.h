#pragma once
#include "Dividir.h"
#include "Multiplicar.h"
#include "Restar.h"
#include "Sumar.h"
#include "Conversor.h"

#include "InfDec.h"
#include "InfInt.h"
#include "InfDinamico.h"
#include <variant>

#include "AnalizadorSemantico.h"

#include <string>
#include <vector>


class Interpretar
{
private:
    // Tablas de datos cargadas desde el archivo .crb
    std::vector<std::string> tabla_CED; // Constantes numéricas
    std::vector<std::string> tabla_CS;  // Constantes de texto (strings)
    std::vector<uint8_t>     bytecode;  // Código de instrucciones (IC)

    size_t pc{ 0 }; // Program Counter

    // Helper para extraer un uint64_t y avanzar pc 8 bytes
	uint64_t leer_u64(size_t& pos);

    // Helper para extraer un uint8_t y avanzar pc 1 byte
    uint8_t leer_u8(size_t& pos);

    InfDinamico OperacionDinamico(InfDinamico n1, InfDinamico n2, char op);

	void Declarar();

    void Imprimir();

	void Peticion();

    void Operacion(uint8_t opcode);

    void Liberar();

public:
    Interpretar() = default;

    // Carga las tablas PCT, CED, CS y el Bytecode desde el archivo binario .crb
    bool CargarArchivoCRB(const std::string& ruta_archivo);

    // Ciclo principal de ejecución (Bucle de la Máquina Virtual)
    void Ejecutar();

    ~Interpretar() = default;
};