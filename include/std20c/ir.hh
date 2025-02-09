#ifndef IR_HH
#define IR_HH
#include <string>
#include <vector>
#include <variant>
#include <set>

using VReg = std::size_t;

struct ImmediateAssignInstruction {
    VReg lhs;
    std::string value;
    ImmediateAssignInstruction(VReg lhs, std::string value): lhs(lhs), value(value) {}
};
struct RegisterAssignInstruction {
    VReg lhs;
    VReg rhs;
    RegisterAssignInstruction(VReg lhs, VReg rhs): lhs(lhs), rhs(rhs) {}
};
struct GenericWriteInstruction {
    VReg lhs;
    std::vector<std::variant<VReg, std::string>> rhs;
    GenericWriteInstruction(VReg lhs, std::initializer_list<std::variant<VReg, std::string>> list): lhs(lhs), rhs(list) {}
    GenericWriteInstruction(VReg lhs, std::vector<std::variant<VReg, std::string>> list): lhs(lhs), rhs(list) {}
};
struct GenericReadInstruction {
    std::vector<std::variant<VReg, std::string>> instruction;
    GenericReadInstruction(std::initializer_list<std::variant<VReg, std::string>> list): instruction(list) {}
    GenericReadInstruction(std::vector<std::variant<VReg, std::string>> list): instruction(list) {}
};
using Instruction = std::variant<ImmediateAssignInstruction, RegisterAssignInstruction, GenericWriteInstruction, GenericReadInstruction>;

template<typename ReadFn, typename WriteFn>
void visitVReg(const Instruction &ins, ReadFn read, WriteFn write) {
    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ImmediateAssignInstruction>) {
            write(arg.lhs);
        } else if constexpr (std::is_same_v<T, RegisterAssignInstruction>) {
            write(arg.lhs);
            read(arg.rhs);
        } else if constexpr (std::is_same_v<T, GenericWriteInstruction>) {
            write(arg.lhs);
            for (const auto& v : arg.rhs) {
                if (std::holds_alternative<VReg>(v)) {
                    read(std::get<VReg>(v));
                }
            }
        } else if constexpr (std::is_same_v<T, GenericReadInstruction>) {
            for (const auto& v : arg.instruction) {
                if (std::holds_alternative<VReg>(v)) {
                    read(std::get<VReg>(v));
                }
            }
        }
    }, ins);
}

template<typename ReadFn, typename WriteFn>
void visitVReg(Instruction &ins, ReadFn read, WriteFn write) {
    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ImmediateAssignInstruction>) {
            write(arg.lhs);
        } else if constexpr (std::is_same_v<T, RegisterAssignInstruction>) {
            write(arg.lhs);
            read(arg.rhs);
        } else if constexpr (std::is_same_v<T, GenericWriteInstruction>) {
            write(arg.lhs);
            for (auto& v : arg.rhs) {
                if (std::holds_alternative<VReg>(v)) {
                    read(std::get<VReg>(v));
                }
            }
        } else if constexpr (std::is_same_v<T, GenericReadInstruction>) {
            for (auto& v : arg.instruction) {
                if (std::holds_alternative<VReg>(v)) {
                    read(std::get<VReg>(v));
                }
            }
        }
    }, ins);
}

struct IR {
    std::set<VReg> virtualRegisters;
    std::vector<Instruction> instructions;
};


#endif

