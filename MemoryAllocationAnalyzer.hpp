/* MemoryAllocationAnalyzer Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * MemoryAllocationAnalyzer header
 */

#ifndef FREEHAND_MEMORYALLOCATIONANALYZER_HPP
#define FREEHAND_MEMORYALLOCATIONANALYZER_HPP

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <llvm-14/llvm/IR/LLVMContext.h>
#include <llvm-14/llvm/IR/Module.h>

#include "UAF.hpp"
#include "DF.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class MemoryAllocationAnalyzer {
public:
    explicit MemoryAllocationAnalyzer(fs::path &bytecode_path);

    ~MemoryAllocationAnalyzer() = default;

    void analyzeAllFunctions();

    void analyzeFunction(llvm::Function &function);

    void analyzeFunctionByName(std::string &function_name);

    std::vector<DF> getAllDF();

    std::vector<UAF> getAllUAF();


    [[maybe_unused]] std::vector<DF> getDFByFunction(std::string &function_name);

    [[maybe_unused]] std::vector<UAF> getUAFByFunction(std::string &function_name);


private:
    fs::path bytecode_path;
    llvm::LLVMContext llvm_context;
    std::unique_ptr<llvm::Module> llvm_module;
    std::map<llvm::Function *, std::vector<UAF>> uafMap;
    std::map<llvm::Function *, std::vector<DF>> dfMap;

    void addDF(llvm::Function *function, const llvm::DebugLoc &debugLoc);

    void addUAF(llvm::Function *function, const llvm::DebugLoc &debugLoc);
};

#endif //FREEHAND_MEMORYALLOCATIONANALYZER_HPP
