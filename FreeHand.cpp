/* MemoryAllocationAnalyzer Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * FreeHand runner
 */

#include <iostream>
#include <algorithm>
#include "MemoryAllocationAnalyzer.hpp"
#include "UAF.hpp"
#include "DF.hpp"

std::string VERSION("1.0.0");

void print_usage(po::options_description &options) {
    std::cout << "Usage: freehand [options] bytecode_file" << std::endl;
    std::cout << options << std::endl;
}

int main(int argc, char **argv) {
    po::options_description options("Options");
    options.add_options()
            ("version,v", "Print a version string")
            ("help,h", "Produce a help message")
            ("function,f", po::value<std::vector<std::string >>(), "Specific functions to analyze")
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
        std::cout << "MemoryAllocationAnalyzer v" << VERSION << std::endl;
        return 0;
    }

    if (!variablesMap.count("bytecode_file")) {
        std::cerr << "ERROR: Missing argument 'bytecode_file'" << std::endl << std::endl;
        print_usage(options);
        return 1;
    }

    fs::path bytecode_file = variablesMap["bytecode_file"].as<fs::path>();
    MemoryAllocationAnalyzer freehand(bytecode_file);

    if (!variablesMap.count("function")) {
        freehand.analyzeAllFunctions();
    } else {
        for (auto function_name: variablesMap["function"].as<std::vector<std::string >>()) {
            freehand.analyzeFunctionByName(function_name);
        }
    }

    auto dfs = freehand.getAllDF();
    if (dfs.empty()) {
        std::cout << "No Double-Free bugs found!" << std::endl;
    } else {
        std::for_each(dfs.begin(), dfs.end(), [](const DF &df) { std::cout << df; });
    }

    auto uafs = freehand.getAllUAF();
    if (uafs.empty()) {
        std::cout << "No Use-After-Free bugs found!" << std::endl;
    } else {
        std::for_each(uafs.begin(), uafs.end(), [](const UAF &uaf) { std::cout << uaf; });
    }

    return 0;
}