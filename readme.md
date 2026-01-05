<div align="center">

# Incanti

![logo](assets/logo.gif)

---

**Incanti** – A single-header C++ library for simple and elegant command line argument parsing.

</div>

Just as a sorcerer meticulously prepares incantations to manifest a spell, **Incanti** helps you structure the commands that bring your software to life. It transforms the chaotic ritual of `argc` and `argv` into a clean, expressive declaration of intent. Simply drop in `incanti.hpp`, define your options, and let Incanti handle the rest without any external dependencies.

## Features

- Single header, no dependencies
- Type-safe argument parsing (strings, numbers, booleans, etc.)
- Short (`-v`) and long (`--verbose`) option support
- Required and optional arguments with default values
- Custom converters and validators
- Automatic help generation
- Positional argument handling
- Combined short flags (`-vdf`)

## Quick Start

```cpp
#include "incanti.hpp"

int main(int argc, char *argv[]) {
    Incanti::Parser parser("program", "program to test incanti");

    std::string input;
    int threads = 4;
    bool verbose = false;

    // Define arguments with fluent interface
    parser.add("input", "i", &input)
        .help("Input file path")
        .required();

    parser.add("threads", "t", &threads)
        .help("Number of threads")
        .default_value(4);

    parser.flag("verbose", "v", &verbose)
        .help("Enable verbose output");

    try {
        parser.parse(argc, argv);

        // Your application logic here
        std::cout << "Processing " << input
                  << " with " << threads << " threads\n";

    } catch (const Incanti::ParseError &e) {
        std::cerr << "Error: " << e.what() << "\n";
        parser.print_help();
        return 1;
    }

    return 0;
}
```

Run with:
```bash
./myapp --input data.txt --threads 8 -v
./myapp -i data.txt -t 8 -v
./myapp -i data.txt -vt8  # the vanilla combined flags with attached value
```

## Architecture

Incanti follows a clean object-oriented design with polymorphic argument handling:

<div align="center">

![Incanti Architecture](assets/incanti_lib.webp)

</div>

**Key Components:**

- **`Argument`** – Abstract base class defining the interface for all argument types
- **`TypedArgument<T>`** – Template class handling typed arguments (string, int, float, double, long, bool) with optional custom converters and validators
- **`FlagArgument`** – Specialized class for boolean flags that don't require values
- **`Parser`** – Main orchestrator that manages argument registration, parsing, and help generation
- **`ParseError`** – Exception class for parsing errors

The design allows for easy extension: add new argument types by inheriting from `Argument`, or customize existing types with converters and validators.

## Supported Types

Out of the box, Incanti supports:

- `std::string` – Text values
- `int` – Integer numbers
- `float` – Single-precision floating point
- `double` – Double-precision floating point
- `long` – Long integers
- `bool` – Boolean flags (with "true/false", "1/0", "yes/no")

## Usage Examples

### Basic Arguments

```cpp
std::string name;
parser.add("name", "n", &name).help("Your name");

// Usage: ./app --name Alice
// Usage: ./app -n Alice
```

### Required Arguments

```cpp
std::string input;
parser.add("input", "i", &input)
    .help("Input file")
    .required();

// Usage: ./app --input data.txt
```

### Default Values

```cpp
int port = 0;
parser.add("port", "p", &port)
    .help("Server port")
    .default_value(8080);

// Usage: ./app                    (port = 8080)
// Usage: ./app --port 3000        (port = 3000)
```

### Boolean Flags

```cpp
bool verbose = false;
parser.flag("verbose", "v", &verbose)
    .help("Enable verbose output");

// Usage: ./app -v
// Usage: ./app --verbose
```

### Combined Flags

```cpp
bool verbose = false, debug = false, force = false;
parser.flag("verbose", "v", &verbose);
parser.flag("debug", "d", &debug);
parser.flag("force", "f", &force);

// Usage: ./app -vdf    (all three flags set)
```

### Custom Converters

```cpp
std::string mode;
parser.add("mode", "m", &mode)
    .help("Processing mode")
    .converter([](const std::string &s) {
        std::string upper = s;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        if (upper != "FAST" && upper != "SLOW" && upper != "BALANCED") {
            throw Incanti::ParseError("Mode must be fast, slow, or balanced");
        }
        return upper;
    });

// Usage: ./app --mode fast        (converts to "FAST")
// Usage: ./app --mode FaSt        (converts to "FAST")
```

