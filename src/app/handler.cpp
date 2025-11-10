#include "app/handler.hpp"

#include <format>
#include <fstream>
#include <iostream>

using namespace App;

#include <lini/file.hpp>

#include "app/bf_handler.hpp"

Result<void> App::run(int argc, char* argv[])
{
    if(argc < 2)
        return std::unexpected(std::format("Bad usage.\n[INFO]: Usage: {} [config.ini]\n", argv[0]));

    std::ifstream ini_in(argv[1]);
    if(!ini_in)
        return std::unexpected(std::format("Could not open input file at path {}.", argv[1]));

    std::stringstream ini_stream; ini_stream << ini_in.rdbuf();
    ini_in.close();

    LINI::File ini_file;
    ini_file.parse(ini_stream);

    if(!ini_file.sections.contains("Runner"))
        return std::unexpected("No Runner specified in input ini file.");

    LINI::Section& runner_section = ini_file.sections["Runner"];

    if(!runner_section.entries.contains("core"))
        return std::unexpected("No core specified in the Runner section of the input ini file.");

    const std::string core = runner_section.entries["core"].to_string();
    if(core == "BF")
        return handle_bf_emulation(ini_file);

    return std::unexpected(
        std::format(
            "Invalid core specified in the Runner section of the input ini file: {}",
            core
        )
    );
}