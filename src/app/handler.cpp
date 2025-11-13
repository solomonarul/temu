#include "app/handler.hpp"

#include <format>
#include <fstream>

using namespace App;

#include <lini/file.hpp>

#include "app/handlers/bf.hpp"

Result<void> App::run(int argc, char* argv[])
{
    IF_ERROR_FMT_RET(argc < 2, "Bad usage.\n[INFO]: Usage: {} [config.ini]\n", argv[0]);

    std::ifstream ini_in(argv[1]);
    IF_ERROR_FMT_RET(!ini_in.is_open(), "Could not open input file at path {}.", argv[1]);

    std::string ini_contents = {std::istreambuf_iterator<char>(ini_in), {}};
    std::move(ini_in).close();

    LINI::File ini_file = LINI::File().parse(ini_contents);
    IF_ERROR_RET(!ini_file.sections.contains("Runner"), "No Runner specified in input ini file.");

    LINI::Section& runner_section = ini_file.sections["Runner"];
    IF_ERROR_RET(!runner_section.entries.contains("core"), "No core specified in the Runner section of the input ini file.");

    const std::string core = runner_section.entries["core"].to_string();
    if(core == "BF")
        return run_bf_emulation(ini_file);

    return ERROR_FMT("Invalid core specified in the Runner section of the input ini file: {}", core);
}