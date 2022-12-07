/* MemoryAllocationAnalyzer Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * MemoryAllocationAnalyzer implementation
 */

#include "MemoryAllocationAnalyzer.hpp"

#include <llvm-14/llvm/IRReader/IRReader.h>
#include <llvm-14/llvm/Support/SourceMgr.h>
#include <llvm-14/llvm/IR/Verifier.h>
#include <llvm-14/llvm/IR/Instructions.h>

std::set<std::string> ALLOC_FUNCTION_NAMES = {"malloc", "aligned_malloc",
                                              "calloc", "kmalloc", "kcalloc",
                                              "vmalloc", "kzalloc"};
std::set<std::string> FREE_FUNCTION_NAMES = {"free", "kfree"};

MemoryAllocationAnalyzer::MemoryAllocationAnalyzer(fs::path &bytecode_path) {
    this->bytecode_path = bytecode_path;

    llvm::SMDiagnostic diagnostic_report;
    this->llvm_module = llvm::parseIRFile(llvm::StringRef(this->bytecode_path.c_str()),
                                          diagnostic_report,
                                          this->llvm_context);

    if (!this->llvm_module) {
        std::cerr << "ERROR: Failed to parse bytecode file:" << std::endl <<
                  diagnostic_report.getMessage().data() << std::endl;
        exit(1);
    }

    std::string error_output;
    llvm::raw_string_ostream error_output_stream(error_output);
    if (llvm::verifyModule(*this->llvm_module, &error_output_stream, nullptr)) {
        std::cerr << "ERROR: Invalid module detected:" << std::endl << error_output_stream.str() << std::endl;
        exit(1);
    }

}

void MemoryAllocationAnalyzer::analyzeAllFunctions() {
    for (auto &function: *this->llvm_module) {
        analyzeFunction(function);
    }
}

void MemoryAllocationAnalyzer::analyzeFunction(llvm::Function &function) {
    unsigned long long nextId = 0;
    std::set<unsigned long long> freeSet;

    std::map<llvm::Value *, std::vector<unsigned long long>> valueToIds;

    for (auto &basicBlock: function) {
        for (auto &instruction: basicBlock) {

            if (llvm::isa<llvm::CallBase>(&instruction)) {
                auto callInstruction = llvm::cast<llvm::CallBase>(&instruction);

                if (callInstruction->isIndirectCall()) {
                    std::cout << "WARNING: Indirect call detected at source line "
                              << instruction.getDebugLoc().getLine() << std::endl;
                } else {
                    llvm::Function *callee = callInstruction->getCalledFunction();
                    std::string calleeName = callee->getName().str();

                    if (ALLOC_FUNCTION_NAMES.contains(calleeName)) {
                        std::vector p = {nextId++};
                        valueToIds.emplace(&instruction, p);
                    } else if (FREE_FUNCTION_NAMES.contains(calleeName)) {
                        for (auto id: valueToIds[callInstruction->getArgOperand(0)]) {
                            if (!freeSet.emplace(id).second) {
                                this->addDF(&function, instruction.getDebugLoc());
                            }
                        }
                    }
                }
            } else if (llvm::isa<llvm::CastInst>(instruction)) {
                auto castInstruction = llvm::cast<llvm::CastInst>(&instruction);

                auto value = valueToIds.find(castInstruction->getOperand(0));
                if (value != valueToIds.end()) {
                    for (auto id: value->second) {
                        if (freeSet.contains(id)) {
                            this->addUAF(&function, instruction.getDebugLoc());
                        }
                    }
                    valueToIds[&instruction] = value->second;
                }

            } else if (llvm::isa<llvm::BinaryOperator>(instruction)) {
                auto binaryOperator = llvm::cast<llvm::BinaryOperator>(&instruction);

                if (!(binaryOperator->getOpcode() == llvm::BinaryOperator::BinaryOps::Xor &&
                      binaryOperator->getOpcode() == llvm::BinaryOperator::BinaryOps::And &&
                      binaryOperator->getOpcode() == llvm::BinaryOperator::BinaryOps::Or)) {

                    std::vector<unsigned long long> new_ids;
                    auto value1 = valueToIds.find(binaryOperator->getOperand(0));
                    if (value1 != valueToIds.end()) {
                        new_ids = value1->second;
                    }

                    auto value2 = valueToIds.find(binaryOperator->getOperand(1));
                    if (value2 != valueToIds.end()) {
                        new_ids.reserve(new_ids.size() + std::distance(value2->second.begin(),
                                                                       value2->second.end()));
                        new_ids.insert(new_ids.end(), value2->second.begin(), value2->second.end());
                    }

                    if (!new_ids.empty()) {
                        for (auto id: new_ids) {
                            if (freeSet.contains(id)) {
                                this->addUAF(&function, instruction.getDebugLoc());
                            }
                        }
                        valueToIds[&instruction] = new_ids;
                    }
                }
            } else if (llvm::isa<llvm::LoadInst>(instruction)) {
                auto loadInstruction = llvm::cast<llvm::LoadInst>(&instruction);

                auto value = valueToIds.find(loadInstruction->getPointerOperand());
                if (value != valueToIds.end() && loadInstruction->getType()->isPointerTy()) {
                    for (auto id: value->second) {
                        if (freeSet.contains(id)) {
                            this->addUAF(&function, instruction.getDebugLoc());
                        }
                    }
                    valueToIds[&instruction] = value->second;
                }
            } else if (llvm::isa<llvm::StoreInst>(instruction)) {
                auto storeInstruction = llvm::cast<llvm::StoreInst>(&instruction);

                auto value = valueToIds.find(storeInstruction->getValueOperand());
                if (value != valueToIds.end() && storeInstruction->getValueOperand()->getType()->isPointerTy()) {
                    for (auto id: value->second) {
                        if (freeSet.contains(id)) {
                            this->addUAF(&function, instruction.getDebugLoc());
                        }
                    }
                    valueToIds[storeInstruction->getPointerOperand()] = value->second;
                }
            } else if (llvm::isa<llvm::GetElementPtrInst>(instruction)) {
                auto getElementPtrInstruction = llvm::cast<llvm::GetElementPtrInst>(&instruction);

                auto value = valueToIds.find(getElementPtrInstruction->getPointerOperand());
                if (value != valueToIds.end()) {
                    for (auto id: value->second) {
                        if (freeSet.contains(id)) {
                            this->addUAF(&function, instruction.getDebugLoc());
                        }
                    }
                    valueToIds[&instruction] = value->second;
                }
            }
        }
    }
}

