#include "lifetime.hh"
#include "std20c/ir.hh"
#include <cstddef>
#include <map>
#include <optional>


LifeTimeChart generateLifetimes(const IR& ir) {
    LifeTimeChart chart(ir.instructions.size());
    std::map<VReg, size_t> lastWrite;
    std::map<VReg, std::optional<size_t>> lastRead;
    auto registerIsRead = [&](VReg reg, size_t position) {
        lastRead[reg] = position;
    };
    auto registerIsWritten = [&](VReg reg, size_t position) {
        if (lastRead[reg].has_value()) {
            // register was read in the past before it is written
            chart.registersBecomingAlive[lastWrite[reg]].push_back(reg);   // alive when it was written initially
            chart.registersBecomingDead[*lastRead[reg]].push_back(reg);    // dead last time it was read
        }
        lastRead[reg] = std::nullopt;   // reset read
        lastWrite[reg] = position;      // new written now
    };

    for (auto &e: ir.virtualRegisters) {
        lastWrite.emplace(e, 0);
        lastRead.emplace(e, std::optional<size_t>());
    }
    for (size_t i = 0; i < ir.instructions.size(); i++) {
        auto &ins = ir.instructions[i];
        visitVReg(ins, 
            [&](const VReg &v){ registerIsRead(v, i); }, 
            [&](const VReg &v){ registerIsWritten(v, i); });
    }
    for (auto &e: ir.virtualRegisters) {
        registerIsWritten(e, ir.instructions.size());
    }
    return chart;
}