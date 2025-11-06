#include "app/handler.hpp"

#include <format>
#include <fstream>
#include <iostream>

using namespace App;

#include <lini/file.hpp>

#include "bf/runners/interpreter.hpp"
#include "bf/runners/xbyak.hpp"

Result<void> handle_bf_emulation(LINI::File& ini_file)
{
    if(!ini_file.sections.contains("Runner.BF"))
        return std::unexpected("No BF specific section in the input ini file.");

    LINI::Section& bf_section = ini_file.sections["Runner.BF"];
    if(!bf_section.entries.contains("file"))
        return std::unexpected("No BF input file specified in the input ini file.");

    const std::string path = bf_section.entries["file"].to_string();
    std::ifstream bf_in(path);
    if(!bf_in)
        return std::unexpected(std::format("Could not open BF source file at path {}.", path));
    std::string input_bf{std::istreambuf_iterator<char>(bf_in), {}};
    bf_in.close();

    BF::IR code;
    auto result = code.compile_string(input_bf, {.input = true, .output = true});
    if(!result)
        return std::unexpected(result.error());
    
    // TODO: fit these into an union or smth.
    BF::Runners::Interpreter bf_interpreter;
    BF::Runners::xbyak bf_jit;
    BF::IRunner& runner = bf_jit;
    result = runner.load_ir(code);
    if(!result)
        return std::unexpected(result.error());

    BF::State state;
    state.f_in = [](BF::State&) -> size_t {
        char ch;
        if(!std::cin.get(ch))
            return std::char_traits<char>::eof();
        return static_cast<size_t>(ch);
    };
    state.f_out = [](BF::State&, size_t data) -> void {
        std::cout.put(static_cast<char>(data));
    };
    result = runner.run(state);
    if(!result)
        return std::unexpected(result.error());
    std::cout.flush();
    return {};
}

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