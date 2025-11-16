#include "bf/runners/interpreter.hpp"

#include <sstream>
#include <format>

using namespace BF::Runners;

Result<void> Interpreter::load_ir(IR& ir)
{
    this->code = ir.code;
    return {};
}

#define NEXT() goto *code_list[this->code[++pc].type]
#define DISPATCH() goto *code_list[this->code[pc].type];

Result<void> Interpreter::run(State& state, IDeviceHandler* handler)
{
    (void)handler;
    state.reset();

    constexpr void* code_list[] = { 
        &&unimplemented_label, &&add_label, &&mov_label, &&jmp_label, &&add_at_offset_label,
        &&in_label, &&out_label, &&clr_label, &&end_label, &&unimplemented_label
    };

    std::stringstream result_message;
    size_t pc = 0;
    uint16_t index = 0;

    DISPATCH();

add_label: state.memory[index] += this->code[pc].arg; NEXT();

mov_label: index += this->code[pc].arg; NEXT();

jmp_label:
    pc += ((this->code[pc].arg > 0 && state.memory[index] == 0)
        || (this->code[pc].arg < 0 && state.memory[index] != 0)) ? this->code[pc].arg : 1;
    DISPATCH();

in_label: // Same as xbyak, if input is not set this will crash.
    state.memory[index] = state.device->handler.input(state); NEXT();

out_label: // Same as xbyak, if output is not set this will crash.
    state.device->handler.output(state, state.memory[index]); NEXT();

clr_label: state.memory[index] = 0; NEXT();

add_at_offset_label: state.memory[index + this->code[pc].arg] += state.memory[index], state.memory[index] = 0; NEXT();

unimplemented_label:
    result_message << std::format(
        "BF interpreter encountered unimplemented opcode {} (enum: {})",
        Instruction::type_to_string(this->code[pc].type), (int)this->code[pc].type
    );

    result_message << (this->code[pc].has_arguments() ?
        std::format(" with arguments {}", this->code[pc].args_to_string()) : "") << "!\n";

    result_message << std::format("        Interpreter state:\n        PC: {}\tI: {}\n", pc, index);
    return ERROR(result_message.str());

end_label:
    return {};
}

#undef DISPATCH
#undef NEXT
