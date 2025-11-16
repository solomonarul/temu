#include "bf/runners/xbyak.hpp"

using namespace BF;

#define CELL_AT(offset) qword [r13 + (offset) * CELL_SIZE]

using bf_funct_t = void(*)(State*, IDeviceHandler*);

constexpr size_t CELL_SIZE = sizeof(decltype(State::memory[0]));

struct LoopLabels { Xbyak::Label start, end; };

Result<void> BF::Runners::xbyak::load_ir(IR& ir)
{
    // Initial state (On System-V abi they are supposed to be restored after calls):
    // r12 = &state.handler
    // r13 = state.memory[0]
    // r14w = 16-bit offset into memory
    // System-V abi arguments in order: rdi, rsi, rdx, rcx, r8, r9
    // Everything else goes onto the stack, except floating point values which go xmm0..xmm7.
    mov(r12, rsi);
    lea(r13, ptr[rdi + offsetof(State, memory)]);
    xor_(r14, r14);

    std::vector<LoopLabels> startedLoops;

    for (const auto& instruction: ir.code)
    {
        switch(instruction.type)
        {
        case Instruction::BF_INSTRUCTION_MOV:
            add(r14w, instruction.arg);
            break;

        case Instruction::BF_INSTRUCTION_ADD:
            add(CELL_AT(r14), instruction.arg);
            break;

        case Instruction::BF_INSTRUCTION_JMP:
            if(instruction.arg > 0)
            {
                startedLoops.emplace_back(Xbyak::Label(), Xbyak::Label());
                
                auto& back = startedLoops.back();
                mov(rax, CELL_AT(r14));
                cmp(rax, 0);
                jz(back.end, T_NEAR);
                L(back.start);
            }
            else {
                auto& back = startedLoops.back();
                
                mov(rax, CELL_AT(r14));
                cmp(rax, 0);
                jnz(back.start,T_NEAR);

                L(back.end);
                startedLoops.pop_back();
            }
            break;

        case Instruction::BF_INSTRUCTION_AAO:
            mov(rax, CELL_AT(r14));
            mov(CELL_AT(r14), 0);
            mov(rdx, r14);
            add(rdx, instruction.arg);
            add(CELL_AT(rdx), rax);
            break;

        case Instruction::BF_INSTRUCTION_INP:
            // state.f_in(rdi)
            // No checks are done, if f_out is invalid this will crash.
            mov(rax, ptr [r12 + offsetof(IDeviceHandler, input)]);
            mov(rdi, r12);
            call(rax);
            mov(CELL_AT(r14), rax);
            break;

        case Instruction::BF_INSTRUCTION_OUT:
            // state.f_out(rdi, rsi)
            // No checks are done, if f_out is invalid this will crash.
            mov(rax, ptr [r12 + offsetof(IDeviceHandler, output)]);
            mov(rdi, r12);
            mov(rsi, CELL_AT(r14));
            call(rax);
            break;

        case Instruction::BF_INSTRUCTION_CLR:
            mov(CELL_AT(r14), 0);
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

    if(auto error = Xbyak::GetError(); error)
        return ERROR_FMT("XByak generator error: {}", Xbyak::ConvertErrorToString(error));

    return {};
}

#undef CELL_AT

void BF::Runners::xbyak::dump(std::ofstream& fout)
{
    fout.write((char*)getCode<bf_funct_t>(), getSize());
}

Result<void> BF::Runners::xbyak::run(State& state, IDeviceHandler* handler)
{
    state.reset();
    (getCode<bf_funct_t>())(&state, handler);
    return {};
}
