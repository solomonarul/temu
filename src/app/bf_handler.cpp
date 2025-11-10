#include "app/bf_handler.hpp"

#include <iostream>

#include "bf/runners/interpreter.hpp"
#include "bf/runners/xbyak.hpp"

using namespace BF;

Result<void> handle_bf_runner(BF::IRunner& runner, BF::IR& code)
{
    auto result = runner.load_ir(code);
    if(!result)
        return std::unexpected(result.error());

    State state;
    state.f_in = [](State&) -> size_t {
        char ch;
        if(!std::cin.get(ch))
            return std::char_traits<char>::eof();
        return static_cast<size_t>(ch);
    };
    state.f_out = [](State&, size_t data) -> void {
        std::cout.put(static_cast<char>(data));
    };
    result = runner.run(state);
    if(!result)
        return std::unexpected(result.error());
    std::cout.flush();
    return {};
}

Result<void> handle_bf_emulation(LINI::File& ini_file)
{
    if(!ini_file.sections.contains("Runner.BF"))
        return std::unexpected("No BF specific section in the input ini file.");

    LINI::Section& bf_section = ini_file.sections["Runner.BF"];
    if(!bf_section.entries.contains("file"))
        return std::unexpected("No BF input file specified in the input ini file.");

    auto path = bf_section.entries["file"].to_string();
    std::ifstream bf_in(path);
    if(!bf_in)
        return std::unexpected(std::format("Could not open BF source file at path {}.", path));
    std::string input_bf{std::istreambuf_iterator<char>(bf_in), {}};
    bf_in.close();

    IR code;
    auto result = code.compile(input_bf, {.input = true, .output = true});
    if(!result)
        return std::unexpected(result.error());
    
    if(!bf_section.entries.contains("type"))
        return std::unexpected("No BF runner type specified in input file.");

    auto type = bf_section.entries["type"].to_string();
    std::transform(type.begin(), type.end(), type.begin(),
        [](unsigned char c){ return std::tolower(c); });
    if(type == "jit")
    {
        Runners::Interpreter interpreter;
        return handle_bf_runner(interpreter, code);
    }
    else if(type == "interpreter")
    {
        Runners::xbyak jit;
        return handle_bf_runner(jit, code);        
    }
    return std::unexpected(std::format("Unknown BF runner type {} in input file.", type));
}