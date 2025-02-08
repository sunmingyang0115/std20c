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
};
using Instruction = std::variant<ImmediateAssignInstruction, RegisterAssignInstruction, GenericWriteInstruction, GenericReadInstruction>;
struct IR {
    std::set<VReg> virtualRegisters;
    std::vector<Instruction> instructions;
};


#endif

