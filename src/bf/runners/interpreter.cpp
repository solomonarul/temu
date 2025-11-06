#include "bf/runners/interpreter.hpp"

#include <sstream>
#include <format>

using namespace BF::Runners;

Result<void> Interpreter::load_ir(IR& ir)
{
    this->code = ir.code;
    return {};
}

Result<void> Interpreter::run(State& state)
{
    state.reset();

    static void* code_list[(int)Instruction::BF_INSTRUCTION_COUNT + 1] = { 
        &&unimplemented_label, &&add_label, &&mov_label, &&jmp_label,
        &&in_label, &&out_label, &&clr_label, &&end_label, &&unimplemented_label
    };

    std::stringstream result_message;
    size_t pc = 0;
    uint16_t index = 0;

    goto *code_list[this->code[pc].type];

add_label:
    state.memory[index] += this->code[pc].arg;
    goto *code_list[this->code[++pc].type];

mov_label:
    index += this->code[pc].arg;
    goto *code_list[this->code[++pc].type];

jmp_label:
    pc += ((this->code[pc].arg > 0 && state.memory[index] == 0)
        || (this->code[pc].arg < 0 && state.memory[index] != 0)) ? this->code[pc].arg : 1;
    goto *code_list[this->code[pc].type];

in_label:
    // Same as xbyak, if f_in is not set this will crash.
    state.memory[index] = state.f_in(state);
    goto *code_list[this->code[++pc].type];

out_label:
    // Same as xbyak, if f_out is not set this will crash.
    state.f_out(state, state.memory[index]);
    goto *code_list[this->code[++pc].type];

clr_label:
    state.memory[index] = 0;
    goto *code_list[this->code[++pc].type];

unimplemented_label:
    result_message << std::format(
        "BF interpreter encountered unimplemented opcode {} (enum: {})",
        Instruction::type_to_string(this->code[pc].type), (int)this->code[pc].type
    );

    result_message << (this->code[pc].has_arguments() ?
        std::format(" with arguments {}", this->code[pc].args_to_string()) : "") << "!\n";

    result_message << std::format("        Interpreter state:\n        PC: {}\tI: {}\n", pc, index);
    return std::unexpected<std::string>(result_message.str());

end_label:
    return {};
}
