# FreeHand, An Automated Memory Bug Detector
FreeHand is an automated dynamic memory bug detector that focuses on detection
of Use-After-Free (UAF) and Double-Free (DF) bugs within single-threaded C
programs. FreeHand uses LLVM to traverse intra-procedural control-flow graphs and 
identify potential bugs.

## Requirements
| Requirement | Version  |
|-------------|----------|
| C++         | \>=20    |
| Boost       | \>=1.8.0 |
| LLVM        | \>=14.0  |
| Clang(++)   | \>=14.0  |
| CMake       | \>=3.22  |

## Building
FreeHand requires Clang, LLVM, libBoost to compile and uses CMake as a build system.
Building FreeHand on Windows machines is untested and difficult due to the
complexity of installing LLVM on Windows. Clang is also used to generate LLVM
bytecode for testing/using FreeHand. Ninja is recommended as a build tool.

### Installing Requirements
For Ubuntu-based Linux systems:\
`sudo apt install -y llvm-14 libboost-all-dev CMake clang-14 ninja-build`

### Building with CMake
For Ubuntu-based Linux systems:\
`cmake -B build -G Ninja .`

## Using FreeHand
Usage: `freehand [options] bytecode_file`\
Options:\
`-v [ --version ]` Print a version string\
`-h [ --help ]` Produce a help message\
`-f [ --function ] arg` Specific functions to analyze\
`--bytecode_file arg`   Bytecode file to analyze.\