/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * FreeHand header
 */

#ifndef FREEHAND_FREEHAND_H
#define FREEHAND_FREEHAND_H

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

std::string VERSION("1.0.0");
std::set<std::string> ALLOC_FUNCTION_NAMES = {"malloc", "aligned_malloc",
                                              "calloc", "kmalloc", "kcalloc",
                                              "vmalloc", "kzalloc"};
std::set<std::string> FREE_FUNCTION_NAMES = {"free", "kfree"};

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class FreeHand {
public:
    explicit FreeHand(fs::path &bytecode_path);

    ~FreeHand() = default;

    void AnalyzeAllFunctions();
    void AnalyzeFunction(llvm::Function& function);
    void AnalyzeFunctionByName(std::string& function_name);

private:
    llvm::LLVMContext llvm_context;
    std::unique_ptr<llvm::Module> llvm_module;

    void printDF(llvm::Function &function, const llvm::DebugLoc &debugLoc);

    void printUAF(llvm::Function &function, const llvm::DebugLoc &debugLoc);
};

void print_usage(po::options_description &options) {
    std::cout << "Usage: freehand [options] bytecode_file" << std::endl;
    std::cout << options << std::endl;
}

int main(int argc, char **argv) {
    po::options_description options("Options");
    options.add_options()
            ("version,v", "Print a version string")
            ("help,h", "Produce a help message")
            ("bytecode_file", po::value<fs::path>(),
             "Bytecode file to analyze.");

    po::positional_options_description positional_options;
    positional_options.add("bytecode_file", 1);

    po::variables_map variablesMap;
    try {
        po::store(po::command_line_parser(argc, argv)
                          .options(options)
                          .positional(positional_options)
                          .run(),
                  variablesMap);
        po::notify(variablesMap);

    } catch (po::error &e) {
        std::cout << "ERROR: " << e.what() << std::endl << std::endl;
        print_usage(options);
        return 1;
    }

    if (variablesMap.count("help")) {
        print_usage(options);
        return 0;
    } else if (variablesMap.count("version")) {
        std::cout << "FreeHand v" << VERSION << std::endl;
        return 0;
    }

    if (!variablesMap.count("bytecode_file")) {
        std::cerr << "ERROR: Missing argument 'bytecode_file'" << std::endl << std::endl;
        print_usage(options);
        return 1;
    }

    fs::path bytecode_file = variablesMap["bytecode_file"].as<fs::path>();
    FreeHand freehand(bytecode_file);

    freehand.AnalyzeAllFunctions();

    return 0;
}


#endif //FREEHAND_FREEHAND_H
