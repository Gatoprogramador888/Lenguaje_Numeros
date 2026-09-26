# Lenguaje Números ( CRD )

Un lenguaje de programación de propósito numérico, compilado e interpretado, escrito en C++. Diseñado para declarar variables, realizar operaciones matemáticas y lógicas, y ejecutar programas desde la línea de comandos.

---

## Características

- **Tipos de datos**: `int`, `decimal`, `bool`, `dynamic`
- **Constantes** con la palabra clave `const`
- **Aritmética completa**: `+`, `-`, `*`, `/`, `++`, `+=` y expresiones anidadas con paréntesis (algoritmo Shunting Yard)
- **Lógica booleana**: `and`, `or`, `not`, `==`, `!=`, `>`, `<`, `>=`, `<=`
- **Entrada/salida**: `print` e `input`
- **Interpolación de cadenas**: `$ "El valor es: {variable}"`
- **Valor nulo**: asignación de `null` para liberar variables dinámicas
- **Compilación a bytecode** en formato binario `.crb`
- **Interpretación directa** del bytecode generado

---

## Estructura del proyecto

```
Lenguaje Numeros/
├── main.cpp                  # Punto de entrada
├── Comandos_App.cpp/.h       # CLI: compilar, interpretar, ayuda
├── AnalizadorLexico.cpp/.h   # Análisis léxico
├── Tokens.cpp/.h             # Tokenizador
├── AnalizadorSemantico.cpp/.h# Análisis semántico
├── Divisor.cpp/.h            # División de instrucciones
├── Interpretar.cpp/.h        # Intérprete de bytecode
├── CRObjetos.cpp/.h          # Objetos en memoria y Garbage Collector
├── Informacion.h             # Tipos, tokens, opcodes y estructuras base
├── prueba.crd                # Programa de ejemplo
└── a.crb                     # Ejemplo de bytecode compilado
```

---

## Requisitos

- **Compilador**: MSVC (Visual Studio 2022 recomendado)
- **Plataforma**: Windows x64
- **Dependencias**: [Boost.Multiprecision](https://www.boost.org/) (instalado vía vcpkg)

### Instalar dependencias con vcpkg

```bash
vcpkg install boost-multiprecision:x64-windows
```

---

## Compilar el proyecto

Abre `Lenguaje Numeros.vcxproj` en Visual Studio y compila en modo **Release x64**, o usa MSBuild:

```bash
msbuild "Lenguaje Numeros.vcxproj" /p:Configuration=Release /p:Platform=x64
```

---

## Uso

El ejecutable acepta comandos desde la línea de comandos.

### Ayuda

```bash
crd help commands
crd help compile
crd help syntax
crd help interpret
```

### Compilar un programa

Convierte un archivo fuente `.crd` en bytecode `.crb`:

```bash
crd -c <source_file.crd> <output_file.crb>
```

### Interpretar bytecode

Ejecuta un archivo `.crb` ya compilado:

```bash
crd -i <compiled_file.crb>
```

---

## Sintaxis del lenguaje

### Declaración de variables

```
int : contador = 5, limite = 20;
decimal : precio = 9.99, total = 0.0;
bool : activo = true;
dynamic : estado = 1;
const decimal : PI = 3.1416;
```

### Imprimir

```
print : $ "Hola mundo";
print : $ "El contador es: {contador}";
print : contador;
```

### Pedir entrada

```
input : limite;
```

### Operaciones

```
operation : contador++;
operation : contador += 10;
operation : total = precio * 2 - descuento;
operation : resultado = ((a + b) * (c - d)) / e;
operation : estado = null;
```

### Booleanos y lógica

```
bool : es_mayor = contador > 10;
bool : en_rango = contador > 5 and contador < 100;
bool : iguales = a == b;
operation : es_mayor = not true;
```

### Comentarios

```
# Esto es un comentario
```

---

## Ejemplo completo

El archivo `prueba.crd` incluido en el proyecto demuestra:

- Declaración de constantes y múltiples variables
- Cálculo del área de un círculo con entrada del usuario
- Cinemática: `posicion = pos_inicial + (vel_inicial * tiempo) + ((aceleracion * (tiempo * tiempo)) / 2.0)`
- Volumen de un cono truncado
- Expresiones algebraicas anidadas con enteros
- Operadores de comparación y lógicos
- Asignación de `null` a variables dinámicas

Para ejecutarlo:


---

## Arquitectura interna

| Componente | Responsabilidad |
|---|---|
| `AnalizadorLexico` | Divide el texto fuente en tokens |
| `Tokenizador` | Clasifica tokens con tipo y posición |
| `AnalizadorSemantico` | Valida tipos, constantes y genera bytecode |
| `Divisor` | Separa instrucciones del flujo de entrada |
| `Interpretar` | Ejecuta los opcodes del bytecode `.crb` |
| `TablaSimbolos` | Registra variables con ID, tipo y constancia |
| `GC` | Garbage Collector: aloja, actualiza y libera objetos en un pool |

### Formato de bytecode (`.crb`)

Los archivos compilados comienzan con el magic `CRB\0` y contienen instrucciones de 1 byte (`OpCode`) seguidas de operandos de 64 bits. Los bits más significativos del operando codifican si el valor es una variable, constante, string, o nulo.

---

## Licencia

Este proyecto no incluye licencia explícita. Todos los derechos reservados al autor.