void MemoryAllocationAnalyzer::analyzeFunctionByName(std::string &function_name) {

    auto *function = this->llvm_module->getFunction(function_name);
    if (function == nullptr) {
        std::cerr << "ERROR: Function '" << function_name << "' does not exist" << std::endl;
    }

    this->analyzeFunction(*function);
}

void MemoryAllocationAnalyzer::addDF(llvm::Function *function, const llvm::DebugLoc &debugLoc) {
    for (const auto &pair: this->dfMap[function]) {
        if (pair.getLineNumber() == debugLoc.getLine() && pair.getColumnNumber() == debugLoc.getCol()) {
            return;
        }
    }
    this->dfMap[function].emplace_back(this->bytecode_path, function, debugLoc);
}

void MemoryAllocationAnalyzer::addUAF(llvm::Function *function, const llvm::DebugLoc &debugLoc) {
    for (const auto &pair: this->uafMap[function]) {
        if (pair.getLineNumber() == debugLoc.getLine() && pair.getColumnNumber() == debugLoc.getCol()) {
            return;
        }
    }
    this->uafMap[function].emplace_back(this->bytecode_path, function, debugLoc);
}

std::vector<DF> MemoryAllocationAnalyzer::getAllDF() {
    std::vector<DF> all{};

    for (const auto &pair: this->dfMap) {
        all.reserve(all.size() + pair.second.size());
        all.insert(all.end(), pair.second.begin(), pair.second.end());
    }

    return all;
}

std::vector<UAF> MemoryAllocationAnalyzer::getAllUAF() {
    std::vector<UAF> all{};

    for (const auto &pair: this->uafMap) {
        all.reserve(all.size() + pair.second.size());
        all.insert(all.end(), pair.second.begin(), pair.second.end());
    }

    return all;
}

[[maybe_unused]] std::vector<DF> MemoryAllocationAnalyzer::getDFByFunction(std::string &function_name) {
    llvm::Function *function = this->llvm_module->getFunction(function_name);

    if (function == nullptr || !this->dfMap.contains(function)) {
        return std::vector<DF>{};
    } else {
        return this->dfMap[function];
    }
}

[[maybe_unused]] std::vector<UAF> MemoryAllocationAnalyzer::getUAFByFunction(std::string &function_name) {
    llvm::Function *function = this->llvm_module->getFunction(function_name);

    if (function == nullptr || !this->uafMap.contains(function)) {
        return std::vector<UAF>{};
    } else {
        return this->uafMap[function];
    }
}
