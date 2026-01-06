### Basic Arguments

```cpp
std::string name;
parser >> arg("name", "n", &name) | "Your name";

// Usage: ./app --name Alice
// Usage: ./app -n Alice
```

### Required Arguments

```cpp
std::string input;
parser >> arg("input", "i", &input)
    | "Input file"
    | required();

// Usage: ./app --input data.txt
```

### Default Values

```cpp
int port = 0;
parser >> arg("port", "p", &port)
    | "Server port"
    | def(8080);

// Usage: ./app                    (port = 8080)
// Usage: ./app --port 3000        (port = 3000)
```

### Boolean Flags

```cpp
bool verbose = false;
parser >> flag("verbose", "v", &verbose)
    | "Enable verbose output";

// Usage: ./app -v
// Usage: ./app --verbose
```

### Combined Flags (implicit)

```cpp
bool verbose = false, debug = false, force = false;
parser >> flag("verbose", "v", &verbose);
parser >> flag("debug", "d", &debug);
parser >> flag("force", "f", &force);

// Usage: ./app -vdf    (all three flags set)
```

### Custom Converters

```cpp
std::string mode;
parser >> arg("mode", "m", &mode)
    | help("Processing mode")
    | [](const std::string &s) {
        std::string upper = s;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        if (upper != "FAST" && upper != "SLOW" && upper != "BALANCED") {
            throw Incanti::ParseError("Mode must be fast, slow, or balanced");
        }
        return upper;
    };

// Usage: ./app --mode fast        (converts to "FAST")
// Usage: ./app --mode FaSt        (converts to "FAST")
```

### Custom Validators

```cpp
double threshold = 0.0;
parser >> arg("threshold", &threshold)
    | help("Confidence threshold (0.0-1.0)")
    | [](const std::string &s) {
        double val = std::stod(s);
        if (val < 0.0 || val > 1.0) {
            throw Incanti::ParseError("Threshold must be between 0.0 and 1.0");
        }
        return val;
    };
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

parser >> arg("input", "i", &input) | "Input file" | required;
parser >> arg("threads", "t", &threads) | "Worker threads" | def(4);
parser >> flag("verbose", "v", &verbose) | "Verbose output";
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
