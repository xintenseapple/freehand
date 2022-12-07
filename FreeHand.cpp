/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * FreeHand implementation
 */

#include "FreeHand.h"

#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

FreeHand::FreeHand(fs::path &bytecode_path) {
    this->bytecode_path = bytecode_path;

    llvm::SMDiagnostic error;
    this->llvm_module = llvm::parseIRFile(llvm::StringRef(this->bytecode_path.c_str()),
                                          error,
                                          this->llvm_context);

    if (!this->llvm_module) {
        std::cerr << "ERROR: Failed to parse bytecode file: " << std::endl << error.getMessage().data() << std::endl;
        exit(1);
    }
}
