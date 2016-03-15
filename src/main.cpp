#include "nodes.hpp"
#include "Outputter.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

extern int yyparse();
extern "C" FILE* yyin;
extern std::shared_ptr<NRoot> root;

std::tuple<std::string, std::string>
get_extension(const std::string& whole_filename) {
    std::string filename = "";
    std::string ext = "";

    auto pos = whole_filename.find('.');
    if (pos != std::string::npos) {
        filename = whole_filename.substr(0, pos - 1);
        ext = whole_filename.substr(pos + 1);

        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }

    return std::make_tuple(filename, ext);
}

int
main(const int argc, char* const argv[]) {
    yyin = stdin;

    std::string filename, ext;
    if (argc >= 1) {
        std::tie(filename, ext) = get_extension(argv[1]);

        if (ext == "hshg") {
            FILE* in_file = fopen(argv[1], "r");

            if (!in_file) {
                std::cerr << "Couldn't open " << argv[1] << std::endl;
                return 1;
            }

            yyin = in_file;
        }
        else if (ext == "l5x") {
            // TODO: parse l5x file and pipe it into the parser
        }
    }

    // yyparse();

    SymTable s;
    root->populate_symtable(s);

    Outputter verilog_outputter;
    verilog_outputter.append("program_name", filename);
    root->to_verilog(verilog_outputter);
    verilog_outputter.finilize(std::string{argv[2]} + "/verilog/Main.v", "foo.v");

    Outputter dot_outputter;
    dot_outputter.append("program_name", filename);
    root->to_dot(dot_outputter);
    dot_outputter.finilize(std::string{argv[2]} + "/dot/ast.dot", "foo.dot");

    return 0;
}
