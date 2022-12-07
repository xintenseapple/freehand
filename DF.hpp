/* MemoryAllocationAnalyzer Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * DF model header
 */

#ifndef FREEHAND_DF_HPP
#define FREEHAND_DF_HPP

#include <boost/filesystem.hpp>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/Function.h>

namespace fs = boost::filesystem;

class DF {
public:
    DF(fs::path file_path, llvm::Function* function, unsigned long line_number, unsigned long column_number)
            : file_path(std::move(file_path)),
              function(function),
              lineNumber(line_number),
              columnNumber(column_number) {}

    DF(fs::path file_path, llvm::Function* function, const llvm::DebugLoc &debugLoc)
            : file_path(std::move(file_path)),
              function(function),
              lineNumber(debugLoc.getLine()),
              columnNumber(debugLoc.getCol()) {}

    fs::path getFilePath() { return this->file_path; }

    llvm::Function* getFunction() { return this->function; }

    [[nodiscard]] unsigned long getLineNumber() const { return this->lineNumber; }

    [[nodiscard]] unsigned long getColumnNumber() const { return this->columnNumber; }

    friend std::ostream &operator<<(std::ostream &os, DF const &DF) {
        return os << "Potential Double-Free:" << std::endl
                  << "\tFile: " << DF.file_path << std::endl
                  << "\tFunction: " << DF.function->getName().str() << std::endl
                  << "\tLine, Column: " << DF.lineNumber << ", " << DF.columnNumber << std::endl
                  << std::endl;
    }

private:
    fs::path file_path;
    llvm::Function* function;
    unsigned long lineNumber;
    unsigned long columnNumber;
};


#endif //FREEHAND_DF_HPP
