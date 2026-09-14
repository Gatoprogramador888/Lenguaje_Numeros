#include "Interpretar.h"

#include <cctype>
#include <iostream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

/*
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
        if (/*pos_dest != SIZE_MAX && *//* obj[id_variable]->GetType() == Tipos::ENTERO)
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
/*

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
}*/


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

void Interpretar::Declarar()
{
    uint64_t id_var = leer_u64(pc);
    uint64_t igualdad = leer_u64(pc);
    uint8_t tipo = leer_u8(pc);
    uint64_t posicion_valor = igualdad & ~(BIT_CONSTANTE | BIT_STRING); // Limpiar máscaras

    Informacion_Variable var;
    var.id = id_var;
    var.valor = tabla_CED[posicion_valor];
    var.tipo = static_cast<Tipos>(tipo);
    administrador.Crear(var);
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
            std::cout << obj[valor_puro]->GetValor();
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
	ASI.Verificar_Peticion(input, obj[id_var]->GetType());
	administrador.NuevaIgualdad(id_var, input);
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

    std::string  valor_puro_src1 = es_constante_src1 ? tabla_CED[posicion_puro_src1] : obj[posicion_puro_src1]->GetValor();
	std::string valor_puro_src2 = es_constante_src2 ? tabla_CED[posicion_puro_src2] : obj[posicion_puro_src2]->GetValor();

    InfDinamico resultado{};

	switch (opcode)
	{
	case static_cast<uint8_t>(OpCode::ADD):
		resultado = OperacionDinamico(valor_puro_src1, valor_puro_src2, '+');
		break;
	case static_cast<uint8_t>(OpCode::SUB):
		resultado = OperacionDinamico(valor_puro_src1, valor_puro_src2, '-');
		break;
	case static_cast<uint8_t>(OpCode::MUL):
		resultado = OperacionDinamico(valor_puro_src1, valor_puro_src2, '*');
		break;
	case static_cast<uint8_t>(OpCode::DIV):
		resultado = OperacionDinamico(valor_puro_src1, valor_puro_src2, '/');
		break;
	default:
		std::cerr << "Opcode de operación no reconocido: 0x" << std::hex << static_cast<int>(opcode) << std::dec << "\n";
		return;
	}
	administrador.NuevaIgualdad(dest, resultado.esDecimal() ? resultado.operator InfDec().str() : resultado.operator InfInt().str());

}
