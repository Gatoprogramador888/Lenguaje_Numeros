#include "Interpretar.h"

#include <cctype>
#include <iostream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <algorithm>

// ── Cargar Archivo .crb ───────────────────────────────────────────────

bool Interpretar::CargarArchivoCRB(const std::string& ruta_archivo)
{
    std::ifstream arq(ruta_archivo, std::ios::binary);
    if (!arq.is_open()) {
        std::cerr << "Error al abrir el archivo compilado: " << ruta_archivo << "\n";
        return false;
    }

    // 1. Validar Cabecera Magic Bytes "CRB\0"
    char header_magic[4];
    arq.read(header_magic, 4);
    if (std::memcmp(header_magic, magic, 4) != 0) {
        std::cerr << "El archivo no es un ejecutable CRB valido.\n";
        return false;
    }

    // 2. Leer Tabla de Posiciones PCT
    uint64_t pos_CED = 0, pos_CS = 0, pos_IC = 0;
    arq.read(reinterpret_cast<char*>(&pos_CED), sizeof(pos_CED));
    arq.read(reinterpret_cast<char*>(&pos_CS), sizeof(pos_CS));
    arq.read(reinterpret_cast<char*>(&pos_IC), sizeof(pos_IC));

    // 3. Leer Sección CED (Constantes Numéricas)
    arq.seekg(pos_CED, std::ios::beg);
    uint64_t cant_CED = 0;
    arq.read(reinterpret_cast<char*>(&cant_CED), sizeof(cant_CED));
    tabla_CED.resize(cant_CED);
    for (uint64_t i = 0; i < cant_CED; ++i) {
        uint64_t len = 0;
        arq.read(reinterpret_cast<char*>(&len), sizeof(len));
        tabla_CED[i].resize(len);
        arq.read(&tabla_CED[i][0], len);
    }

    // 4. Leer Sección CS (Strings)
    arq.seekg(pos_CS, std::ios::beg);
    uint64_t cant_CS = 0;
    arq.read(reinterpret_cast<char*>(&cant_CS), sizeof(cant_CS));
    tabla_CS.resize(cant_CS);
    for (uint64_t i = 0; i < cant_CS; ++i) {
        uint64_t len = 0;
        arq.read(reinterpret_cast<char*>(&len), sizeof(len));
        tabla_CS[i].resize(len);
        arq.read(&tabla_CS[i][0], len);
    }

    // 5. Leer Sección IC (Bytecode)
    arq.seekg(pos_IC, std::ios::beg);
    arq.seekg(0, std::ios::end);
    size_t tam_tam_ic = static_cast<size_t>(arq.tellg()) - pos_IC;

    bytecode.resize(tam_tam_ic);
    arq.seekg(pos_IC, std::ios::beg);
    arq.read(reinterpret_cast<char*>(bytecode.data()), tam_tam_ic);

    arq.close();
    return true;
}

// ── Bucle Principal de Ejecución (Virtual Machine) ─────────────────────

void Interpretar::Ejecutar()
{
    while (pc < bytecode.size())
    {
        uint8_t opcode = leer_u8(pc);

        switch (static_cast<OpCode>(opcode))
        {
        case OpCode::DECLARAR:
        {
            Declarar();
            break;
        }

        case OpCode::INPUT:
        {
            Peticion();
            break;
        }

        case OpCode::PRINT:
        {
            Imprimir();
            break;
        }

        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        {
            Operacion(opcode);
            break;
        }

		case OpCode::FREE:
		{
            Liberar();
			break;
		}

		case OpCode::NOP:
		{
			break;
		}

        case OpCode::HALT:
            return;

        default:
            std::cerr << "Opcode no reconocido: 0x" << std::hex << static_cast<int>(opcode)
                << " en la posicion " << std::dec << (pc - 1) << "\n";
            return;
        }
    }
}

// Helper para extraer un uint64_t y avanzar pc 8 bytes
uint64_t Interpretar::leer_u64(size_t& pos)
{
    uint64_t valor;
    std::memcpy(&valor, &bytecode[pos], sizeof(uint64_t));
    pos += sizeof(uint64_t);
    return valor;
}

uint8_t Interpretar::leer_u8(size_t& pos)
{
    return bytecode[pos++];
}