### Custom Validators

```cpp
double threshold = 0.0;
parser.add("threshold", &threshold)
    .help("Confidence threshold (0.0-1.0)")
    .converter([](const std::string &s) {
        double val = std::stod(s);
        if (val < 0.0 || val > 1.0) {
            throw Incanti::ParseError("Threshold must be between 0.0 and 1.0");
        }
        return val;
    });
```

### Positional Arguments

```cpp
parser.parse(argc, argv);

const auto &positional = parser.positional();
for (const auto &arg : positional) {
    std::cout << "Positional: " << arg << "\n";
}

// Usage: ./app -i input.txt file1.txt file2.txt file3.txt
// Positional: file1.txt
// Positional: file2.txt
// Positional: file3.txt
```

## Command Line Syntax

Incanti supports multiple syntax styles:

```bash
# Long options with space
./app --input data.txt --threads 4

# Long options with equals
./app --input=data.txt --threads=4

# Short options with space
./app -i data.txt -t 4

# Short options with attached value
./app -idata.txt -t4

# Combined short flags
./app -vdf

# Combined flags with value (last flag gets the value)
./app -vdt4  # -v, -d enabled, -t gets value 4

# Mix and match
./app -i data.txt --threads=8 -vdf
```

## Automatic Help Generation

Incanti automatically generates help messages:

```cpp
Incanti::Parser parser("myapp", "Process data files efficiently");

parser.add("input", "i", &input).help("Input file").required();
parser.add("threads", "t", &threads).help("Worker threads").default_value(4);
parser.flag("verbose", "v", &verbose).help("Verbose output");
```

Running `./myapp --help` or `./myapp -h` displays:

```
Usage: myapp [options]
Process data files efficiently

Options:
-h, --help
   Show this help message
-i, --input <value>
   Input file [required]
-t, --threads <value>
   Worker threads (default: 4)
-v, --verbose
   Verbose output
```

## Try It Out

1. Clone the repository
```bash
git clone https://github.com/yourusername/incanti.git
cd incanti
```

2. Build the demo
```bash
mkdir build && cd build
cmake ..
make
```

3. Run the demo
```bash
./build/bin/Release/incanti --help
./build/bin/Release/incanti -i test.txt -vdf
./build/bin/Release/incanti --input=data.txt --mode fast --threads 8
```

## Building in Your Project

Just include the header:

```cpp
#include "incanti.hpp"
```

No compilation or installation needed. Requires **C++17** or later.

### Using CMake

```cmake
# Copy incanti.hpp to your project
target_include_directories(your_target PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Or add as subdirectory if you include the whole repo
add_subdirectory(external/incanti)
target_link_libraries(your_target PRIVATE incanti)
```

### Manual Compilation

```bash
g++ -std=c++17 -I/path/to/incanti your_app.cpp -o your_app
clang++ -std=c++17 -I/path/to/incanti your_app.cpp -o your_app
```

## Error Handling

All parsing errors throw `Incanti::ParseError`:

```cpp
try {
    parser.parse(argc, argv);
} catch (const Incanti::ParseError &e) {
    std::cerr << "Error: " << e.what() << "\n";
    parser.print_help();
    return 1;
}
```

Common errors:
- Missing required arguments
- Invalid values for types (e.g., "abc" for an integer)
- Unknown arguments
- Validation failures from custom converters
- Duplicate argument names

## Design Philosophy

Incanti embodies a few key principles:

**Simplicity** – Single header, zero dependencies, minimal API interface.

**Type Safety** – Leverage C++ templates for compile-time and runtime type checking.

**Fluency** – Chain methods to build expressive argument definitions.

**Extensibility** – Easy to add custom converters, validators, or new argument types.

This library was crafted as a personal project to explore modern C++ design patterns and to create something genuinely useful for CLI applications.

> It's not trying to be everything to everyone – just a clean, elegant solution for argument parsing.

## Status

**Work in Progress** – The API is stable for basic usage, but expect refinements and new features.

## License

[apache 2.0](license.md)

<div align="center">

*Transform your command line chaos into elegant incantations*

</div>
