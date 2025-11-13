#include "app/handlers/bf.hpp"

#include <iostream>
#include <memory>

#include "bf/runners/interpreter.hpp"
#include "bf/runners/xbyak.hpp"

using namespace BF;

enum InputType {
    BF_INPUT_NONE = 0,
    BF_INPUT_STD,
    BF_INPUT_SDL,
    BF_INPUT_COUNT
};

Result<void> run_bf_runner(IRunner& runner, IR& code, InputType input)
{
    TRY(runner.load_ir(code));

    State state;
    switch(input)
    {
    case BF_INPUT_STD:
        state.f_in = [](State&) -> size_t {
            char ch;
            if(!std::cin.get(ch))
                return std::char_traits<char>::eof();
            return static_cast<size_t>(ch);
        };
        state.f_out = [](State&, size_t data) -> void {
            std::cout.put(static_cast<char>(data));
        };
        break;

    default:
        break;
    }
    TRY(runner.run(state));

    switch(input)
    {
    case BF_INPUT_STD:
        std::cout << '\n';
        break;

    default:
        break;
    }
    
    return {};
}

Result<void> run_bf_emulation(LINI::File& ini_file)
{
    IF_ERROR_RET(!ini_file.sections.contains("Runner.BF"), "No BF specific section in the input ini file.");

    LINI::Section& bf_section = ini_file.sections["Runner.BF"];
    IF_ERROR_RET(!bf_section.entries.contains("file"), "No BF input file specified in the input ini file.");

    auto path = bf_section.entries["file"].to_string();
    std::ifstream bf_in(path);
    IF_ERROR_FMT_RET(!bf_in.is_open(), "Could not open BF source file at path {}.", path);
    std::string input_bf{std::istreambuf_iterator<char>(bf_in), {}};
    bf_in.close();

    auto input_type = BF_INPUT_STD;
    if(!bf_section.entries.contains("device"))
        input_type = BF_INPUT_NONE;
    else {
        auto input = bf_section.entries["device"].to_string(); to_lower(input);
        if(input == "sdl")
            input_type = BF_INPUT_SDL;
        else if(input == "std")
            input_type = BF_INPUT_STD;
        else
            input_type = BF_INPUT_NONE;
    }

    IR code;
    TRY(code.compile(input_bf, (input_type == BF_INPUT_NONE) ?
        IR::CompileFlags{.input = false, .output = false} :
        IR::CompileFlags{.input = true, .output = true}
    ));
    
    IF_ERROR_RET(!bf_section.entries.contains("type"), "No BF runner type specified in input file.");

    auto type = bf_section.entries["type"].to_string(); to_lower(type);

    std::unique_ptr<IRunner> runner;
    if (type == "jit") runner = std::make_unique<Runners::xbyak>();
    else if (type == "interpreter") runner = std::make_unique<Runners::Interpreter>();
    else return ERROR_FMT("Unknown BF runner type {} in input file.", type);
    
    return run_bf_runner(*runner, code, input_type);
}