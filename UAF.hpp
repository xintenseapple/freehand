/* MemoryAllocationAnalyzer Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * UAF model header
 */

#ifndef FREEHAND_UAF_HPP
#define FREEHAND_UAF_HPP

#include <boost/filesystem.hpp>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Function.h>

namespace fs = boost::filesystem;

class UAF {
public:
    UAF(fs::path file_path, llvm::Function *function, unsigned long line_number, unsigned long column_number)
            : file_path(std::move(file_path)),
              function(function),
              lineNumber(line_number),
              columnNumber(column_number) {}

    UAF(fs::path file_path, llvm::Function *function, const llvm::DebugLoc &debugLoc)
            : file_path(std::move(file_path)),
              function(function),
              lineNumber(debugLoc.getLine()),
              columnNumber(debugLoc.getCol()) {}

    fs::path getFilePath() { return this->file_path; }

    llvm::Function *getFunction() { return this->function; }

    [[nodiscard]] unsigned long getLineNumber() const { return this->lineNumber; }

    [[nodiscard]] unsigned long getColumnNumber() const { return this->columnNumber; }

    friend std::ostream &operator<<(std::ostream &os, UAF const &uaf) {
        return os << "Potential Use-After-Free:" << std::endl
                  << "\tFile: " << uaf.file_path << std::endl
                  << "\tFunction: " << uaf.function->getName().str() << std::endl
                  << "\tLine, Column: " << uaf.lineNumber << ", " << uaf.columnNumber << std::endl
                  << std::endl;
    }

private:
    fs::path file_path;
    llvm::Function *function;
    unsigned long lineNumber;
    unsigned long columnNumber;
};


#endif //FREEHAND_UAF_HPP
