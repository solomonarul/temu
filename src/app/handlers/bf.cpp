#include "app/handlers/bf.hpp"

#include <memory>

#include "bf/devices/all.hpp"
#include "bf/runners/all.hpp"

using namespace BF;

Result<void> run_bf_emulation(LINI::File& ini_file)
{
    IF_ERROR_RET(!ini_file.sections.contains("Runner.BF"), "No BF specific section in the input ini file.");

    LINI::Section& bf_section = ini_file.sections["Runner.BF"];
    IF_ERROR_RET(!bf_section.entries.contains("file"), "No BF input file specified in the input ini file.");

    auto path = bf_section.entries["file"].to_string();
    std::ifstream bf_in(path);
    IF_ERROR_FMT_RET(!bf_in.is_open(), "Could not open BF source file at path {}.", path);
    std::string input_bf{std::istreambuf_iterator<char>(bf_in), {}};
    std::move(bf_in).close();

    std::unique_ptr<IDevice> input = std::make_unique<Devices::None>();
    if(bf_section.entries.contains("device"))
    {
        auto type = bf_section.entries["device"].to_string();
        to_lower(type);
        if(type == "std") input = std::make_unique<Devices::STD>();
    }

    IR code;
    TRY(code.compile(input_bf, (input->get_type() >= BF::EDEVICE_NONE) ?
        IR::CompileFlags{.input = false, .output = false} :
        IR::CompileFlags{.input = true, .output = true}
    ));
    
    IF_ERROR_RET(!bf_section.entries.contains("type"), "No BF runner type specified in input file.");

    auto type = bf_section.entries["type"].to_string(); to_lower(type);

    std::unique_ptr<IRunner> runner;
    if (type == "jit") runner = std::make_unique<Runners::xbyak>();
    else if (type == "interpreter") runner = std::make_unique<Runners::Interpreter>();
    else return ERROR_FMT("Unknown BF runner type {} in input file.", type);
    
    TRY(runner->load_ir(code));

    State state;
    state.device.swap(input);
    TRY(runner->run(state, &state.device.get()->handler));

    return {};
}