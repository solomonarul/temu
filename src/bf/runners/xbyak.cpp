#include "bf/runners/xbyak.hpp"

using namespace BF;

typedef void(*bf_funct_t)(State*);

Result<void> BF::Runners::xbyak::load_ir(IR& ir)
{
    constexpr size_t CELL_SIZE = sizeof(typeof(State::memory[0]));

    // Initial state (On System-V abi they are supposed to be restored after calls):
    // r12 = &state
    // r13 = state.memory[0]
    // r14w = 16-bit offset into memory
    mov(r12, rdi);
    lea(r13, ptr [rdi + offsetof(State, memory)]);
    xor_(r14, r14);

    std::vector<std::pair<Xbyak::Label, Xbyak::Label>> startedLoops;

    for (auto instruction: ir.code)
    {
        switch(instruction.type)
        {
        case Instruction::BF_INSTRUCTION_MOV:
            add(r14w, instruction.arg);
            break;

        case Instruction::BF_INSTRUCTION_ADD:
            add(qword [r13 + r14 * CELL_SIZE], instruction.arg);
            break;

        case Instruction::BF_INSTRUCTION_JMP:
            if(instruction.arg > 0)
            {
                Xbyak::Label start, end;
                startedLoops.emplace_back(std::make_pair(start, end));
                
                auto& back = startedLoops.back();
                mov(rax, qword [r13 + r14 * CELL_SIZE]);
                cmp(rax, 0);
                jz(back.second, T_NEAR);
                L(back.first);
            }
            else {
                auto& brackets = startedLoops.back();
                
                mov(rax, qword [r13 + r14 * CELL_SIZE]);
                cmp(rax, 0);
                jnz(brackets.first,T_NEAR);

                L(brackets.second);
                startedLoops.pop_back();
            }
            break;

        case Instruction::BF_INSTRUCTION_INP:
        {
            // state.f_in(rdi)
            // No checks are done, if f_out is invalid this will crash.
            mov(rax, ptr [r12 + offsetof(State, f_in)]);
            mov(rdi, r12);
            call(rax);
            mov(qword [r13 + r14 * CELL_SIZE], rax);
            break;
        }

        case Instruction::BF_INSTRUCTION_OUT:
        {
            // state.f_out(rdi, rsi)
            // No checks are done, if f_out is invalid this will crash.
            mov(rax, ptr [r12 + offsetof(State, f_out)]);
            mov(rdi, r12);
            mov(rsi, qword [r13 + r14 * CELL_SIZE]);
            call(rax);
            break;
        }

        case Instruction::BF_INSTRUCTION_CLR:
            mov(qword [r13 + r14 * CELL_SIZE], 0);
            break;

        case Instruction::BF_INSTRUCTION_END:
            ret();
            break;

        default:
            // Do nothing.
            break;
        }
    }
    readyRE();

    auto result = Xbyak::GetError();
    if(result)
        return ERROR_FMT("XByak generator error: {}", Xbyak::ConvertErrorToString(result));

    return {};
}

void BF::Runners::xbyak::dump(std::ofstream& fout)
{
    fout.write((char*)getCode<bf_funct_t>(), getSize());
}

Result<void> BF::Runners::xbyak::run(State& state)
{
    state.reset();
    (getCode<bf_funct_t>())(&state);
    return {};
}
