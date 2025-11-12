#include "c8/ir.hpp"

#include <sstream>

namespace C8
{
    std::string_view Instruction::type_to_string(Instruction::Type type)
    {
        (void)type;
        return "test";
    }

    bool Instruction::has_arguments(void)
    {
        return true;
    }

    std::string Instruction::args_to_string(void)
    {
        std::stringstream result;
        return std::move(result).str();
    }

    Result<void> IR::compile(std::vector<uint16_t>& code)
    {
        (void)code;
        this->code.emplace_back(Instruction{Instruction::C8_INSTRUCTION_END});
        return {};
    }

    std::string IR::dump(void)
    {
        std::stringstream out;
        return std::move(out).str();
    }
};