InfDinamico Interpretar::OperacionDinamico(InfDinamico n1, InfDinamico n2, char op)
{
    switch (op)
    {
    case '+': return n1 + n2; break;
    case '-': return n1 - n2; break;
    case '*': return n1 * n2; break;
    case '/': return n1 / n2; break;
    }
}


void Interpretar::Declarar()
{
    uint64_t id_var = leer_u64(pc);
    uint64_t igualdad = leer_u64(pc);
    uint8_t tipo = leer_u8(pc);
    uint64_t posicion_valor = igualdad & ~(BIT_CONSTANTE | BIT_STRING); // Limpiar máscaras
    bool es_constante_grande = (igualdad & BIT_CONSTANTE) != 0;

    std::string valor = es_constante_grande ? tabla_CED[posicion_valor] :
        std::to_string(igualdad & ~(BIT_STRING));


    Informacion_Variable var;
    var.id = id_var;
    var.valor = valor;
    var.tipo = static_cast<Tipos>(tipo);
    administrador.Alojar(id_var, var);
}

void Interpretar::Imprimir()
{
    uint64_t cantidad_operandos = leer_u64(pc);

    for (uint64_t i = 0; i < cantidad_operandos; ++i)
    {
        uint64_t operando = leer_u64(pc);

        // Determinar tipo de operando mediante máscaras de bits
        bool es_constante = (operando & BIT_CONSTANTE) != 0;
        bool es_string = (operando & BIT_STRING) != 0;
        uint64_t valor_puro = operando & ~(BIT_CONSTANTE | BIT_STRING);

        if (es_constante && es_string) {
            // String de tabla_CS
            std::cout <<  tabla_CS[valor_puro];
        }
        else if (es_constante) {
            // Constante de tabla_CED
            std::cout << tabla_CED[valor_puro];
        }
        else if (es_string) {
            // Entero Inline
            std::cout << tabla_CED[valor_puro];
        }
        else {
            // Variable
            std::cout << administrador.Obtener(valor_puro)->GetValorStr();
        }
    }
    std::cout << "\n";
}

void Interpretar::Peticion()
{
    uint64_t id_codificado = leer_u64(pc);
    uint64_t id_var = id_codificado & ~(BIT_CONSTANTE | BIT_STRING); // Limpiar máscaras
    std::string input;
	std::cin >> input;
	Analizador_Semantico_Interpretacion ASI;
	ASI.Verificar_Peticion(input, administrador.Obtener(id_var)->GetType());

	administrador.Actualizar(id_var, InfDinamico(input));
}

void Interpretar::Operacion(uint8_t opcode)
{
    uint64_t dest = leer_u64(pc);
    uint64_t src1 = leer_u64(pc);
    uint64_t src2 = leer_u64(pc);

    bool es_constante_src1 = (src1 & BIT_CONSTANTE) != 0;
    bool es_constante_src2 = (src2 & BIT_CONSTANTE) != 0;
    uint64_t posicion_puro_src1 = src1 & ~(BIT_CONSTANTE | BIT_STRING);
    uint64_t posicion_puro_src2 = src2 & ~(BIT_CONSTANTE | BIT_STRING);

    InfDinamico vp1 = es_constante_src1
        ? InfDinamico(tabla_CED[posicion_puro_src1])
        : administrador.Obtener(posicion_puro_src1)->ObtenerComoDinamico();

    InfDinamico vp2 = es_constante_src2
        ? InfDinamico(tabla_CED[posicion_puro_src2])
        : administrador.Obtener(posicion_puro_src2)->ObtenerComoDinamico();

    InfDinamico resultado{};

	switch (opcode)
	{
	case static_cast<uint8_t>(OpCode::ADD):
		resultado = OperacionDinamico(vp1, vp2, '+');
		break;
	case static_cast<uint8_t>(OpCode::SUB):
		resultado = OperacionDinamico(vp1, vp2, '-');
		break;
	case static_cast<uint8_t>(OpCode::MUL):
		resultado = OperacionDinamico(vp1, vp2, '*');
		break;
	case static_cast<uint8_t>(OpCode::DIV):
		resultado = OperacionDinamico(vp1, vp2, '/');
		break;
	default:
		std::cerr << "Opcode de operación no reconocido: 0x" << std::hex << static_cast<int>(opcode) << std::dec << "\n";
		return;
	}
	administrador.Actualizar(dest, resultado);

}

void Interpretar::Liberar()
{
	uint64_t id_var = leer_u64(pc);
	administrador.Liberar(id_var);
}
