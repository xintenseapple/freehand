/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * FreeHand implementation
 */

#include "FreeHand.h"
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>


namespace po = boost::program_options;
namespace fs = boost::filesystem;

void print_usage(po::options_description &options) {
    std::cout << "Usage: freehand [options] bytecode_file" << std::endl;
    std::cout << options << std::endl;
}

int main(int argc, char **argv) {

    po::options_description options("Options");
    options.add_options()
            ("version,v", "Print a version string")
            ("help,h", "Produce a help message")
            ("bytecode_file", po::value<std::string>(),
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
    }
    else if (variablesMap.count("version")) {
        std::cout << "FreeHand v" << VERSION << std::endl;
        return 0;
    }

    if (!variablesMap.count("bytecode_file")) {
        std::cout << "ERROR: Missing argument 'bytecode_file'" << std::endl << std::endl;
        print_usage(options);
        return 1;
    }

    std::string bytecode_file = variablesMap["bytecode_file"].as<std::string>();

    std::cout << bytecode_file << std::endl;

    return 0;
}

