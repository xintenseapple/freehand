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

## Directory Overview

### Source Code

 - `freehand.cpp` : main function; pass the bytecode to MemoryAllocationAnalyzer.cpp for analysis
 - `MemoryAllocationAnalyzer.hpp`, `MemoryAllocationAnalyzer.cpp` : functions implementing algorithms for detecting any use after free cases or double free cases
 - `DF.hpp` : header file including functions used for detecting double free cases
 - `UAF.hpp` : header file including functions used for detecting use after free cases


### Test Cases

Under the folder called `tests`,
 - `conditionals` :  test case with if, else statement
 	- `correct` : correct version of the test case
	- `df` : version with double free case
	- `uaf` : version with use after free case
 - `loops` : test case with for loop; **needs fix as it cannot track different allocations called during iterations**
 	- `correct`
 	- `df`
 	- `uaf`
 - `simple` : test case with simple allocation 
 	- `correct`
	- `df`
	- `uaf`

### Generating Bytecode for The Testcases

Whithin the `tests` folder, run `make`

