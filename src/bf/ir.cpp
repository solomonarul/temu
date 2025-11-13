#include "bf/ir.hpp"

#include <sstream>

namespace BF
{
    static constexpr std::string_view mnemonics[] = {
        "", "ADD", "MOV", "JMP", " IN", "OUT", "CLR", "END", ""
    };

    std::string_view Instruction::type_to_string(Instruction::Type type)
    {
        return mnemonics[type];
    }

    bool Instruction::has_arguments(void)
    {
        return (this->type < Instruction::BF_INSTRUCTION_INP) && this->type != Instruction::BF_INSTRUCTION_NONE;
    }

    std::string Instruction::args_to_string(void)
    {
        std::stringstream result;

        switch(this->type)
        {
        case Instruction::BF_INSTRUCTION_ADD: case Instruction::BF_INSTRUCTION_MOV: case Instruction::BF_INSTRUCTION_JMP:
            result << this->arg;
            break;

        default:
            break;
        }

        return std::move(result).str();
    }

    Result<void> IR::compile(std::string& code, CompileFlags flags)
    {
        this->code.resize(0);
        this->code.reserve(code.size());
        std::vector<size_t> bracketStack;

        auto emit_repeating_opcode = [this](Instruction::Type type, ssize_t delta)
        {
            if(!this->code.empty() && this->code.back().type == type)
            {
                this->code.back().arg += delta;
                if(this->code.back().arg == 0)
                    this->code.pop_back();
            }
            else this->code.emplace_back(type, delta);
        };

        for(const auto ch : code)
        {
            switch(ch)
            {
            case '+': emit_repeating_opcode(Instruction::BF_INSTRUCTION_ADD, +1); break;
            case '-': emit_repeating_opcode(Instruction::BF_INSTRUCTION_ADD, -1); break;
            case '<': emit_repeating_opcode(Instruction::BF_INSTRUCTION_MOV, -1); break;
            case '>': emit_repeating_opcode(Instruction::BF_INSTRUCTION_MOV, +1); break;

            case '[':
                this->code.emplace_back( Instruction::BF_INSTRUCTION_JMP );
                bracketStack.emplace_back(this->code.size() - 1);
                break;

            case ']':
                if(!bracketStack.empty())
                {
                    ssize_t oldIndex = bracketStack.back();
                    bracketStack.pop_back();
                    ssize_t distance = this->code.size() - oldIndex;
                    if(distance > 1)
                    {
                        this->code.emplace_back(Instruction::BF_INSTRUCTION_JMP, -distance + 1);
                        this->code[oldIndex].arg = distance + 1;
                    }
                    else this->code.pop_back(); // Remove [] pairs.
                }
                else {
                    this->code.clear();
                    return ERROR("Unmatched ] found.");
                }
                break;

            case ',':
                if(flags.input)
                    this->code.emplace_back(Instruction::BF_INSTRUCTION_INP);
                break;

            case '.':
                if(flags.output)
                    this->code.emplace_back(Instruction::BF_INSTRUCTION_OUT);
                break;

            default:
                // Ignore anything else.
                break;
            }

            optimize();
        }
        this->code.emplace_back(Instruction::BF_INSTRUCTION_END);

        if(!bracketStack.empty())
        {
            this->code.clear();
            return ERROR("Unmatched [ found.");
        }

        return {};
    }

    void IR::optimize(void)
    {
        static std::vector<std::pair<std::vector<Instruction>, Instruction>> patterns = {
            {{   // 3j xa -1j
                Instruction{.type = Instruction::BF_INSTRUCTION_JMP, .arg = 3},
                Instruction{.type = Instruction::BF_INSTRUCTION_ADD, .arg = 0},
                Instruction{.type = Instruction::BF_INSTRUCTION_JMP, .arg = -1}
            }, Instruction{.type = Instruction::BF_INSTRUCTION_CLR, .arg = 0}}
        };

        bool found_sequence = false;
        do {
            found_sequence = false;
            for(auto sequence : patterns)
            {
                const size_t code_size = this->code.size();
                const size_t pattern_size = sequence.first.size();

                if(code_size < pattern_size) continue;

                bool is_sequence_correct = true;
                for(size_t index = 0; index < pattern_size && is_sequence_correct; index++)
                {
                    const Instruction& current_instruction = this->code[code_size - pattern_size + index];
                    if(!(current_instruction.type == sequence.first[index].type &&
                            (sequence.first[index].arg == current_instruction.arg || sequence.first[index].arg == 0)))
                            is_sequence_correct = false;
                }

                if(is_sequence_correct)
                {
                    if(sequence.second.arg >= 0)
                    {
                        size_t count = pattern_size;
                        while(count--) this->code.pop_back();
                        this->code.push_back(sequence.second);
                        found_sequence = true;
                        break;
                    }

                    bool ok = true;
                    Instruction replacement = sequence.second;
                    for(size_t index = 0; index < pattern_size && ok; index++)
                    {
                        if(sequence.first[index].arg == 0)
                        {
                            const Instruction& current_instruction = this->code[code_size - pattern_size + index];
                            replacement.arg = current_instruction.arg;
                            size_t count = pattern_size;
                            while(count--) this->code.pop_back();
                            this->code.push_back(replacement);
                            found_sequence = true;
                            break;
                        }
                    }
                }
            }
        } while(found_sequence == true);
    }

    std::string IR::dump(void)
    {
        std::stringstream out;
        size_t index = 0;
        for(const auto& instr : this->code)
        {
            out << index++ << ": " << Instruction::type_to_string(instr.type);
            if(instr.type < Instruction::BF_INSTRUCTION_INP || instr.type > Instruction::BF_INSTRUCTION_END)
                out << ' ' << instr.arg;
            out << '\n';
        }
        return std::move(out).str();
    }
};